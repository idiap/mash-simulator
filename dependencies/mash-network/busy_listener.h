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


/** @file   busy_listener.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'BusyListener' class
*/

#ifndef _MASH_BUSYLISTENER_H_
#define _MASH_BUSYLISTENER_H_

#include "server_listener.h"


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Listener used by the servers when they can't handle an incoming
    ///         connection because there is too many clients connected already
    ///         (see Server)
    //--------------------------------------------------------------------------
    class MASH_SYMBOL BusyListener: public ServerListener
    {
        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Constructor
        ///
        /// @param  socket                  Socket used b the connection handled
        ///                                 by the listener
        /// @param  stdListenerConstructor  Constructor of the listener normally
        ///                                 used by the server
        //----------------------------------------------------------------------
        BusyListener(int socket, tServerListenerConstructor* stdListenerConstructor);

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        virtual ~BusyListener();


        //_____ Implementation of BusyListener __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Called when a command was received
        ///
        /// @param  strCommand  The command
        /// @param  arguments   The arguments of the command
        /// @return             The action that must be performed by the server
        //----------------------------------------------------------------------
        virtual tAction handleCommand(const std::string& strCommand,
                                      const ArgumentsList& arguments);


        //_____ Attributes __________
    protected:
        tServerListenerConstructor* _stdListenerConstructor;
    };
}

#endif
