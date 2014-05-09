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


/** @file   server_listener.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'ServerListener' class
*/

#ifndef _MASH_SERVERLISTENER_H_
#define _MASH_SERVERLISTENER_H_

#include <mash-utils/arguments_list.h>
#include <mash-utils/outstream.h>
#include <mash-utils/data_buffer.h>
#include <string>
#include <sys/time.h>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Base class for a listener that handle an incoming connection
    ///         (see Server)
    //--------------------------------------------------------------------------
    class MASH_SYMBOL ServerListener
    {
        //_____ Internal types __________
    public:
        //----------------------------------------------------------------------
        /// @brief  The actions that a listener might ask the server to perform
        //----------------------------------------------------------------------
        enum tAction
        {
            ACTION_NONE,                ///< No specific action
            ACTION_CLOSE_CONNECTION,    ///< Close the connection with the client
            ACTION_SLEEP,               ///< The server must go to sleep
        };


        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Constructor
        ///
        /// @param  socket  Socket used b the connection handled by the listener
        //----------------------------------------------------------------------
        ServerListener(int socket);

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        virtual ~ServerListener();


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Wait for some commands and process them
        /// @return The action that must be performed by the server
        //----------------------------------------------------------------------
        tAction process();


        //----------------------------------------------------------------------
        /// @brief  Send a response to the client
        ///
        /// @param  strResponse The response
        /// @param  arguments   The arguments of the response
        /// @return             'false' if failed
        //----------------------------------------------------------------------
        bool sendResponse(const std::string& strResponse,
                          const ArgumentsList& arguments);


        //----------------------------------------------------------------------
        /// @brief  Send some binary data to the client
        ///
        /// Be sure that the client is waiting for binary data (by sending the
        /// appropriate response, according to your protocol), or it might try to
        /// interpret the data as a response.
        /// @param  data    The data
        /// @param  size    Size of the data, in bytes
        /// @return         'false' if failed
        //----------------------------------------------------------------------
        bool sendData(const unsigned char* data, int size);

        //----------------------------------------------------------------------
        /// @brief  Wait for some binary data from the client
        ///
        /// You must know the size (in bytes) of the data before calling this
        /// method. The protocols ensure that you have this information.
        /// @param[out] data    The data (you must allocate the array yourself)
        /// @param      size    Size of the data, in bytes
        /// @return             'false' if failed
        //----------------------------------------------------------------------
        bool waitData(unsigned char* data, int size);


        //_____ Methods to implement __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Called when a command was received
        ///
        /// @param  strCommand  The command
        /// @param  arguments   The arguments of the command
        /// @return             The action that must be performed by the server
        //----------------------------------------------------------------------
        virtual tAction handleCommand(const std::string& strCommand,
                                      const ArgumentsList& arguments) = 0;

        //----------------------------------------------------------------------
        /// @brief  Called when a timeout occured while waiting for a command
        ///
        /// @remark Overriden classes that want to use the timeout mechanism
        ///         must set the _timeout attribute to a non-zero value
        //----------------------------------------------------------------------
        virtual void onTimeout() {}


        //_____ Attributes __________
    protected:
        int             _socket;
        DataBuffer      _buffer;
        struct timeval  _timeout;
        OutStream       _outStream;
    };


    //--------------------------------------------------------------------------
    /// @brief  Construction function for a listener
    //--------------------------------------------------------------------------
    typedef ServerListener* tServerListenerConstructor(int);
}

#endif
