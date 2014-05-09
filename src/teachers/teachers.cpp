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


#include <teachers/teachers.h>
#include <teachers/TeacherReachOneFlagSingleRoom.h>
#include <teachers/TeacherReachOneFlagTwoRooms.h>
#include <teachers/TeacherReachCorrectTargetSingleRoom.h>
#include <teachers/TeacherFollowTheLightLightRoom.h>
#include <teachers/TeacherFollowTheArrowTShapedCorridor.h>
#include <teachers/TeacherFollowTheLineLineRoom.h>
#include <teachers/TeacherFollowTheBlobsInBlobsRoom.h>
#include <teachers/TeacherEatAllTargets.h>


Teacher* createTeacher(const std::string& strGoal, const std::string& strEnvironment,
                       Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
{
    if (strGoal == "reach_1_flag")
    {
        if (strEnvironment == "SingleRoom")
            return new TeacherReachOneFlagSingleRoom(pMap, pCamera);
        else if ((strEnvironment == "TwoRooms") || (strEnvironment == "L-ShapedCorridor"))
            return new TeacherReachOneFlagTwoRooms(pMap, pCamera);
    }
    else if (strGoal == "follow_the_light")
    {
        if (strEnvironment == "LightRoom")
            return new TeacherFollowTheLightLightRoom(pMap, pCamera);
    }
    else if (strGoal == "follow_the_arrow")
    {
        if (strEnvironment == "T-ShapedCorridor")
            return new TeacherFollowTheArrowTShapedCorridor(pMap, pCamera);
    }
    else if (strGoal == "follow_the_line")
    {
        if (strEnvironment == "Line")
            return new TeacherFollowTheLineLineRoom(pMap, pCamera);
    }
    else if (strGoal == "follow_the_blobs")
    {
        if (strEnvironment == "BlobsRoom")
            return new TeacherFollowTheBlobsInBlobsRoom(pMap, pCamera);
    }
    else if (strGoal == "eat_black_disks")
    {
        if (strEnvironment == "HugeRoom")
            return new TeacherEatAllTargets(pMap, pCamera);
    }
    else if (strGoal == "reach_correct_pillar")
    {
        if ((strEnvironment == "SingleRoom") || (strEnvironment == "MediumRoom"))
            return new TeacherReachCorrectTargetSingleRoom(pMap, pCamera);
    }
    else if (strGoal == "reach_correct_object")
    {
        if ((strEnvironment == "SingleRoom") || (strEnvironment == "MediumRoom"))
            return new TeacherReachCorrectTargetSingleRoom(pMap, pCamera);
    }

    return 0;
}
