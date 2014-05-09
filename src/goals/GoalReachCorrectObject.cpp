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


#include <goals/GoalReachCorrectObject.h>


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

GoalReachCorrectObject::GoalReachCorrectObject()
{
}


GoalReachCorrectObject::~GoalReachCorrectObject()
{
}


/******************************** METHODS TO IMPLEMENT *********************************/

void GoalReachCorrectObject::setup(MapBuilder* pMapBuilder)
{
    assert(pMapBuilder);

    unsigned int object = pMapBuilder->getRandomNumberGenerator()->randomize(0, 3);

    pMapBuilder->addTargets(TARGET_OBJECT, 1, "TargetObject", 1);

    std::string mesh;
    switch (object)
    {
        case 0: mesh = "ball.mesh"; break;
        case 1: mesh = "bottle.mesh"; break;
        case 2: mesh = "lamp.mesh"; break;
        case 3: mesh = "weight.mesh"; break;
    }

    pMapBuilder->addTargets(TARGET_OBJECT, 1, "TargetObject", 0, -1, mesh);
}


tResult GoalReachCorrectObject::onTargetReached(tTarget* pTarget, float &reward)
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
