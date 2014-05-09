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


#ifndef _DECLARATIONS_H_
#define _DECLARATIONS_H_

enum tAction
{
    ACTION_GO_FORWARD,
    ACTION_GO_BACKWARD,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,

    ACTIONS_COUNT
};


enum tKeys
{
    VKEY_EXIT    = 1,
    VKEY_UP,
    VKEY_DOWN,
    VKEY_LEFT,
    VKEY_RIGHT,
    VKEY_RESET_TASK,
};


enum tResult
{
    RESULT_NONE,
    RESULT_SUCCESS,
    RESULT_FAILED,
};


extern unsigned int VIEW_WIDTH;
extern unsigned int VIEW_HEIGHT;
extern unsigned int RTT_WIDTH;
extern unsigned int RTT_HEIGHT;


void setResolution(unsigned int width, unsigned int height);

#endif
