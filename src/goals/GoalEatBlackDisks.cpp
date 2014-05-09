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


#include <goals/GoalEatBlackDisks.h>


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

GoalEatBlackDisks::GoalEatBlackDisks()
: m_nbDisksCollected(0)
{
}


GoalEatBlackDisks::~GoalEatBlackDisks()
{
}


/******************************** METHODS TO IMPLEMENT *********************************/

void GoalEatBlackDisks::setup(MapBuilder* pMapBuilder)
{
    assert(pMapBuilder);

    pMapBuilder->addTargets(TARGET_DISK, 3, "Decals/Black-Disk", 0);

    m_nbDisksCollected = 0;
//    m_fTimeout = 15000;
}


tResult GoalEatBlackDisks::onTargetReached(tTarget* pTarget, float &reward)
{
    assert(m_pMap);
    assert(pTarget);

    reward = 10.0f;

    m_pMap->moveTarget(pTarget, m_pAvatar);

    ++m_nbDisksCollected;

//    m_fTimeout = 15000;

    // return (m_nbDisksCollected == 10) ? RESULT_SUCCESS : RESULT_NONE;
    return RESULT_NONE;
}
