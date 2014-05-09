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


/** @file   manip.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the manipulators for the 'OutStream' class
*/

#ifndef _MASH_MANIP_H_
#define _MASH_MANIP_H_

#include <iostream>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Clears the specified flags
    //--------------------------------------------------------------------------
    struct resetiosflags
    {
        resetiosflags(std::ios_base::fmtflags mask)
        : val(mask)
        {
        }

        std::ios_base::fmtflags val;
    };


    //--------------------------------------------------------------------------
    /// @brief  Sets the specified flags
    //--------------------------------------------------------------------------
    struct setiosflags
    {
        setiosflags(std::ios_base::fmtflags mask)
        : val(mask)
        {
        }

        std::ios_base::fmtflags val;
    };


    //--------------------------------------------------------------------------
    /// @brief  Set base for integers
    //--------------------------------------------------------------------------
    struct setbase
    {
        setbase(int base)
        : val(base)
        {
        }

        int val;
    };


    //--------------------------------------------------------------------------
    /// @brief  Sets the character that will be used to fill spaces in a
    ///         right-justified display
    //--------------------------------------------------------------------------
    struct setfill
    {
        setfill(char c)
        : val(c)
        {
        }

        char val;
    };


    //--------------------------------------------------------------------------
    /// @brief  Sets the precision for floating-point values
    //--------------------------------------------------------------------------
    struct setprecision
    {
        setprecision(std::streamsize precision)
        : val(precision)
        {
        }

        std::streamsize val;
    };


    //--------------------------------------------------------------------------
    /// @brief  Specifies the width of the display field
    //--------------------------------------------------------------------------
    struct setw
    {
        setw(std::streamsize wide)
        : val(wide)
        {
        }

        std::streamsize val;
    };
}

#endif
