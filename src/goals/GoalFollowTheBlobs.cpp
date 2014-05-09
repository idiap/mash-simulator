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


#include <goals/GoalFollowTheBlobs.h>
#include <Athena-Entities/Entity.h>
#include <Athena-Entities/Transforms.h>

using namespace Athena::Math;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

GoalFollowTheBlobs::GoalFollowTheBlobs()
{
}


GoalFollowTheBlobs::~GoalFollowTheBlobs()
{
}


/******************************** METHODS TO IMPLEMENT *********************************/

void GoalFollowTheBlobs::setup(MapBuilder* pMapBuilder)
{
    assert(pMapBuilder);

    bool red_flag = (pMapBuilder->getRandomNumberGenerator()->randomize(-100.0f, 100.0f) > 0.0f);

    pMapBuilder->addTargets(TARGET_FLAG, 1, (red_flag ? "Flags/flag_mash" : "Flags/flag_mash_blue"), 0);
}


tResult GoalFollowTheBlobs::onAvatarMoved(float &reward)
{
    assert(m_pMap);
    assert(m_pAvatar);

    Vector3 position = m_pAvatar->getTransforms()->getWorldPosition();

    int robot_position_x = ((int) ((position.x + m_pMap->cell_size * 0.0005f) * 1000.0f)) / m_pMap->cell_size;
    int robot_position_y = ((int) ((position.z + m_pMap->cell_size * 0.0005f) * 1000.0f)) / m_pMap->cell_size;

    if (m_pMap->grid[robot_position_y * m_pMap->width + robot_position_x].type != CELL_SPOT)
    {
        reward = -10.0f;
        return RESULT_FAILED;
    }

    return RESULT_NONE;
}
