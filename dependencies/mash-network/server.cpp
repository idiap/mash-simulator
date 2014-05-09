/*******************************************************************************
* MASH 3D simulator
* 
* Copyright (c) 2014 Idiap Research Institute, http://www.idiap.ch/
* Written by Philip Abbet <philip.abbet@idiap.ch>
* 
* This file is part of mash-simulator.
* 
* mash-simulator is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 3 as
* published by the Free Software Foundation.
* 
* mash-simulator is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with mash-simulator. If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/


/** @file   server.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'Server' class
*/

#include "server.h"
#include "networkutils.h"
#include "busy_listener.h"
#include <mash-utils/stringutils.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;
using namespace Mash;


/********************************** FUNCTIONS *********************************/

void sigchld_handler(int s)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}


/****************************** STATIC ATTRIBUTES *****************************/

string Server::strLogFolder = "logs/";


/************************* CONSTRUCTION / DESTRUCTION *************************/

Server::Server(unsigned int nbMaxClients, unsigned int logLimit,
               const std::string& strName)
: _state(STATE_NORMAL), _nbMaxClients(nbMaxClients), _logLimit(logLimit)
{
    _outStream.open(strName, strLogFolder + strName + "-$TIMESTAMP.log", 200 * 1024);
}


Server::~Server()
{
}


/*********************************** METHODS **********************************/

bool Server::listen(const std::string& host, unsigned int port,
                    tServerListenerConstructor* listenerConstructor)
{
    // Assertions
    assert(listenerConstructor);

    // Declarations
    int listen_socket;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    unsigned int clients_counter = 0;

    if (!host.empty())
        _outStream << "Start to listen for incoming connections on '" << host << ":" << port << "'" << endl;
    else
        _outStream << "Start to listen for incoming connections on port " << port << endl;

    if (_nbMaxClients > 0)
        _outStream << "This server only supports " << _nbMaxClients << " client(s) at the same time" << endl;
    else
        _outStream << "This server supports an unlimited amount of clients" << endl;

    // Retrieve a list of our addresses
    memset(&hints, 0, sizeof hints);
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_flags      = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo((host.empty() ? NULL : host.c_str()), StringUtils::toString(port).c_str(), &hints, &servinfo)) != 0)
    {
        _outStream << "ERROR - Failed to retrieve a list of our addresses: " << gai_strerror(rv) << endl;
        return false;
    }

    // Loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((listen_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            _outStream << "ERROR - Failed to create a socket" << endl;
            continue;
        }

        if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            _outStream << "ERROR - Failed to set the socket options" << endl;
            return false;
        }

        if (bind(listen_socket, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(listen_socket);
            _outStream << "ERROR - Failed to bind the socket" << endl;
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        _outStream << "ERROR - Failed to create the socket of the server" << endl;
        return false;
    }

    freeaddrinfo(servinfo);


    // Start listening
    if (::listen(listen_socket, 10) == -1)
    {
        _outStream << "ERROR - Failed to listen for incoming connections" << endl;
        return false;
    }

    // Reap all dead processes
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        _outStream << "ERROR - Failed to setup the dead processes destruction mecanism" << endl;
        return false;
    }

    while (true)
    {
        // Delete the log file and starts a new one when the limit is reached
        if ((clients_counter >= _logLimit) && _pipesList.empty())
        {
            _outStream << "--------------------------------------------------------------------------------" << endl;

            string strLogName = _outStream.getName();
            string strLogFileName = _outStream.getFileName();

            _outStream.deleteFile();
            _outStream.open(strLogName, strLogFileName);

            time_t t;
            struct tm* timeinfo;
            char buffer[20];

            time(&t);
            timeinfo = localtime(&t);

            strftime(buffer, 20, "%d/%m/%Y %H:%M:%S", timeinfo);

            _outStream << "Reset of the log file: " << buffer << endl;

            if (!host.empty())
                _outStream << "Listen for incoming connections on '" << host << ":" << port << "'" << endl;
            else
                _outStream << "Listen for incoming connections on port " << port << endl;

            if (_nbMaxClients > 0)
                _outStream << "This server only supports " << _nbMaxClients << " client(s) at the same time" << endl;
            else
                _outStream << "This server supports an unlimited amount of clients" << endl;

            clients_counter = 0;
        }


        _outStream << "--------------------------------------------------------------------------------" << endl
                   << "Waiting..." << endl;


        // Wait for some events
        fd_set readfds;
        std::vector<int>::iterator iter, iterEnd;
        int max = 0;

        FD_ZERO(&readfds);
        for (iter = _pipesList.begin(), iterEnd = _pipesList.end(); iter != iterEnd; ++iter)
        {
            FD_SET(*iter, &readfds);

            if (*iter > max)
                max = *iter;
        }

        FD_SET(listen_socket, &readfds);

        if (listen_socket > max)
            max = listen_socket;

        int nb = 0;
        while (nb <= 0)
            nb = select(max + 1, &readfds, 0, 0, 0);

        // Process the events coming from the listeners
        std::vector<int> tmp = _pipesList;
        _pipesList.clear();

        for (iter = tmp.begin(), iterEnd = tmp.end(); iter != iterEnd; ++iter)
        {
            if (FD_ISSET(*iter, &readfds))
            {
                char buffer[6] = { 0 };
                ssize_t count = read(*iter, buffer, 6);

                if (strcmp("SLEEP", buffer) == 0)
                {
                    _outStream << "Going to sleep..." << endl;
                    _state = STATE_GOING_TO_SLEEP;
                }

                close(*iter);
            }
            else
            {
                _pipesList.push_back(*iter);
            }
        }

        if (tmp.size() != _pipesList.size())
        {
            unsigned int diff = tmp.size() - _pipesList.size();
            if (diff > 1)
                _outStream << diff << " client(s) are done" << endl;
            else
                _outStream << "One client is done" << endl;
        }

        if ((_state == STATE_GOING_TO_SLEEP) && _pipesList.empty())
        {
            _outStream << "Sleeping..." << endl;
            _state = STATE_SLEEPING;
        }

        // Process the incoming connections
        if (!FD_ISSET(listen_socket, &readfds))
            continue;

        ++clients_counter;

        sin_size = sizeof(their_addr);
        int child_socket = accept(listen_socket, (struct sockaddr*) &their_addr, &sin_size);
        if (child_socket == -1)
        {
            _outStream << "ERROR - Failed to accept an incoming connection" << endl;
            continue;
        }

        inet_ntop(their_addr.ss_family, NetworkUtils::getNetworkAddress((struct sockaddr*) &their_addr),
                  s, sizeof(s));

        if (their_addr.ss_family == AF_INET)
            _outStream << "Incoming connection from " << s << ":" << ((struct sockaddr_in*) &their_addr)->sin_port << endl;
        else
            _outStream << "Incoming connection from " << s << ":" << ((struct sockaddr_in6*) &their_addr)->sin6_port << endl;


        // Determine if we can handle this client
        bool bBusy = false;
        int serverPipe, listenerPipe;
        if (_state == STATE_SLEEPING)
        {
            bBusy = true;
            unsigned int nb = (unsigned int) _pipesList.size() + 1;
            _outStream << "The server is sleeping, there is currently " << nb << " clients connected" << endl;
        }
        else if (_state == STATE_GOING_TO_SLEEP)
        {
            bBusy = true;
            unsigned int nb = (unsigned int) _pipesList.size() + 1;
            _outStream << "The server is going to sleep, there is still " << nb << " clients connected" << endl;
        }
        else if (_nbMaxClients > 0)
        {
            bBusy = (_pipesList.size() >= _nbMaxClients);

            unsigned int nb = (unsigned int) _pipesList.size();

            if (bBusy)
                _outStream << "The server is busy (" << nb << "/" << _nbMaxClients << " client(s) connected)" << endl;
            else
                _outStream << "The server is available (" << nb << "/" << _nbMaxClients << " client(s) connected)" << endl;
        }
        else
        {
            unsigned int nb = (unsigned int) _pipesList.size() + 1;
            _outStream << "There is currently " << nb << " clients connected" << endl;
        }

        addClient(&serverPipe, &listenerPipe);


        if (!fork())
        {
            // This is the child process
            close(listen_socket);
            clearClientsList();

            struct linger lingerOpt;
            lingerOpt.l_onoff = 1;
            lingerOpt.l_onoff = 100;
            setsockopt(child_socket, SOL_SOCKET, SO_LINGER, &lingerOpt, sizeof(lingerOpt));

            ServerListener* pListener;

            if ((_state == STATE_NORMAL) && !bBusy)
                pListener = listenerConstructor(child_socket);
            else
                pListener = new BusyListener(child_socket, listenerConstructor);

            ServerListener::tAction action = pListener->process();

            delete pListener;

            close(child_socket);

            // Notify the server that the listener is done
            int flags = fcntl(listenerPipe, F_GETFD, 0);
            fcntl(listenerPipe, F_SETFD, flags | O_NONBLOCK);

            if (action == ServerListener::ACTION_SLEEP)
                write(listenerPipe, "SLEEP", 6);
            else
                write(listenerPipe, "DONE", 5);

            close(listenerPipe);

            exit(0);
        }


        // Parent doesn't need those
        close(child_socket);
        close(listenerPipe);
    }

    close(listen_socket);
    clearClientsList();

    return true;
}


void Server::addClient(int* serverPipe, int* listenerPipe)
{
    // Assertions
    assert(serverPipe);
    assert(listenerPipe);

    int pipes[2];
    pipe(pipes);

    *serverPipe = pipes[0];
    *listenerPipe = pipes[1];

    _pipesList.push_back(*serverPipe);
}


void Server::clearClientsList()
{
    std::vector<int>::iterator iter, iterEnd;
    for (iter = _pipesList.begin(), iterEnd = _pipesList.end(); iter != iterEnd; ++iter)
        close(*iter);

    _pipesList.clear();
}
