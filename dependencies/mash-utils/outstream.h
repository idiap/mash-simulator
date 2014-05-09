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


/** @file   outstream.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'OutStream' class
*/

#ifndef _MASH_OUTSTREAM_H_
#define _MASH_OUTSTREAM_H_

#include "manip.h"
#include "platform.h"
#include "stringutils.h"
#include <string>
#include <fstream>
#include <stdint.h>
#include <string.h>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Represents an output stream
    ///
    /// The goal of this class is to have several output streams in the same
    /// application.
    ///
    /// Each output stream can write into its own file (and optionally to the
    /// console), or write into the same file than another output stream.
    //--------------------------------------------------------------------------
    class MASH_SYMBOL OutStream
    {
        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Constructor
        //----------------------------------------------------------------------
        OutStream(unsigned char verbosityLevel = 5);

        //----------------------------------------------------------------------
        /// @brief  Copy constructor
        //----------------------------------------------------------------------
        OutStream(const OutStream& ref);

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        ~OutStream();


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Open the stream
        ///
        /// @param  strName         Name of the stream
        /// @param  strFileName     Path to the file to write. Can contain the
        ///                         '$TIMESTAMP' format string, which will be
        ///                         replaced by the current timestamp
        /// @param  maxSize         If greater than 0: maximum size of the file
        ///                         (if necessary, the beginning of the file is
        ///                         truncated, unless bCanReopen is false)
        /// @param  bCanReopen      Indicates if the file can be closed and
        ///                         opened again at will by the stream
        /// @return                 'true' if successful
        //----------------------------------------------------------------------
        bool open(const std::string& strName, const std::string& strFileName,
                  int64_t maxSize = 0, bool bCanReopen = true);

        //----------------------------------------------------------------------
        /// @brief  Close the stream
        //----------------------------------------------------------------------
        void close();

        //----------------------------------------------------------------------
        /// @brief  Delete the log file of the stream
        //----------------------------------------------------------------------
        void deleteFile();

        //----------------------------------------------------------------------
        /// @brief  Dump the content of the log file of the stream
        ///
        /// @param[out] pBuffer     Pointer to the memory buffer (allocated by
        ///                         dump())
        /// @param[in]  max_size    If greater than 0: maximum size of the
        ///                         returned buffer (if necessary, the beginning
        ///                         of the file is ignored)
        /// @return                 Size of the buffer, 0 if failed
        //----------------------------------------------------------------------
        int64_t dump(unsigned char** pBuffer, int64_t max_size = 0);

        void operator=(const OutStream& ref);

        void write(const char* pData, int64_t size);

        template <typename TYPE>
        OutStream& operator<< (TYPE val)
        {
            if (_pStream && _pStream->bCanWrite)
            {
                bool bCanWrite = true;

                if ((_pStream->maxSize > 0) && (_pStream->currentSize >= _pStream->maxSize * 2))
                {
                    if (_pStream->bCanReopen)
                    {
                        unsigned char* pBuffer = 0;
                        int64_t nb = dump(&pBuffer, _pStream->maxSize);

                        _pStream->stream.close();
                        remove(_pStream->strFileName.c_str());

                        _pStream->stream.open(_pStream->strFileName.c_str());
                        _pStream->stream.write((const char*) pBuffer, nb);

                        _pStream->currentSize = nb;

                        delete[] pBuffer;
                    }
                    else
                    {
                        _pStream->bCanWrite = false;

                        _pStream->stream << "-------- Log file size limit reached -----------" << std::endl;
                        _pStream->stream.flush();
                    }
                }

                if (_pStream->bCanWrite)
                {
                    _pStream->stream << val;
                    _pStream->stream.flush();

                    _pStream->currentSize = _pStream->stream.tellp();
                }
            }

            if (_verbosityLevel <= verbosityLevel)
            {
                if (_pStream && _pStream->newline)
                    std::cout << "{" << _pStream->strName.c_str() << "} " << val;
                else
                    std::cout << val;

                if (_pStream)
                    _pStream->newline = false;
            }

            return *this;
        }


        inline OutStream& operator<< (char val)
        {
            return operator<<((int16_t) val);
        }

        inline OutStream& operator<< (signed char val)
        {
            return operator<<((int16_t) val);
        }

        inline OutStream& operator<< (unsigned char val)
        {
            return operator<<((uint16_t) val);
        }

        inline OutStream& operator<< (const struct timeval& val)
        {
            return operator<<(StringUtils::toString(val));
        }


        inline OutStream& operator<< (const char* s)
        {
            write(s, strlen(s));
            return *this;
        }

        inline OutStream& operator<< (const signed char* s)
        {
            write((const char*) s, strlen((const char*) s));
            return *this;
        }

        inline OutStream& operator<< (const unsigned char* s)
        {
            write((const char*) s, strlen((const char*) s));
            return *this;
        }


        OutStream& operator<< (const std::string& s);

        OutStream& operator<< (std::streambuf* sb);

        OutStream& operator<< (std::ostream& (*pf)(std::ostream&));
        OutStream& operator<< (std::ios& (*pf)(std::ios&));
        OutStream& operator<< (std::ios_base& (*pf)(std::ios_base&));

        OutStream& operator<< (const Mash::resetiosflags& manip);
        OutStream& operator<< (const Mash::setiosflags& manip);
        OutStream& operator<< (const Mash::setbase& manip);
        OutStream& operator<< (const Mash::setfill& manip);
        OutStream& operator<< (const Mash::setprecision& manip);
        OutStream& operator<< (const Mash::setw& manip);


        inline std::string getName() const
        {
            return (_pStream ? _pStream->strName : "");
        }

        inline std::string getFileName() const
        {
            return (_pStream ? _pStream->strFileName : "");
        }

        inline void setVerbosityLevel(unsigned char level)
        {
            _verbosityLevel = level;
        }


        //_____ Internal types __________
    private:
        struct tStream
        {
            std::string     strName;
            std::string     strFileName;
            std::ofstream   stream;
            bool            newline;
            unsigned int    refCounter;
            bool            bCanReopen;
            bool            bCanWrite;
            int64_t         maxSize;
            int64_t         currentSize;
        };


        //_____ Static attributes __________
    public:
        static unsigned char verbosityLevel;


        //_____ Attributes __________
    private:
        tStream*        _pStream;
        unsigned char   _verbosityLevel;    ///< Each stream has its own verbosity level
    };
}

#endif
