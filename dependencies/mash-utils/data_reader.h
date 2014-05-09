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


/** @file   data_reader.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'DataReader' class
*/

#ifndef _MASH_DATAREADER_H_
#define _MASH_DATAREADER_H_

#include "declarations.h"
#include "stringutils.h"
#include <stdint.h>
#include <sys/time.h>
#include <string>
#include <fstream>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Used to read data from a file
    //--------------------------------------------------------------------------
    class MASH_SYMBOL DataReader
    {
        //_____ Internal types __________
    public:
        enum tPosition
        {
            BEGIN,
            CURRENT,
            END,
        };


        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Constructor
        //----------------------------------------------------------------------
        DataReader();

        //----------------------------------------------------------------------
        /// @brief  Constructor
        ///
        /// @param  ref     Data reader to copy
        /// @param  offset  Number of bytes to skip (they will not be readable
        ///                 at all)
        //----------------------------------------------------------------------
        DataReader(const DataReader& ref, int64_t offset);

        //----------------------------------------------------------------------
        /// @brief  Copy constructor
        //----------------------------------------------------------------------
        DataReader(const DataReader& ref);

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        ~DataReader();


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Open a file
        ///
        /// @param  strFileName     Path to the file
        /// @param  offset          Number of bytes to skip (they will not be
        ///                         readable at all)
        /// @return                 'true' if successful
        //----------------------------------------------------------------------
        bool open(const std::string& strFileName, int64_t offset = 0);

        //----------------------------------------------------------------------
        /// @brief  Close the file
        //----------------------------------------------------------------------
        void close();

        //----------------------------------------------------------------------
        /// @brief  Delete the file
        //----------------------------------------------------------------------
        void deleteFile();

        //----------------------------------------------------------------------
        /// @brief  Indicates if the file is open
        //----------------------------------------------------------------------
        inline bool isOpen() const
        {
            return (_pStream != 0);
        }

        //----------------------------------------------------------------------
        /// @brief  Assignment operator
        //----------------------------------------------------------------------
        void operator=(const DataReader& ref);

        //----------------------------------------------------------------------
        /// @brief  Returns the name of the file
        //----------------------------------------------------------------------
        inline std::string getFileName() const
        {
            return (_pStream ? _pStream->strFileName : "");
        }


        //_____ Methods to read data __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Move the reading cursor in the file
        ///
        /// @param  offset  The offset to move by
        /// @param  origin  The origin from where the offset is computed
        //----------------------------------------------------------------------
        void seek(int64_t offset, tPosition origin = CURRENT);

        //----------------------------------------------------------------------
        /// @brief  Returns the position of the reading cursor in the file
        //----------------------------------------------------------------------
        int64_t tell();

        //----------------------------------------------------------------------
        /// @brief  Returns the size of the file
        //----------------------------------------------------------------------
        inline int64_t size() const
        {
            return (_pStream ? totalSize() - _offset: 0);
        }

        //----------------------------------------------------------------------
        /// @brief  Indicates if the reading cursor is at the endo of the file
        //----------------------------------------------------------------------
        inline bool eof()
        {
            return (_pStream ? _current >= totalSize() - _offset : true);
        }

        //----------------------------------------------------------------------
        /// @brief  Read a block of data from the file
        ///
        /// @param[out] pData   Buffer where the data will be stored (must be
        ///                     large enough!)
        /// @param      size    The maximum number of bytes to read
        /// @return             The number of bytes actually in the buffer
        //----------------------------------------------------------------------
        int64_t read(int8_t* pData, int64_t size);

        //----------------------------------------------------------------------
        /// @brief  Read a block of data from the file
        ///
        /// @param[out] pData   Buffer where the data will be stored (must be
        ///                     large enough!)
        /// @param      size    The maximum number of bytes to read
        /// @return             The number of bytes actually in the buffer
        //----------------------------------------------------------------------
        inline int64_t read(uint8_t* pData, int64_t size)
        {
            return read((int8_t*) pData, size);
        }

        //----------------------------------------------------------------------
        /// @brief  Reads and returns the next byte without extracting it, i.e.
        ///         leaving it as the next byte to be extracted from the stream
        ///
        /// @return The next byte of data
        //----------------------------------------------------------------------
        uint8_t peek();

        //----------------------------------------------------------------------
        /// @brief  Read a line of data from the file
        ///
        /// A line ends with a '\n' character. The '\n' character is discarded.
        /// @param[out] pData   Buffer where the line will be stored (must be
        ///                     large enough!)
        /// @param      size    The maximum number of bytes to read
        /// @return             The number of bytes actually in the buffer
        //----------------------------------------------------------------------
        int64_t readline(char* pData, int64_t size);


        template <typename TYPE>
        DataReader& operator>> (TYPE &val)
        {
            if (_pStream)
                _pStream->stream >> val;
            return *this;
        }


        inline DataReader& operator>> (char &val)
        {
            int16_t v;
            operator>>(v);
            val = v;
            return *this;
        }

        inline DataReader& operator>> (signed char &val)
        {
            int16_t v;
            operator>>(v);
            val = v;
            return *this;
        }

        inline DataReader& operator>> (unsigned char &val)
        {
            uint16_t v;
            operator>>(v);
            val = v;
            return *this;
        }

    private:
        //----------------------------------------------------------------------
        /// @brief  Returns the total size of the file
        //----------------------------------------------------------------------
        int64_t totalSize() const;


        //_____ Internal types __________
    private:
        struct tStream
        {
            std::string     strFileName;
            std::ifstream   stream;
            unsigned int    refCounter;
        };


        //_____ Attributes __________
    private:
        tStream*    _pStream;
        int64_t     _offset;
        int64_t     _current;
    };
}

#endif
