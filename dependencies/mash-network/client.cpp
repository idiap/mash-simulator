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


/** @file   client.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'Client' class
*/

#include "client.h"
#include "networkutils.h"
#include <mash-utils/stringutils.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>

using namespace std;
using namespace Mash;


/************************* CONSTRUCTION / DESTRUCTION *************************/

Client::Client(OutStream* pOutStream)
: _socket(-1)
{
    if (pOutStream)
        _outStream = *pOutStream;
}


Client::~Client()
{
    if (_socket != -1)
        close();
}


/*********************************** METHODS **********************************/

bool Client::connect(const std::string& strAddress, unsigned int port)
{
    // Assertions
    assert(!strAddress.empty());

    // Declarations
    int numbytes;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    _outStream << "Trying to establish a connection to '" << strAddress << ":" << port << "'" << endl;

    // Retrieve a list of the server addresses
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(strAddress.c_str(), StringUtils::toString(port).c_str(), &hints, &servinfo)) != 0)
    {
        _outStream << "ERROR - Failed to retrieve a list of the server addresses: " << gai_strerror(rv) << endl;
        return false;
    }

    // Loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            _outStream << "ERROR - Failed to create a socket" << endl;
            continue;
        }

        if (::connect(_socket, p->ai_addr, p->ai_addrlen) == -1)
        {
            ::close(_socket);
            _socket = -1;

            _outStream << "ERROR - Failed to connect with the server" << endl;
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        _outStream << "ERROR - Failed to establish a connection with the server" << endl;
        return false;
    }

    inet_ntop(p->ai_family, NetworkUtils::getNetworkAddress((struct sockaddr*) p->ai_addr),
              s, sizeof s);

    _outStream << "Connection established with the server at '" << s << ":" << port << "'" << endl;

    freeaddrinfo(servinfo);

    _buffer.reset();

    return true;
}


bool Client::sendCommand(const std::string& strCommand,
                         const ArgumentsList& arguments)
{
    _outStream << "> " << strCommand;

    for (unsigned int i = 0; i < arguments.size(); ++i)
        _outStream << " " << arguments.getString(i);

    _outStream << endl;

    return NetworkUtils::sendMessage(_socket, strCommand, arguments);
}


bool Client::sendData(const unsigned char* data, int size)
{
    _outStream << "> <" << size << " bytes of data>" << endl;

    return NetworkUtils::sendData(_socket, data, size);
}


bool Client::waitResponse(std::string* strResponse, ArgumentsList* arguments)
{
    bool bResult = NetworkUtils::waitMessage(_socket, &_buffer, strResponse, arguments);

    if (bResult)
    {
        _outStream << "< " << strResponse->c_str();

        for (unsigned int i = 0; i < arguments->size(); ++i)
            _outStream << " " << arguments->getString(i);

        _outStream << endl;
    }

    return bResult;
}


bool Client::waitData(unsigned char* data, int size)
{
    bool bResult = NetworkUtils::waitData(_socket, &_buffer, data, size);

    if (bResult)
        _outStream << "< <" << size << " bytes of data>" << endl;

    return bResult;
}


void Client::close()
{
    ::close(_socket);
    _socket = -1;
}
