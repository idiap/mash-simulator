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


#ifndef _TEACHER_FOLLOWTHELINE_LINEROOM_H_
#define _TEACHER_FOLLOWTHELINE_LINEROOM_H_

#include <teachers/Teacher.h>


class TeacherFollowTheLineLineRoom: public Teacher
{
    //_____ Construction / Destruction __________
public:
    TeacherFollowTheLineLineRoom(Map* pMap, Athena::Graphics::Visual::Camera* pCamera);
    virtual ~TeacherFollowTheLineLineRoom();


    //_____ Methods __________
public:
    virtual Mash::tActionsList notRecommendedActions();

protected:
    virtual tAction computeNextAction();


    //_____ Internal types __________
protected:
    enum tState
    {
        STATE_FORWARD_1,
        STATE_FORWARD_2,
        STATE_LOOK_FOR_FLAG,
        STATE_GO_TOWARD_FLAG,
    };


    //_____ Attributes __________
protected:
    tState  state;
    tPoint  target;
};

#endif
