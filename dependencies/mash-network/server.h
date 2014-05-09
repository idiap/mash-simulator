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


/** @file   server.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'Server' class
*/

#ifndef _MASH_SERVER_H_
#define _MASH_SERVER_H_

#include <mash-utils/arguments_list.h>
#include <mash-utils/outstream.h>
#include "server_listener.h"
#include <vector>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Manages the server side of a TCP/IP communication
    ///
    /// A 'listener' is created to handle each incoming connection (see
    /// ServerListener).
    //--------------------------------------------------------------------------
    class MASH_SYMBOL Server
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
        ///                         0 means 'no limit'
        /// @param  strName         Name of the server (used for the logs)
        //----------------------------------------------------------------------
        Server(unsigned int nbMaxClients = 0, unsigned int logLimit = 100,
               const std::string& strName = "Server");

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        ~Server();


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Start to listen for incoming communications of a port
        ///
        /// @param  host                    The host name or IP address that the
        ///                                 server object must listen to. If
        ///                                 empty, the first available is used.
        /// @param  port                    The port that the server object must
        ///                                 listen to
        /// @param  listenerConstructor     Pointer to the function to use to
        ///                                 create the listener that will handle
        ///                                 the incoming connections
        /// @return                         'false' if failed
        ///
        /// @remark Blocking call
        //----------------------------------------------------------------------
        bool listen(const std::string& host, unsigned int port,
                    tServerListenerConstructor* listenerConstructor);


    private:
        //----------------------------------------------------------------------
        /// @brief  Add a client to the list
        ///
        /// @param[out] serverPipe      Pipe that must be used by the server to
        ///                             test if the client is done
        /// @param[out] listenerPipe    Pipe that must be used by the listener to
        ///                             indicate to the server that the client
        ///                             is done
        //----------------------------------------------------------------------
        void addClient(int* serverPipe, int* listenerPipe);

        //----------------------------------------------------------------------
        /// @brief  Clear the internal list of connected clients
        //----------------------------------------------------------------------
        void clearClientsList();


        //_____ Internal types __________
    private:
        enum tState
        {
            STATE_NORMAL,
            STATE_GOING_TO_SLEEP,
            STATE_SLEEPING
        };


        //_____ Static attributes __________
    public:
        static std::string  strLogFolder;


        //_____ Attributes __________
    private:
        tState              _state;
        unsigned int        _nbMaxClients;
        unsigned int        _logLimit;
        std::vector<int>    _pipesList;
        OutStream           _outStream;
    };
}

#endif
