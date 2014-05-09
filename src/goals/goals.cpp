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


#include <goals/goals.h>
#include <goals/GoalReachOneFlag.h>
#include <goals/GoalReachTwoFlagsInOrder.h>
#include <goals/GoalReachUniqueFlag.h>
#include <goals/GoalAllYouCanEat.h>
#include <goals/GoalFollowTheLight.h>
#include <goals/GoalFollowTheArrow.h>
#include <goals/GoalFollowTheLine.h>
#include <goals/GoalFollowTheBlobs.h>
#include <goals/GoalEatBlackDisks.h>
#include <goals/GoalReachCorrectPillar.h>
#include <goals/GoalReachCorrectObject.h>
#include <goals/GoalSecret.h>


Goal* createGoal(const std::string& strName)
{
    if (strName == "reach_1_flag")
        return new GoalReachOneFlag();
    else if (strName == "reach_2_flags_in_order")
        return new GoalReachTwoFlagsInOrder();
    else if (strName == "reach_unique_flag")
        return new GoalReachUniqueFlag();
    else if (strName == "all_you_can_eat")
        return new GoalAllYouCanEat();
    else if (strName == "follow_the_light")
        return new GoalFollowTheLight();
    else if (strName == "follow_the_arrow")
        return new GoalFollowTheArrow();
    else if (strName == "follow_the_line")
        return new GoalFollowTheLine();
    else if (strName == "follow_the_blobs")
        return new GoalFollowTheBlobs();
    else if (strName == "eat_black_disks")
        return new GoalEatBlackDisks();
    else if (strName == "reach_correct_pillar")
        return new GoalReachCorrectPillar();
    else if (strName == "reach_correct_object")
        return new GoalReachCorrectObject();
    else if (strName == "secret")
        return new GoalSecret();

    return 0;
}
