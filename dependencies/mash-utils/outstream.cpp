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


/** @file   outstream.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'OutStream' class
*/

#include "outstream.h"
#include <iostream>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <memory.h>


using namespace std;
using namespace Mash;


/************************************ MACROS **********************************/

#define IMPLEMENT_OPERATOR(OPERATOR)                        \
OPERATOR                                                    \
{                                                           \
    if (_pStream && _pStream->bCanWrite)                    \
    {                                                       \
        _pStream->stream << val;                            \
        _pStream->stream.flush();                           \
    }                                                       \
                                                            \
    if (_verbosityLevel <= verbosityLevel)                  \
    {                                                       \
        if (_pStream && _pStream->newline)                  \
            cout << "{" << _pStream->strName.c_str() << "} " << val; \
        else                                                \
            cout << val;                                    \
                                                            \
        if (_pStream)                                       \
            _pStream->newline = false;                      \
    }                                                       \
                                                            \
    return *this;                                           \
}


#define IMPLEMENT_OPERATOR_FOR_TYPE(TYPE)   IMPLEMENT_OPERATOR(OutStream& OutStream::operator<< (TYPE val))


#define IMPLEMENT_OPERATOR_FOR_MANIPULATOR(MANIPULATOR)     \
OutStream& OutStream::operator<< (const Mash::MANIPULATOR& manip) \
{                                                           \
    if (_pStream && _pStream->bCanWrite)                    \
        _pStream->stream << std::MANIPULATOR(manip.val);    \
                                                            \
    if (_verbosityLevel <= verbosityLevel)                  \
        cout << std::MANIPULATOR(manip.val);                \
                                                            \
    return *this;                                           \
}


/****************************** STATIC ATTRIBUTES *****************************/

unsigned char OutStream::verbosityLevel = 0;



/************************* CONSTRUCTION / DESTRUCTION *************************/

OutStream::OutStream(unsigned char verbosityLevel)
: _pStream(0), _verbosityLevel(verbosityLevel)
{
}


OutStream::OutStream(const OutStream& ref)
: _pStream(ref._pStream), _verbosityLevel(ref._verbosityLevel)
{
    if (_pStream)
        ++_pStream->refCounter;
}


OutStream::~OutStream()
{
    if (_pStream)
        close();
}


/*********************************** METHODS **********************************/

bool OutStream::open(const std::string& strName, const std::string& strFileName,
                     int64_t maxSize, bool bCanReopen)
{
    // Assertions
    assert(!strName.empty());
    assert(!strFileName.empty());

    if (_pStream)
        close();

    if ((maxSize > 0) && (maxSize < 100))
        maxSize = 100;

    // Initialisations
    _pStream = new tStream();
    _pStream->strName       = strName;
    _pStream->strFileName   = strFileName;
    _pStream->newline       = true;
    _pStream->refCounter    = 1;
    _pStream->bCanReopen    = bCanReopen;
    _pStream->bCanWrite     = true;
    _pStream->maxSize       = maxSize;
    _pStream->currentSize   = 0;

    size_t offset = _pStream->strFileName.find("$TIMESTAMP");
    if (offset != string::npos)
    {
        time_t t;
        struct tm* timeinfo;
        char buffer[16];

        time(&t);
        timeinfo = localtime(&t);

        strftime(buffer, 16, "%Y%m%d-%H%M%S", timeinfo);

        _pStream->strFileName = _pStream->strFileName.substr(0, offset) + buffer +
                                _pStream->strFileName.substr(offset + 10);
    }

    // Create the folders if necessary
    offset = _pStream->strFileName.find_last_of("/");
    if (offset != string::npos)
    {
        string dest = _pStream->strFileName.substr(0, offset + 1);

        size_t start = dest.find("/", 0);
        while (start != string::npos)
        {
            string dirname = dest.substr(0, start);

            DIR* d = opendir(dirname.c_str());
            if (!d)
                mkdir(dirname.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            else
                closedir(d);

            start = dest.find("/", start + 1);
        }
    }

    // Open the file
    _pStream->stream.open(_pStream->strFileName.c_str());

    if (_pStream->stream.is_open())
    {
        _pStream->stream << "********************************************************************************" << endl
                         << "*" << endl
                         << "*                   " << strName.c_str() << endl
                         << "*" << endl
                         << "********************************************************************************" << endl
                         << endl;
        _pStream->stream.flush();

        _pStream->currentSize = _pStream->stream.tellp();
    }

    return _pStream->stream.is_open();
}


void OutStream::close()
{
    if (_pStream)
    {
        --_pStream->refCounter;

        if (_pStream->refCounter == 0)
        {
            _pStream->stream << "---------------- End of the log file ----------------" << endl;
            _pStream->stream.close();
            delete _pStream;
        }

        _pStream = 0;
    }
}


void OutStream::deleteFile()
{
    if (!_pStream)
        return;

    if (_pStream->refCounter > 1)
    {
        close();
        return;
    }

    string strFileName = _pStream->strFileName;

    close();

    if (!strFileName.empty())
        remove(strFileName.c_str());
}


int64_t OutStream::dump(unsigned char** pBuffer, int64_t max_size)
{
    if (!pBuffer || !_pStream || _pStream->strFileName.empty() || !_pStream->bCanReopen)
        return 0;

    ifstream inFile;
    inFile.open(_pStream->strFileName.c_str());
    if (!inFile.is_open())
        return 0;

    inFile.seekg(0, ios_base::end);
    int size = inFile.tellg();

    if (size <= 0)
        return 0;

    bool bTruncated = false;

    if ((max_size > 0) && (max_size < 100))
        max_size = 100;

    if ((max_size > 0) && (size > max_size))
    {
        inFile.seekg(size - (max_size - 4), ios_base::beg);
        size = max_size;
        bTruncated = true;
    }
    else
    {
        inFile.seekg(0, ios_base::beg);
    }

    (*pBuffer) = new unsigned char[size+1];
    memset(*pBuffer, 0, (size + 1) * sizeof(unsigned char));

    unsigned char* pDest = (*pBuffer);

    if (bTruncated)
    {
        memcpy(pDest, "...\n", 4);
        pDest += 4;
    }

    while (size - (pDest - (*pBuffer)) > 0)
    {
        inFile.read((char*) pDest, size - (pDest - (*pBuffer)));
        unsigned int nb = inFile.gcount();
        pDest += nb;
    }

    inFile.close();

    return size;
}


void OutStream::operator=(const OutStream& ref)
{
    if (_pStream)
        close();

    if (ref._pStream)
    {
        _pStream = ref._pStream;
        ++_pStream->refCounter;
    }

    if (ref._verbosityLevel < _verbosityLevel)
        _verbosityLevel = ref._verbosityLevel;
}


void OutStream::write(const char* pData, int64_t size)
{
    // Assertions
    assert(pData);
    assert(size > 0);

    if (_pStream && _pStream->bCanWrite)
    {
        int64_t offset = 0;

        if ((_pStream->maxSize > 0) && (_pStream->currentSize + size >= _pStream->maxSize * 2))
        {
            if (_pStream->bCanReopen)
            {
                if (size >= _pStream->maxSize)
                {
                    _pStream->stream.close();
                    remove(_pStream->strFileName.c_str());

                    _pStream->stream.open(_pStream->strFileName.c_str());

                    offset = size - _pStream->maxSize;

                    _pStream->currentSize = 0;
                }
                else
                {
                    unsigned char* pBuffer = 0;
                    int nb = dump(&pBuffer, _pStream->maxSize - size);

                    _pStream->stream.close();
                    remove(_pStream->strFileName.c_str());

                    _pStream->stream.open(_pStream->strFileName.c_str());
                    _pStream->stream.write((const char*) pBuffer, nb);

                    _pStream->currentSize = nb;

                    delete[] pBuffer;
                }
            }
            else
            {
                _pStream->bCanWrite = false;

                _pStream->stream << "-------- Log file size limit reached -----------" << endl;
                _pStream->stream.flush();
            }
        }

        if (_pStream->bCanWrite)
        {
            _pStream->stream.write(pData + offset, size - offset);
            _pStream->stream.flush();

            _pStream->currentSize += size - offset;
        }
    }

    if (_verbosityLevel <= verbosityLevel)
    {
        if (_pStream && _pStream->newline)
            cout << "{" << _pStream->strName.c_str() << "} ";

        cout.write(pData, size);

        if (_pStream)
            _pStream->newline = false;
    }
}


IMPLEMENT_OPERATOR_FOR_TYPE(std::streambuf*);


OutStream& OutStream::operator<< (std::ostream& (*val)(std::ostream&))
{
    if (_pStream && _pStream->bCanWrite)
    {
        _pStream->stream << val;
        _pStream->stream.flush();
    }

    if (_verbosityLevel <= verbosityLevel)
    {
        if (_pStream && _pStream->newline)
            cout << "{" << _pStream->strName.c_str() << "} " << val;
        else
            cout << val;

        if (_pStream)
            _pStream->newline = (val == static_cast<std::ostream& (*)(std::ostream&)>(std::endl));
    }

    return *this;
}


IMPLEMENT_OPERATOR(OutStream& OutStream::operator<< (std::ios& (*val)(std::ios&)));
IMPLEMENT_OPERATOR(OutStream& OutStream::operator<< (std::ios_base& (*val)(std::ios_base&)));


OutStream& OutStream::operator<< (const std::string& s)
{
    if (_pStream && _pStream->bCanWrite)
    {
        int offset = 0;

        if ((_pStream->maxSize > 0) && (_pStream->currentSize + s.size() >= _pStream->maxSize * 2))
        {
            if (_pStream->bCanReopen)
            {
                if (s.size() >= _pStream->maxSize)
                {
                    _pStream->stream.close();
                    remove(_pStream->strFileName.c_str());

                    _pStream->stream.open(_pStream->strFileName.c_str());

                    offset = s.size() - _pStream->maxSize;

                    _pStream->currentSize = 0;
                }
                else
                {
                    unsigned char* pBuffer = 0;
                    int64_t nb = dump(&pBuffer, _pStream->maxSize - s.size());

                    _pStream->stream.close();
                    remove(_pStream->strFileName.c_str());

                    _pStream->stream.open(_pStream->strFileName.c_str());
                    _pStream->stream.write((const char*) pBuffer, nb);

                    _pStream->currentSize = nb;

                    delete[] pBuffer;
                }
            }
            else
            {
                _pStream->bCanWrite = false;

                _pStream->stream << "-------- Log file size limit reached -----------" << endl;
                _pStream->stream.flush();
            }
        }

        if (_pStream->bCanWrite)
        {
            _pStream->stream << s.substr(offset).c_str();
            _pStream->stream.flush();

            _pStream->currentSize += s.size() - offset;
        }
    }

    if (_verbosityLevel <= verbosityLevel)
    {
        if (_pStream && _pStream->newline)
            cout << "{" << _pStream->strName.c_str() << "} " << s.c_str();
        else
            cout << s.c_str();

        if (_pStream)
            _pStream->newline = false;
    }

    return *this;
}


IMPLEMENT_OPERATOR_FOR_MANIPULATOR(resetiosflags);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setiosflags);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setbase);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setfill);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setprecision);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setw);
