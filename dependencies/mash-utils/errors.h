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


/** @file   utils.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Utility functions
*/

#ifndef _MASH_UTILS_H_
#define _MASH_UTILS_H_

#include "declarations.h"
#include <string>


namespace Mash
{
    extern MASH_SYMBOL std::string getErrorDescription(tError error);
    extern MASH_SYMBOL std::string getErrorDetailedDescription(tError error);
}

#endif
