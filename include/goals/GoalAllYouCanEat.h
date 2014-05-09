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


#ifndef _GOAL_ALL_YOU_CAN_EAT_H_
#define _GOAL_ALL_YOU_CAN_EAT_H_

#include <goals/Goal.h>


class GoalAllYouCanEat: public Goal
{
    //_____ Construction / Destruction __________
public:
    GoalAllYouCanEat();
    virtual ~GoalAllYouCanEat();


    //_____ Methods to implement __________
public:
    virtual void setup(MapBuilder* pMapBuilder);

    virtual void reset()
    {
        m_nbFlagsCollected = 0;
    }

protected:
    virtual tResult onTargetReached(tTarget* pTarget, float &reward);


    //_____ Attributes __________
protected:
    unsigned int m_nbFlagsCollected;
};

#endif
