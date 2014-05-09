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


/** @file   networkutils.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the class 'NetworkUtils'
*/

#ifndef _MASH_NETWORKUTILS_H_
#define _MASH_NETWORKUTILS_H_

#include <mash-utils/data_buffer.h>
#include <string>
#include <sys/socket.h>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Network-related utility class
    //--------------------------------------------------------------------------
    class NetworkUtils
    {
    public:
        static void* getNetworkAddress(struct sockaddr* sa);

        static bool sendMessage(int socket, const std::string& strMessage,
                                const ArgumentsList& arguments);

        static bool sendData(int socket, const unsigned char* data, int size);

        static bool waitMessage(int socket, DataBuffer* pBuffer,
                                std::string* strMessage, ArgumentsList* arguments,
                                struct timeval* pTimeout = 0);

        static bool waitData(int socket, DataBuffer* pBuffer, unsigned char* data, int size);
    };
}

#endif
