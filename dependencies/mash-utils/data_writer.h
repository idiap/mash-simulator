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


/** @file   data_writer.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'DataWriter' class
*/

#ifndef _MASH_DATAWRITER_H_
#define _MASH_DATAWRITER_H_

#include "manip.h"
#include "declarations.h"
#include "stringutils.h"
#include <stdint.h>
#include <sys/time.h>
#include <string>
#include <fstream>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Used to write data about the experiment into a file
    ///
    /// The structure of the file is undefined. It is expected that a script
    /// knowing how to interpret this data is provided with the code that used
    /// the data writer object.
    //--------------------------------------------------------------------------
    class MASH_SYMBOL DataWriter
    {
        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Constructor
        //----------------------------------------------------------------------
        DataWriter();

        //----------------------------------------------------------------------
        /// @brief  Copy constructor
        //----------------------------------------------------------------------
        DataWriter(const DataWriter& ref);

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        ~DataWriter();


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Open the file
        ///
        /// @param  strFileName     Path to the file to write. Can contain the
        ///                         '$TIMESTAMP' format string, which will be
        ///                         replaced by the current timestamp
        /// @param  maximumSize     Maximum size of the generated file (-1 for
        ///                         unlimited)
        /// @return                 'true' if successful
        //----------------------------------------------------------------------
        bool open(const std::string& strFileName, int64_t maximumSize = -1);

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
            return (_pStream ? _pStream->stream.is_open() : false);
        }

        //----------------------------------------------------------------------
        /// @brief  Dump the content of the file
        /// @return The content of the file
        //----------------------------------------------------------------------
        std::string dump();

        //----------------------------------------------------------------------
        /// @brief  Assignment operator
        //----------------------------------------------------------------------
        void operator=(const DataWriter& ref);

        //----------------------------------------------------------------------
        /// @brief  Returns the name of the file
        //----------------------------------------------------------------------
        inline std::string getFileName() const
        {
            return (_pStream ? _pStream->strFileName : "");
        }


        //_____ Methods to write data __________
    public:
        void write(const int8_t* pData, int64_t size);

        inline void write(const uint8_t* pData, int64_t size)
        {
            write((const int8_t*) pData, size);
        }


        template <typename TYPE>
        DataWriter& operator<< (TYPE val)
        {
            if (_pStream)
            {
                if ((_pStream->maximumSize < 0) || (_pStream->maximumSize > _pStream->stream.tellp()))
                {
                    _pStream->stream << val;
                    _pStream->stream.flush();
                }
            }

            return *this;
        }


        inline DataWriter& operator<< (char val)
        {
            return operator<<((int16_t) val);
        }

        inline DataWriter& operator<< (signed char val)
        {
            return operator<<((int16_t) val);
        }

        inline DataWriter& operator<< (unsigned char val)
        {
            return operator<<((uint16_t) val);
        }

        inline DataWriter& operator<< (const struct timeval& val)
        {
            return operator<<(StringUtils::toString(val));
        }

        DataWriter& operator<< (const char* s);
        DataWriter& operator<< (const signed char* s);
        DataWriter& operator<< (const unsigned char* s);

        DataWriter& operator<< (const std::string& s);

        DataWriter& operator<< (std::streambuf* sb);

        DataWriter& operator<< (std::ostream& (*pf)(std::ostream&));
        DataWriter& operator<< (std::ios& (*pf)(std::ios&));
        DataWriter& operator<< (std::ios_base& (*pf)(std::ios_base&));

        DataWriter& operator<< (const Mash::resetiosflags& manip);
        DataWriter& operator<< (const Mash::setiosflags& manip);
        DataWriter& operator<< (const Mash::setbase& manip);
        DataWriter& operator<< (const Mash::setfill& manip);
        DataWriter& operator<< (const Mash::setprecision& manip);
        DataWriter& operator<< (const Mash::setw& manip);


        //_____ Internal types __________
    private:
        struct tStream
        {
            std::string     strFileName;
            std::ofstream   stream;
            int64_t         maximumSize;
            unsigned int    refCounter;
        };


        //_____ Attributes __________
    private:
        tStream*    _pStream;
    };
}

#endif
