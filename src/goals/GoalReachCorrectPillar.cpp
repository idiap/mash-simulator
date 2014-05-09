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


#include <goals/GoalReachCorrectPillar.h>


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

GoalReachCorrectPillar::GoalReachCorrectPillar()
{
}


GoalReachCorrectPillar::~GoalReachCorrectPillar()
{
}


/******************************** METHODS TO IMPLEMENT *********************************/

void GoalReachCorrectPillar::setup(MapBuilder* pMapBuilder)
{
    assert(pMapBuilder);

    int color1 = pMapBuilder->getRandomNumberGenerator()->randomize(-100, 100);
    int color2 = pMapBuilder->getRandomNumberGenerator()->randomize(-100, 100);
    unsigned int pattern = pMapBuilder->getRandomNumberGenerator()->randomize(0, 3);

    pMapBuilder->addTargets(TARGET_PILLAR, 1, (color1 >= 0 ? "Pillars/Diagonal/Right/Red" : "Pillars/Diagonal/Right/Blue"), 1);

    std::string material = "Pillars/";
    switch (pattern)
    {
        case 0: material += "Squares"; break;
        case 1: material += "Horizontal"; break;
        case 2: material += "Vertical"; break;
        case 3: material += "Diagonal/Left"; break;
    }

    material += (color2 >= 0 ? "/Red" : "/Blue");

    pMapBuilder->addTargets(TARGET_PILLAR, 1, material, 0);
}


tResult GoalReachCorrectPillar::onTargetReached(tTarget* pTarget, float &reward)
{
    assert(pTarget);

    // Unique flag
    if (pTarget->goal_specific == 1)
    {
        reward = 10.0f;
        return RESULT_SUCCESS;
    }

    reward = -5.0f;
    return RESULT_FAILED;
}
