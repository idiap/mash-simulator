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


/** @file   data_writer.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'DataWriter' class
*/

#include "data_writer.h"
#include <iostream>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>


using namespace std;
using namespace Mash;


/************************************ MACROS **********************************/

#define IMPLEMENT_OPERATOR(OPERATOR)                                                            \
OPERATOR                                                                                        \
{                                                                                               \
    if (_pStream)                                                                               \
    {                                                                                           \
        if ((_pStream->maximumSize < 0) || (_pStream->maximumSize > _pStream->stream.tellp()))  \
        {                                                                                       \
            _pStream->stream << val;                                                            \
            _pStream->stream.flush();                                                           \
        }                                                                                       \
    }                                                                                           \
                                                                                                \
    return *this;                                                                               \
}


#define IMPLEMENT_OPERATOR_FOR_TYPE(TYPE)   IMPLEMENT_OPERATOR(DataWriter& DataWriter::operator<< (TYPE val))

#define IMPLEMENT_OPERATOR_FOR_MANIPULATOR(MANIPULATOR)                                         \
DataWriter& DataWriter::operator<< (const Mash::MANIPULATOR& manip)                             \
{                                                                                               \
    if (_pStream)                                                                               \
    {                                                                                           \
        if ((_pStream->maximumSize < 0) || (_pStream->maximumSize > _pStream->stream.tellp()))  \
            _pStream->stream << std::MANIPULATOR(manip.val);                                    \
    }                                                                                           \
                                                                                                \
    return *this;                                                                               \
}


/************************* CONSTRUCTION / DESTRUCTION *************************/

DataWriter::DataWriter()
: _pStream(0)
{
}


DataWriter::DataWriter(const DataWriter& ref)
: _pStream(ref._pStream)
{
    if (_pStream)
        ++_pStream->refCounter;
}


DataWriter::~DataWriter()
{
    if (_pStream)
        close();
}


/*********************************** METHODS **********************************/

bool DataWriter::open(const std::string& strFileName, int64_t maximumSize)
{
    // Assertions
    assert(!strFileName.empty());

    if (_pStream)
        close();

    // Initialisations
    _pStream = new tStream();
    _pStream->strFileName   = strFileName;
    _pStream->maximumSize   = maximumSize;
    _pStream->refCounter    = 1;

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

    return _pStream->stream.is_open();
}


void DataWriter::close()
{
    if (_pStream)
    {
        --_pStream->refCounter;

        if (_pStream->refCounter == 0)
        {
            _pStream->stream.close();
            delete _pStream;
        }

        _pStream = 0;
    }
}


void DataWriter::deleteFile()
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


std::string DataWriter::dump()
{
    if (!_pStream || _pStream->strFileName.empty())
        return "";

    ifstream inFile;
    inFile.open(_pStream->strFileName.c_str());
    if (!inFile.is_open())
        return "";

    string strContent;
    const unsigned int BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];

    while (!inFile.eof())
    {
        inFile.read(buffer, BUFFER_SIZE - 1);
        unsigned int nb = inFile.gcount();
        buffer[nb] = '\0';

        strContent += buffer;
    }

    inFile.close();

    return strContent;
}


void DataWriter::operator=(const DataWriter& ref)
{
    if (_pStream)
        close();

    if (ref._pStream)
    {
        _pStream = ref._pStream;
        ++_pStream->refCounter;
    }
}


/**************************** METHODS TO WRITE DATA ***************************/

void DataWriter::write(const int8_t* pData, int64_t size)
{
    // Assertions
    assert(pData);
    assert(size > 0);

    int64_t currentSize = _pStream->stream.tellp();

    if (_pStream && ((_pStream->maximumSize < 0) || (_pStream->maximumSize > currentSize)))
    {
        if (_pStream->maximumSize < 0)
            _pStream->stream.write((const char*) pData, size);
        else
            _pStream->stream.write((const char*) pData, min(size, _pStream->maximumSize - currentSize));

        _pStream->stream.flush();
    }
}


IMPLEMENT_OPERATOR_FOR_TYPE(const char*);
IMPLEMENT_OPERATOR_FOR_TYPE(const signed char*);
IMPLEMENT_OPERATOR_FOR_TYPE(const unsigned char*);

IMPLEMENT_OPERATOR_FOR_TYPE(std::streambuf*);


DataWriter& DataWriter::operator<< (std::ostream& (*val)(std::ostream&))
{
    if (_pStream && ((_pStream->maximumSize < 0) || (_pStream->maximumSize > _pStream->stream.tellp())))
    {
        _pStream->stream << val;
        _pStream->stream.flush();
    }

    return *this;
}


IMPLEMENT_OPERATOR(DataWriter& DataWriter::operator<< (std::ios& (*val)(std::ios&)));
IMPLEMENT_OPERATOR(DataWriter& DataWriter::operator<< (std::ios_base& (*val)(std::ios_base&)));


DataWriter& DataWriter::operator<< (const std::string& s)
{
    int64_t currentSize = _pStream->stream.tellp();

    if (_pStream && ((_pStream->maximumSize < 0) || (_pStream->maximumSize > currentSize)))
    {
        if (_pStream->maximumSize < 0)
            _pStream->stream << s.c_str();
        else
            _pStream->stream << s.substr(0, min((int64_t) s.size(), _pStream->maximumSize - currentSize));

        _pStream->stream.flush();
    }

    return *this;
}


IMPLEMENT_OPERATOR_FOR_MANIPULATOR(resetiosflags);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setiosflags);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setbase);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setfill);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setprecision);
IMPLEMENT_OPERATOR_FOR_MANIPULATOR(setw);
