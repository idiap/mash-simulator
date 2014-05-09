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


/** @file   data_buffer.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the class 'DataBuffer'
*/

#ifndef _MASH_DATABUFFER_H_
#define _MASH_DATABUFFER_H_

#include <mash-utils/arguments_list.h>
#include <string>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Buffer of data
    //--------------------------------------------------------------------------
    class DataBuffer
    {
        //_____ Constructor / Destructor __________
    public:
        DataBuffer();
        DataBuffer(const std::string& str);
        DataBuffer(const unsigned char* pData, unsigned int dataSize);
        ~DataBuffer();


        //_____ Methods __________
    public:
        void add(const std::string& str);
        void add(const unsigned char* pData, unsigned int dataSize);

        void extract(unsigned char* pDest, unsigned int nbBytes);
        bool extractLine(std::string &strLine);

        void reset();

        inline unsigned int size() const
        {
            return _size;
        }

        bool extractMessage(std::string* strMessage, ArgumentsList* arguments);

        static std::string encodeArgument(const std::string& strArgument);
        static std::string decodeArgument(const std::string& strArgument);

    private:
        void reallocate(unsigned int nbBytesToAdd);


        //_____ Attributes __________
    private:
        unsigned char*  _data;
        unsigned int    _size;
        unsigned int    _allocated;
    };
}

#endif
