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


#ifndef _GOAL_FOLLOW_THE_LIGHT_H_
#define _GOAL_FOLLOW_THE_LIGHT_H_

#include <goals/Goal.h>


class GoalFollowTheLight: public Goal
{
    //_____ Construction / Destruction __________
public:
    GoalFollowTheLight();
    virtual ~GoalFollowTheLight();


    //_____ Methods to implement __________
public:
    virtual void setup(MapBuilder* pMapBuilder);
    virtual void update();


    //_____ Internal types __________
protected:
    enum tPhase
    {
        PHASE_FORWARD,
        PHASE_LEFT,
        PHASE_RIGHT,

        PHASES_COUNT
    };


    //_____ Attributes __________
protected:
    tPhase          m_phase;
    unsigned int    m_counter;
};

#endif
