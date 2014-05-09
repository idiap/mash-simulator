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


/** @file   random_number_generator.cpp
    @author Philip Abbet

    Implementation of the 'RandomNumberGenerator' class
*/

#include "random_number_generator.h"
#include <math.h>
#include <time.h>


using namespace Mash;


#if MASH_PLATFORM == MASH_PLATFORM_WIN32
    typedef unsigned __int64    uint64;
    typedef __int64             int64;
#else
    typedef unsigned long long  uint64;
    typedef long long           int64;
#endif


/************************************** CONSTANTS ***************************************/

const unsigned int A = 1664525;
const unsigned int B = 1013904223;
const uint64 N = 0x100000000LL;


/****************************** CONSTRUCTION / DESTRUCTION ******************************/

RandomNumberGenerator::RandomNumberGenerator()
: m_uiSeed(0), m_uiCurrent(0)
{
    m_uiSeed = time(0);
    m_uiCurrent = m_uiSeed;
}

//-----------------------------------------------------------------------

RandomNumberGenerator::~RandomNumberGenerator()
{
}


/*************************************** METHODS ****************************************/

void RandomNumberGenerator::setSeed(unsigned int uiSeed)
{
    m_uiSeed = uiSeed;
    m_uiCurrent = m_uiSeed;
}

//-----------------------------------------------------------------------

void RandomNumberGenerator::reset()
{
    m_uiCurrent = m_uiSeed;
}

//-----------------------------------------------------------------------

unsigned int RandomNumberGenerator::randomize(unsigned int max)
{
    if (max == 0)
        return 0;

    return randomize() % ((uint64) max) + 1;
}

//-----------------------------------------------------------------------

unsigned int RandomNumberGenerator::randomize(unsigned int min, unsigned int max)
{
    if (max == min)
        return min;

    return randomize() % (((uint64) max) + 1 - min) + min;
}

//-----------------------------------------------------------------------

int RandomNumberGenerator::randomize(int max)
{
    if (max == 0)
        return 0;

    return randomize() % ((int64) max) + 1;
}

//-----------------------------------------------------------------------

int RandomNumberGenerator::randomize(int min, int max)
{
    if (max == min)
        return min;

    return randomize() % (((int64) max) + 1 - min) + min;
}

//-----------------------------------------------------------------------

float RandomNumberGenerator::randomize(float max)
{
    if ((max >= -1e-6f) && (max <= 1e-6f))
        return 0.0f;

    return ((float) randomize() / 0xFFFFFFFF) * max;
}

//-----------------------------------------------------------------------

float RandomNumberGenerator::randomize(float min, float max)
{
    if ((max >= min - 1e-6f) && (max <= min + 1e-6f))
        return min;

    return ((float) randomize() / 0xFFFFFFFF) * (max - min) + min;
}

//-----------------------------------------------------------------------

unsigned int RandomNumberGenerator::randomize()
{
    m_uiCurrent = (A * m_uiCurrent + B) % N;
    return m_uiCurrent;
}
