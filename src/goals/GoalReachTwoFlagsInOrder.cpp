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


#include <goals/GoalReachTwoFlagsInOrder.h>


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

GoalReachTwoFlagsInOrder::GoalReachTwoFlagsInOrder()
{
}


GoalReachTwoFlagsInOrder::~GoalReachTwoFlagsInOrder()
{
}


/******************************** METHODS TO IMPLEMENT *********************************/

void GoalReachTwoFlagsInOrder::setup(MapBuilder* pMapBuilder)
{
    assert(pMapBuilder);

    pMapBuilder->addTargets(TARGET_FLAG, 1, "Flags/flag_mash_blue", 1);
    pMapBuilder->addTargets(TARGET_FLAG, 1, "Flags/flag_mash", 2);
}


tResult GoalReachTwoFlagsInOrder::onTargetReached(tTarget* pTarget, float &reward)
{
    assert(pTarget);

    // First flag
    if (pTarget->goal_specific == 1)
    {
        if (!m_bFirstFlagReached)
        {
            reward = 5.0f;
            m_bFirstFlagReached = true;
        }
    }

    // Second flag
    else if (pTarget->goal_specific == 2)
    {
        if (m_bFirstFlagReached)
        {
            reward = 10.0f;
            return RESULT_SUCCESS;
        }
        else
        {
            reward = -5.0f;
        }
    }

    return RESULT_NONE;
}
