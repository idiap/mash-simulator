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


/** @file   data_buffer.cpp
    @author Philip Abbet

    Implementation of the 'DataBuffer' class
*/

#include "declarations.h"
#include "data_buffer.h"
#include "stringutils.h"
#include <memory.h>
#include <assert.h>


using namespace Mash;
using namespace std;


const unsigned int BUFFER_INCREMENT = 1024;


/****************************** CONSTRUCTION / DESTRUCTION ******************************/

DataBuffer::DataBuffer()
: _data(0), _size(0), _allocated(BUFFER_INCREMENT)
{
    _data = new unsigned char[_allocated];
}

//-----------------------------------------------------------------------

DataBuffer::DataBuffer(const std::string& str)
: _data(0), _size(str.size()), _allocated(((str.size() / BUFFER_INCREMENT) + 1) * BUFFER_INCREMENT)
{
    _data = new unsigned char[_allocated];
    memcpy(_data, str.c_str(), _size);
}

//-----------------------------------------------------------------------

DataBuffer::DataBuffer(const unsigned char* pData, unsigned int dataSize)
: _data(0), _size(dataSize), _allocated(((dataSize / BUFFER_INCREMENT) + 1) * BUFFER_INCREMENT)
{
    assert(pData);
    assert(dataSize > 0);

    _data = new unsigned char[_allocated];
    memcpy(_data, pData, _size);
}

//-----------------------------------------------------------------------

DataBuffer::~DataBuffer()
{
    delete[] _data;
}


/*********************************** METHODS **********************************/

void DataBuffer::add(const std::string& str)
{
    if (_size + str.size() > _allocated)
        reallocate(str.size());

    memcpy(_data + _size, str.c_str(), str.size());

    _size += str.size();
}


void DataBuffer::add(const unsigned char* pData, unsigned int dataSize)
{
    assert(pData);
    assert(dataSize > 0);

    if (_size + dataSize > _allocated)
        reallocate(dataSize);

    memcpy(_data + _size, pData, dataSize);

    _size += dataSize;
}


void DataBuffer::extract(unsigned char* pDest, unsigned int nbBytes)
{
    assert(pDest);
    assert(nbBytes <= _size);

    memcpy(pDest, _data, nbBytes);

    unsigned char* previous = _data;

    _size -= nbBytes;

    _data = new unsigned char[_allocated];
    memcpy(_data, previous + nbBytes, _size);

    delete[] previous;
}


bool DataBuffer::extractLine(std::string &strLine)
{
    for (unsigned int i = 0; i < _size; ++i)
    {
        if (_data[i] == '\n')
        {
            _data[i] = 0;
            strLine = (char*) _data;

            unsigned char* previous = _data;

            _size -= i + 1;

            _data = new unsigned char[_allocated];
            memcpy(_data, previous + i + 1, _size);

            delete[] previous;

            return true;
        }
    }

    return false;
}


void DataBuffer::reset()
{
    _size = 0;
}


void DataBuffer::reallocate(unsigned int nbBytesToAdd)
{
    if (nbBytesToAdd <= _allocated - _size)
        return;

    _allocated = (((_size + nbBytesToAdd) / BUFFER_INCREMENT) + 1) * BUFFER_INCREMENT;

    unsigned char* previous = _data;

    _data = new unsigned char[_allocated];
    memcpy(_data, previous, _size);

    delete[] previous;
}


bool DataBuffer::extractMessage(std::string* strMessage, ArgumentsList* arguments)
{
    // Assertions
    assert(strMessage);
    assert(arguments);

    // Declarations
    size_t offset;
    tStringList parts;
    tStringIterator iter, iterEnd;
    string strLine;

    while (extractLine(strLine))
    {
        if (strLine.empty())
            continue;

        parts = StringUtils::split(strLine, " ");

        bool bQuotedString = false;
        string quotedString = "";
        for (iter = ++(parts.begin()), iterEnd = parts.end(); iter != iterEnd; ++iter)
        {
            if (!bQuotedString)
            {
                if (iter->at(0) == '\'')
                {
                    quotedString = iter->substr(1);
                    bQuotedString = true;
                }
                else
                {
                    arguments->add(decodeArgument(*iter));
                }
            }
            else
            {
                if ((iter->at(iter->size() - 1) == '\'') and ((iter->size() == 1) || (iter->at(iter->size() - 2) != '\\')))
                {
                    quotedString += " " + iter->substr(0, iter->size() - 1);
                    arguments->add(decodeArgument(quotedString));
                    bQuotedString = false;
                    quotedString = "";
                }
                else
                {
                    quotedString += " " + *iter;
                }
            }
        }

        (*strMessage) = parts[0];

        return true;
    }

    return false;
}


std::string DataBuffer::encodeArgument(const std::string& strArgument)
{
    string strResult = StringUtils::replaceAll(strArgument, "'", "\\'");
    strResult = StringUtils::replaceAll(strResult, "\n", "\\n");

    return strResult;
}


std::string DataBuffer::decodeArgument(const std::string& strArgument)
{
    string strResult = StringUtils::replaceAll(strArgument, "\\'", "'");
    strResult = StringUtils::replaceAll(strResult, "\\n", "\n");

    return strResult;
}
