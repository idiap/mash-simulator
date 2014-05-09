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


/** @file   stringutils.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the class 'StringUtils'
*/

#include "stringutils.h"
#include <sstream>
#include <ctype.h>
#include <stdlib.h>


using namespace Mash;
using namespace std;


/********************************** METHODS ***********************************/

std::string StringUtils::toString(int val)
{
    std::ostringstream str;

    str << val;
    return str.str();
}

//-----------------------------------------------------------------------

std::string StringUtils::toString(unsigned int val)
{
    std::ostringstream str;

    str << val;
    return str.str();
}

//-----------------------------------------------------------------------

std::string StringUtils::toString(float val)
{
    std::ostringstream str;

    str << val;
    return str.str();
}

//-----------------------------------------------------------------------

std::string StringUtils::toString(double val)
{
    std::ostringstream str;

    str << val;
    return str.str();
}

//-----------------------------------------------------------------------

std::string StringUtils::toString(const timeval& val)
{
    std::ostringstream str1, str2;

    str1 << val.tv_sec << ".";
    str2 << val.tv_usec;

    if (str2.str().size() < 6)
        str1 << string("000000").substr(0, 6 - str2.str().size());

    return str1.str() + str2.str();
}

//-----------------------------------------------------------------------

int StringUtils::parseInt(const std::string& val)
{
    std::istringstream str(val);
    int ret;

    str >> ret;
    return ret;
}

//-----------------------------------------------------------------------

unsigned int StringUtils::parseUnsignedInt(const std::string& val)
{
    return static_cast<unsigned int>(strtoul(val.c_str(), 0, 10));
}

//-----------------------------------------------------------------------

float StringUtils::parseFloat(const std::string& val)
{
    std::istringstream str(val);
    float ret;

    str >> ret;
    return ret;
}

//-----------------------------------------------------------------------

double StringUtils::parseDouble(const std::string& val)
{
    std::istringstream str(val);
    double ret;

    str >> ret;
    return ret;
}

//-----------------------------------------------------------------------

struct timeval StringUtils::parseTimeval(const std::string& val)
{
    struct timeval ret = { 0, 0 };

    if (val[0] != '.')
    {
        tStringList parts = split(val, ".");

        std::istringstream str(parts[0]);
        str >> ret.tv_sec;

        if ((parts.size() == 2) && !parts[1].empty())
        {
            std::istringstream str(parts[1]);
            str >> ret.tv_usec;
        }
    }
    else
    {
        std::istringstream str(val.substr(1, val.size() - 1));
        str >> ret.tv_usec;
    }

    return ret;
}

//-----------------------------------------------------------------------

tStringList StringUtils::split(const std::string& str,
                               const std::string& delims,
                               unsigned int maxSplits)
{
    // Delcarations
    tStringList ret;

    // Pre-allocate some space for performance
    ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

    unsigned int numSplits = 0;

    // Use STL methods
    size_t start, pos;
    start = 0;
    do
    {
        pos = str.find_first_of(delims, start);
        if (pos == start)
        {
            // Do nothing
            start = pos + 1;
        }
        else if (pos == string::npos || (maxSplits && numSplits == maxSplits))
        {
            // Copy the rest of the string
            ret.push_back( str.substr(start) );
            break;
        }
        else
        {
            // Copy up to delimiter
            ret.push_back(str.substr(start, pos - start));
            start = pos + 1;
        }
        // parse up to next real data
        start = str.find_first_not_of(delims, start);
        ++numSplits;

    } while (pos != string::npos);

    return ret;
}

//-----------------------------------------------------------------------

bool StringUtils::startsWith(const std::string& str, const std::string& pattern,
                             bool bLowerCase)
{
    size_t len = str.length();
    size_t patternLen = pattern.length();

    if ((len < patternLen) || (patternLen == 0))
        return false;

    string start = str.substr(0, patternLen);
    if (bLowerCase)
        StringUtils::toLowerCase(start);

    return (start == pattern);
}

//-----------------------------------------------------------------------

bool StringUtils::endsWith(const std::string& str, const std::string& pattern,
                           bool bLowerCase)
{
    size_t len = str.length();
    size_t patternLen = pattern.length();

    if ((len < patternLen) || (patternLen == 0))
        return false;

    string end = str.substr(len - patternLen, patternLen);
    if (bLowerCase)
        StringUtils::toLowerCase(end);

    return (end == pattern);
}

//-----------------------------------------------------------------------

void StringUtils::toLowerCase(std::string &str)
{
    string::iterator iter, iterEnd;
    for (iter = str.begin(), iterEnd = str.end(); iter != iterEnd; ++iter)
        *iter = tolower(*iter);
}

//-----------------------------------------------------------------------

std::string StringUtils::replaceAll(const std::string& value,
                                    const std::string& search,
                                    const std::string& replacement)
{
    string result = value;
    size_t offset = result.find(search);
    while (offset != string::npos)
    {
        result.replace(offset, search.size(), replacement);
        offset = result.find(search, offset + replacement.size());
    }

    return result;
}

//-----------------------------------------------------------------------

std::string StringUtils::rtrim(const std::string& str, const std::string& spaces)
{
    size_t offset = str.find_last_not_of(spaces);
    if (offset == string::npos)
        return "";

    return str.substr(0, offset + 1);
}

//-----------------------------------------------------------------------

std::string StringUtils::ltrim(const std::string& str, const std::string& spaces)
{
    size_t offset = str.find_first_not_of(spaces);
    if (offset == string::npos)
        return "";

    return str.substr(offset);
}

//-----------------------------------------------------------------------

std::string StringUtils::trim(const std::string& str, const std::string& spaces)
{
    return ltrim(rtrim(str, spaces), spaces);
}
