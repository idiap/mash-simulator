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


#include <goals/GoalFollowTheArrow.h>

using namespace Athena::Utils;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

GoalFollowTheArrow::GoalFollowTheArrow()
{
}


GoalFollowTheArrow::~GoalFollowTheArrow()
{
}


/******************************** METHODS TO IMPLEMENT *********************************/

void GoalFollowTheArrow::setup(MapBuilder* pMapBuilder)
{
    assert(pMapBuilder);

    bool b1 = pMapBuilder->getRandomNumberGenerator()->randomize(-100.0f, 100.0f) > 0.0f;
    bool b2 = pMapBuilder->getRandomNumberGenerator()->randomize(-100.0f, 100.0f) > 0.0f;

    Variant* pValue = pMapBuilder->getMap()->properties.get("arrow_on_left");
    assert(pValue);

    bool arrow_on_left = pValue->toBool();

    pMapBuilder->addTargets(TARGET_FLAG, 1, b1 ? "Flags/flag_mash_blue" : "Flags/flag_mash", (arrow_on_left ? 1 : 0), 0);
    pMapBuilder->addTargets(TARGET_FLAG, 1, b2 ? "Flags/flag_mash_blue" : "Flags/flag_mash", (arrow_on_left ? 0 : 1), 1);
}


tResult GoalFollowTheArrow::onTargetReached(tTarget* pTarget, float &reward)
{
    // Good flag
    if (pTarget->goal_specific == 1)
    {
        reward = 10.0f;
        return RESULT_SUCCESS;
    }

    // Bad flag
    reward = -10.0f;
    return RESULT_FAILED;
}
