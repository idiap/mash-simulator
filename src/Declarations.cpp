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


#include <Declarations.h>
#include <Athena-Math/MathUtils.h>

using namespace Athena;
using namespace Athena::Math;


unsigned int VIEW_WIDTH  = 320;
unsigned int VIEW_HEIGHT = 240;
unsigned int RTT_WIDTH   = 512;
unsigned int RTT_HEIGHT  = 512;


void setResolution(unsigned int width, unsigned int height)
{
    VIEW_WIDTH  = width;
    VIEW_HEIGHT = height;
    RTT_WIDTH   = MathUtils::Pow(2, MathUtils::Ceil(MathUtils::Log2(width)));
    RTT_HEIGHT  = MathUtils::Pow(2, MathUtils::Ceil(MathUtils::Log2(height)));
}
