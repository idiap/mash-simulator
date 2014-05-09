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


#include <goals/GoalReachUniqueFlag.h>


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

GoalReachUniqueFlag::GoalReachUniqueFlag()
{
}


GoalReachUniqueFlag::~GoalReachUniqueFlag()
{
}


/******************************** METHODS TO IMPLEMENT *********************************/

void GoalReachUniqueFlag::setup(MapBuilder* pMapBuilder)
{
    assert(pMapBuilder);

    unsigned int n = pMapBuilder->getRandomNumberGenerator()->randomize(0, 1);

    pMapBuilder->addTargets(TARGET_FLAG, (n == 0 ? 1 : 2), "Flags/flag_mash_blue", (n == 0 ? 1 : 0));
    pMapBuilder->addTargets(TARGET_FLAG, (n == 0 ? 2 : 1), "Flags/flag_mash", (n == 0 ? 0 : 1));
}


tResult GoalReachUniqueFlag::onTargetReached(tTarget* pTarget, float &reward)
{
    assert(pTarget);

    // Unique flag
    if (pTarget->goal_specific == 1)
    {
        reward = 10.0f;
        return RESULT_SUCCESS;
    }

    reward = -5.0f;
    return RESULT_NONE;
}
