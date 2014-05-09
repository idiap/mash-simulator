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


/** @file   server_listener.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'ServerListener' class
*/

#include "server.h"
#include "networkutils.h"
#include <assert.h>

using namespace std;
using namespace Mash;


/************************* CONSTRUCTION / DESTRUCTION *************************/

ServerListener::ServerListener(int socket)
: _socket(socket)
{
    _timeout.tv_sec = 0;
    _timeout.tv_usec = 0;
}


ServerListener::~ServerListener()
{
    _outStream.deleteFile();
}


/*********************************** METHODS **********************************/

ServerListener::tAction ServerListener::process()
{
    // Declarations
    std::string strCommand;
    ArgumentsList arguments;

    struct timeval* pTimeout = 0;

    if ((_timeout.tv_sec > 0) || (_timeout.tv_usec > 0))
        pTimeout = &_timeout;

    while (NetworkUtils::waitMessage(_socket, &_buffer, &strCommand, &arguments, pTimeout))
    {
        // Timeout ?
        if (pTimeout && strCommand.empty())
        {
            onTimeout();
            continue;
        }

        _outStream << "< " << strCommand;

        for (unsigned int i = 0; i < arguments.size(); ++i)
            _outStream << " " << arguments.getString(i);

        _outStream << endl;

        tAction action = handleCommand(strCommand, arguments);

        if (action != ACTION_NONE)
            return action;
    }

    return ACTION_NONE;
}


bool ServerListener::sendResponse(const std::string& strResponse,
                                  const ArgumentsList& arguments)
{
    _outStream << "> " << strResponse;

    for (unsigned int i = 0; i < arguments.size(); ++i)
        _outStream << " " << arguments.getString(i);

    _outStream << endl;

    return NetworkUtils::sendMessage(_socket, strResponse, arguments);
}


bool ServerListener::sendData(const unsigned char* data, int size)
{
    _outStream << "> <" << size << " bytes of data>" << endl;

    return NetworkUtils::sendData(_socket, data, size);
}


bool ServerListener::waitData(unsigned char* data, int size)
{
    bool bResult = NetworkUtils::waitData(_socket, &_buffer, data, size);

    if (bResult)
        _outStream << "< <" << size << " bytes of data>" << endl;

    return bResult;
}
