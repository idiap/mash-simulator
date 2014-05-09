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


/** @file   data_reader.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'DataReader' class
*/

#include "data_reader.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>


using namespace std;
using namespace Mash;


/************************* CONSTRUCTION / DESTRUCTION *************************/

DataReader::DataReader()
: _pStream(0), _offset(0), _current(0)
{
}


DataReader::DataReader(const DataReader& ref, int64_t offset)
: _pStream(ref._pStream), _offset(offset + ref._offset), _current(0)
{
    if (_pStream)
    {
        if (ref.size() < offset)
            _pStream = 0;
        else
            ++_pStream->refCounter;
    }
}


DataReader::DataReader(const DataReader& ref)
: _pStream(ref._pStream), _offset(ref._offset), _current(0)
{
    if (_pStream)
        ++_pStream->refCounter;
}


DataReader::~DataReader()
{
    if (_pStream)
        close();
}


/*********************************** METHODS **********************************/

bool DataReader::open(const std::string& strFileName, int64_t offset)
{
    // Assertions
    assert(!strFileName.empty());

    if (_pStream)
        close();

    // Initialisations
    _pStream = new tStream();
    _pStream->strFileName   = strFileName;
    _pStream->refCounter    = 1;
    _offset                 = offset;
    _current                = 0;

    // Open the file
    _pStream->stream.open(strFileName.c_str());

    // Offset & size management
    if (_pStream->stream.is_open())
    {
        _pStream->stream.seekg(0, ios_base::end);
        int64_t size = _pStream->stream.tellg();

        if (size < offset)
            _pStream->stream.close();
        else
            _pStream->stream.seekg(offset, ios_base::beg);
    }

    if (!_pStream->stream.is_open())
    {
        delete _pStream;
        _pStream = 0;
    }

    return (_pStream != 0);
}


void DataReader::close()
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

    _offset = 0;
    _current = 0;
}


void DataReader::deleteFile()
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


void DataReader::operator=(const DataReader& ref)
{
    if (_pStream)
        close();

    if (ref._pStream)
    {
        _pStream = ref._pStream;
        ++_pStream->refCounter;

        _offset = ref._offset;
    }
}


/**************************** METHODS TO READ DATA ****************************/

void DataReader::seek(int64_t offset, tPosition origin)
{
    if (!_pStream)
        return;

    int64_t size = totalSize();

    switch (origin)
    {
        case BEGIN:
            if (offset <= 0)
                _current = 0;
            else if (offset >= size - _offset)
                _current = size - _offset;
            else
                _current = offset;
            break;

        case CURRENT:
            if (_current + offset <= 0)
                _current = 0;
            else if (_current + offset >= size - _offset)
                _current = size - _offset;
            else
                _current += offset;
            break;

        case END:
            if (offset <= - (size - _offset))
                _current = 0;
            else if (offset >= 0)
                _current = size - _offset;
            else
                _current = size - _offset + offset;
            break;
    }
}


int64_t DataReader::tell()
{
    if (_pStream)
    {
        assert(_current <= size());
        return _current;
    }

    return 0;
}


int64_t DataReader::totalSize() const
{
    _pStream->stream.seekg(0, ios_base::end);
    return _pStream->stream.tellg();
}


int64_t DataReader::read(int8_t* pData, int64_t size)
{
    // Assertions
    assert(pData);
    assert(size > 0);

    if (_pStream)
    {
        _pStream->stream.clear();
        _pStream->stream.seekg(_current + _offset, iostream::beg);
        _pStream->stream.read((char*) pData, size);

        int64_t nb = _pStream->stream.gcount();
        _current += nb;
        return nb;
    }

    return 0;
}


int64_t DataReader::readline(char* pData, int64_t size)
{
    // Assertions
    assert(pData);
    assert(size > 0);

    if (_pStream)
    {
        _pStream->stream.clear();
        _pStream->stream.seekg(_current + _offset, iostream::beg);
        _pStream->stream.getline(pData, size);

        int64_t nb = _pStream->stream.gcount();
        _current += nb;
        return nb;
    }

    return 0;
}


uint8_t DataReader::peek()
{
    if (_pStream)
    {
        _pStream->stream.clear();
        _pStream->stream.seekg(_current + _offset, iostream::beg);
        return (uint8_t) _pStream->stream.peek();
    }

    return 0;
}
