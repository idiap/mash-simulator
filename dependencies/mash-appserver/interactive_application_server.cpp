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


/** @file   interactive_application_server.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'InteractiveApplicationServer' class
*/

#include "interactive_application_server.h"
#include "interactive_listener.h"

using namespace std;
using namespace Mash;


/************************* CONSTRUCTION / DESTRUCTION *************************/

InteractiveApplicationServer::InteractiveApplicationServer(unsigned int nbMaxClients,
                                                           unsigned int logLimit,
                                                           const std::string& strName)
: _server(nbMaxClients, logLimit, strName)
{
}


InteractiveApplicationServer::~InteractiveApplicationServer()
{
}


/*********************************** METHODS **********************************/

bool InteractiveApplicationServer::listen(const std::string& host, unsigned int port,
                tApplicationServerConstructor* applicationServerConstructor,
                bool bVerbose, struct timeval* pTimeout)
{
    // Initialize the listener
    InteractiveListener::initialize(bVerbose, applicationServerConstructor, pTimeout);

    OutStream::verbosityLevel = (bVerbose ? 1 : 0);

    // Start handling requests from clients
    return _server.listen(host, port, InteractiveListener::createListener);
}


std::string InteractiveApplicationServer::getProtocol() const
{
    return InteractiveListener::getProtocol();
}
