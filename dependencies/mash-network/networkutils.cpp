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


/** @file   networkutils.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the class 'NetworkUtils'
*/

#include "networkutils.h"
#include <netinet/in.h>
#include <memory.h>
#include <assert.h>
#include <iostream>
#include <errno.h>


using namespace Mash;
using namespace std;



void* NetworkUtils::getNetworkAddress(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*) sa)->sin_addr);

    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}


bool NetworkUtils::sendMessage(int socket, const std::string& strMessage,
                               const ArgumentsList& arguments)
{
    // Assertions
    assert(socket >= 0);
    assert(!strMessage.empty());

    // Declarations
    int total;
    int bytesleft;
    const char* pSrc;

    // Build the line that will be sent
    string data = strMessage;
    for (int i = 0; i < arguments.size(); ++i)
    {
        string arg = arguments.getString(i);

        bool bMustQuote = false;
        string mod = DataBuffer::encodeArgument(arg);
        if (mod != arg)
        {
            arg = mod;
            bMustQuote = true;
        }

        if (bMustQuote || (arg.find(' ') != string::npos))
            arg = "'" + arg + "'";

        data += " " + arg;
    }
    data += "\n";
    pSrc = data.c_str();

    // Send the response to the client
    total = 0;
    bytesleft = data.length();
    while (total < data.length())
    {
        errno = 0;
        int n = send(socket, pSrc + total, bytesleft, 0);
        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            return false;
        }

        total += n;
        bytesleft -= n;
    }

    return true;
}


bool NetworkUtils::sendData(int socket, const unsigned char* data, int size)
{
    // Assertions
    assert(socket >= 0);
    assert(data);
    assert(size > 0);

    // Declarations
    int total;
    int bytesleft;

    // Send the response to the client
    total = 0;
    bytesleft = size;
    while (total < size)
    {
        errno = 0;
        int n = send(socket, data + total, bytesleft, 0);
        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            return false;
        }

        total += n;
        bytesleft -= n;
    }

    return true;
}


bool NetworkUtils::waitMessage(int socket, DataBuffer* pBuffer,
                               std::string* strMessage, ArgumentsList* arguments,
                               struct timeval* pTimeout)
{
    // Assertions
    assert(socket >= 0);
    assert(pBuffer);
    assert(strMessage);
    assert(arguments);

    const int MAXDATASIZE = 256;

    // Declarations
    char buf[MAXDATASIZE];
    fd_set readfds;
    int nbBytes;

    FD_ZERO(&readfds);

    arguments->clear();

    if (pBuffer->extractMessage(strMessage, arguments))
        return true;

    while (true)
    {
        FD_SET(socket, &readfds);
        errno = 0;

        int ret = select(socket + 1, &readfds, NULL, NULL, pTimeout);

        // Timeout?
        if (ret == 0)
        {
            *strMessage = "";
            return true;
        }

        if (FD_ISSET(socket, &readfds))
        {
            nbBytes = recv(socket, buf, MAXDATASIZE - 1, 0);
            if (nbBytes > 0)
            {
                buf[nbBytes] = 0;
                pBuffer->add((const unsigned char*) buf, nbBytes);

                if (pBuffer->extractMessage(strMessage, arguments))
                    return true;
            }
            else if (errno == EINTR)
            {
                continue;
            }
            else
            {
                // Connection closed or error
                return false;
            }
        }
    }
}


bool NetworkUtils::waitData(int socket, DataBuffer* pBuffer, unsigned char* data, int size)
{
    // Assertions
    assert(socket >= 0);
    assert(pBuffer);
    assert(data);
    assert(size > 0);

    // Declarations
    fd_set readfds;
    int nbBytes;
    int total = 0;
    int bytesleft = size;

    if (pBuffer->size() >= size)
    {
        pBuffer->extract(data, size);
        return true;
    }
    else if (pBuffer->size() > 0)
    {
        total = pBuffer->size();
        bytesleft -= total;

        pBuffer->extract(data, pBuffer->size());
    }

    FD_ZERO(&readfds);

    while (true)
    {
        FD_SET(socket, &readfds);
        errno = 0;

        select(socket + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(socket, &readfds))
        {
            nbBytes = recv(socket, data + total, bytesleft, 0);
            if (nbBytes > 0)
            {
                total += nbBytes;
                bytesleft -= nbBytes;

                if (bytesleft == 0)
                    return true;
            }
            else if (errno == EINTR)
            {
                continue;
            }
            else
            {
                // Connection closed or error
                return false;
            }
        }
    }
}
