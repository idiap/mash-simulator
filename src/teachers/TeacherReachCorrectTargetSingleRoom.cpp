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


#include <teachers/TeacherReachCorrectTargetSingleRoom.h>
#include <Athena-Entities/Transforms.h>

using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Utils;
using namespace std;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

TeacherReachCorrectTargetSingleRoom::TeacherReachCorrectTargetSingleRoom(Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
: Teacher(pMap, pCamera), target_index(-1)
{
}


TeacherReachCorrectTargetSingleRoom::~TeacherReachCorrectTargetSingleRoom()
{
}


/************************************** METHODS ****************************************/

tAction TeacherReachCorrectTargetSingleRoom::computeNextAction()
{
    if (target_index == -1)
    {
        for (unsigned int i = 0; i < m_detected_targets.size(); ++i)
        {
            tPoint point = m_detected_targets[i];
            unsigned int index = m_pMap->grid[point.y * m_pMap->width + point.x].infos_index;

            if (m_pMap->targets[index].goal_specific == 1)
            {
                target_index = i;
                break;
            }
        }
    }

    if (target_index >= 0)
    {
        tPoint target = m_detected_targets[target_index];

        Degree angle = getAngleToTarget(target);

        if (angle.valueDegrees() >= 3.0f)
            return ACTION_TURN_RIGHT;
        else if (angle.valueDegrees() <= -3.0f)
            return ACTION_TURN_LEFT;
        else
            return ACTION_GO_FORWARD;
    }

    return ACTION_TURN_LEFT;
}


Mash::tActionsList TeacherReachCorrectTargetSingleRoom::notRecommendedActions()
{
    Mash::tActionsList actions;

    if (m_nextAction == ACTIONS_COUNT)
        nextAction();

    if (target_index >= 0)
    {
        if (m_nextAction == ACTION_GO_FORWARD)
        {
            actions.push_back(ACTION_GO_BACKWARD);
        }
        else if (m_nextAction == ACTION_TURN_RIGHT)
        {
            actions.push_back(ACTION_GO_BACKWARD);
            actions.push_back(ACTION_TURN_LEFT);
        }
        else if (m_nextAction == ACTION_TURN_LEFT)
        {
            actions.push_back(ACTION_GO_BACKWARD);
            actions.push_back(ACTION_TURN_RIGHT);
        }
    }
    else
    {
        actions.push_back(ACTION_GO_BACKWARD);
    }

    return actions;
}
