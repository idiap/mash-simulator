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


/** @file   client.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'Client' class
*/

#ifndef _MASH_CLIENT_H_
#define _MASH_CLIENT_H_

#include <mash-utils/arguments_list.h>
#include <mash-utils/outstream.h>
#include <mash-utils/data_buffer.h>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Manages the client side of a TCP/IP communication
    ///
    /// The client sends commands to the server, which sends responses back. The
    /// server never spontaneously sends data to the client.
    //--------------------------------------------------------------------------
    class MASH_SYMBOL Client
    {
        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Constructor
        ///
        /// @param  pOutStream  The output stream to use (if 0, nothing is
        ///                     reported)
        //----------------------------------------------------------------------
        Client(OutStream* pOutStream = 0);

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        ~Client();


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Initiate a connection to a server
        ///
        /// @param  strAddress  The address of the server
        /// @param  port        The port of the server
        /// @return             'false' if failed
        //----------------------------------------------------------------------
        bool connect(const std::string& strAddress, unsigned int port);

        //----------------------------------------------------------------------
        /// @brief  Send a command to the server
        ///
        /// @param  strCommand  The command
        /// @param  arguments   The arguments of the command
        /// @return             'false' if failed
        //----------------------------------------------------------------------
        bool sendCommand(const std::string& strCommand,
                         const ArgumentsList& arguments);

        //----------------------------------------------------------------------
        /// @brief  Send some binary data to the server
        ///
        /// Be sure that the server is waiting for binary data (by sending the
        /// appropriate command, according to your protocol), or it might try to
        /// interpret the data as a command.
        /// @param  data    The data
        /// @param  size    Size of the data, in bytes
        /// @return         'false' if failed
        //----------------------------------------------------------------------
        bool sendData(const unsigned char* data, int size);

        //----------------------------------------------------------------------
        /// @brief  Wait for a response from the server
        ///
        /// @param[out] strResponse The response
        /// @param[out] arguments   The arguments of the response
        /// @return                 'false' if failed
        //----------------------------------------------------------------------
        bool waitResponse(std::string* strResponse, ArgumentsList* arguments);

        //----------------------------------------------------------------------
        /// @brief  Wait for some binary data from the server
        ///
        /// You must know the size (in bytes) of the data before calling this
        /// method. The protocols ensure that you have this information.
        /// @param[out] data    The data (you must allocate the array yourself)
        /// @param      size    Size of the data, in bytes
        /// @return             'false' if failed
        //----------------------------------------------------------------------
        bool waitData(unsigned char* data, int size);

        //----------------------------------------------------------------------
        /// @brief  Close the connection to the server
        //----------------------------------------------------------------------
        void close();


        //_____ Attributes __________
    private:
        int         _socket;
        DataBuffer  _buffer;
        OutStream   _outStream;
    };
}

#endif
