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


/** @file   interactive_application_server.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'InteractiveApplicationServer' class
*/

#ifndef _MASH_INTERACTIVE_APPLICATION_SERVER_H_
#define _MASH_INTERACTIVE_APPLICATION_SERVER_H_

#include "interactive_listener.h"
#include <mash-network/server.h>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Manages the server side of an 'Interactive Application Server'
    ///
    /// An instance of an user-supplied class is created to handle each incoming
    /// connection (see IApplicationServer).
    //--------------------------------------------------------------------------
    class InteractiveApplicationServer
    {
        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Constructor
        ///
        /// @param  nbMaxClients    Maximum number of clients that this server
        ///                         can handle simultaneously. 0 means 'no limit'
        /// @param  logLimit        Maximum number of incoming connexions logged
        ///                         per file. The log file is deleted and a new
        ///                         one is created when that limit is reached.
        ///                         can handle simultaneously. 0 means 'no limit'
        /// @param  strName         Name of the server (used for the logs)
        //----------------------------------------------------------------------
        InteractiveApplicationServer(unsigned int nbMaxClients = 0,
                                     unsigned int logLimit = 100,
                                     const std::string& strName = "Server");

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        ~InteractiveApplicationServer();


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Start to listen for incoming communications of a port
        ///
        /// @param  host                            The host name or IP address
        ///                                         that the server object must
        ///                                         listen to. If empty, the
        ///                                         first available is used.
        /// @param  port                            The port that the server
        ///                                         object must listen to
        /// @param  applicationServerConstructor    Pointer to the function to
        ///                                         use to create the application
        ///                                         server implementation that
        ///                                         will handle the incoming
        ///                                         connections
        /// @param  bVerbose                        Verbose mode
        /// @return                                 'false' if failed
        ///
        /// @remark Blocking call
        //----------------------------------------------------------------------
        bool listen(const std::string& host, unsigned int port,
                    tApplicationServerConstructor* applicationServerConstructor,
                    bool bVerbose = false, struct timeval* pTimeout = 0);


        //----------------------------------------------------------------------
        /// @brief  Returns the version of the protocol supported
        //----------------------------------------------------------------------
        std::string getProtocol() const;


        //_____ Attributes __________
    private:
        Server _server;
    };
}

#endif
