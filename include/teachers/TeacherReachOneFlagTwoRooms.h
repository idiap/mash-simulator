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


#ifndef _TEACHER_REACHONEFLAG_TWOROOMS_H_
#define _TEACHER_REACHONEFLAG_TWOROOMS_H_

#include <teachers/Teacher.h>


class TeacherReachOneFlagTwoRooms: public Teacher
{
    //_____ Construction / Destruction __________
public:
    TeacherReachOneFlagTwoRooms(Map* pMap, Athena::Graphics::Visual::Camera* pCamera);
    virtual ~TeacherReachOneFlagTwoRooms();


    //_____ Methods __________
public:
    virtual Mash::tActionsList notRecommendedActions();

protected:
    virtual tAction computeNextAction();


    //_____ Attributes __________
protected:
    bool    flag_found;
    bool    door_reached;
    tPoint  door;
};

#endif
