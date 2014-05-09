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


#include <teachers/TeacherFollowTheArrowTShapedCorridor.h>
#include <Athena-Entities/Transforms.h>

using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Utils;
using namespace std;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

TeacherFollowTheArrowTShapedCorridor::TeacherFollowTheArrowTShapedCorridor(Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
: Teacher(pMap, pCamera), state(STATE_GO_TOWARD_ARROW)
{
    target.x = -1;
    target.y = -1;
}


TeacherFollowTheArrowTShapedCorridor::~TeacherFollowTheArrowTShapedCorridor()
{
}


/************************************** METHODS ****************************************/

tAction TeacherFollowTheArrowTShapedCorridor::computeNextAction()
{
    // Determine if a change of state is possible
    if ((state == STATE_LOOK_LEFT_FOR_FLAG) || (state == STATE_LOOK_RIGHT_FOR_FLAG))
    {
        if (!m_detected_targets.empty())
        {
            for (unsigned int i = 0; i < m_detected_targets.size(); ++i)
            {
                tPoint target_pos = m_detected_targets[i];

                if (m_pMap->targets[m_pMap->grid[target_pos.y * m_pMap->width + target_pos.x].infos_index].goal_specific == 1)
                {
                    target = target_pos;
                    state = STATE_GO_TOWARD_FLAG;
                    break;
                }
            }
        }
    }
    else if (state == STATE_GO_TOWARD_ARROW)
    {
        if (target.x < 0)
        {
            if (!m_new_waypoints.empty())
            {
                // Don't really care about the door, we want to reach the spot behind the door
                target.x = m_pMap->spots[0].position.x;
                target.y = m_pMap->spots[0].position.y;
                m_new_waypoints.clear();
            }
        }
        else
        {
            int index_robot = m_robot_position.y * m_pMap->width + m_robot_position.x;
            if (m_pMap->grid[index_robot].type == CELL_SPOT)
            {
                target.x = -1;
                target.y = -1;

                Variant* pValue = m_pMap->properties.get("arrow_on_left");
                assert(pValue);

                state = (pValue->toBool() ? STATE_LOOK_LEFT_FOR_FLAG : STATE_LOOK_RIGHT_FOR_FLAG);
            }
        }
    }
    else if (target.x < 0)
    {
        for (unsigned int i = 0; i < m_detected_targets.size(); ++i)
        {
            tPoint target_pos = m_detected_targets[i];

            if (m_pMap->targets[m_pMap->grid[target_pos.y * m_pMap->width + target_pos.x].infos_index].goal_specific == 1)
            {
                target = target_pos;
                break;
            }
        }
    }

    // Select the action to perform
    if (state == STATE_LOOK_LEFT_FOR_FLAG)
    {
        return ACTION_TURN_LEFT;
    }
    else if (state == STATE_LOOK_RIGHT_FOR_FLAG)
    {
        return ACTION_TURN_RIGHT;
    }
    else if (target.x >= 0)
    {
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


Mash::tActionsList TeacherFollowTheArrowTShapedCorridor::notRecommendedActions()
{
    Mash::tActionsList actions;

    if (m_nextAction == ACTIONS_COUNT)
        nextAction();

    actions.push_back(ACTION_GO_BACKWARD);

    if (state == STATE_LOOK_LEFT_FOR_FLAG)
    {
        actions.push_back(ACTION_GO_FORWARD);
        actions.push_back(ACTION_TURN_RIGHT);
    }
    else if (state == STATE_LOOK_RIGHT_FOR_FLAG)
    {
        actions.push_back(ACTION_GO_FORWARD);
        actions.push_back(ACTION_TURN_LEFT);
    }
    else
    {
        if (m_nextAction == ACTION_TURN_RIGHT)
        {
            actions.push_back(ACTION_TURN_LEFT);
        }
        else if (m_nextAction == ACTION_TURN_LEFT)
        {
            actions.push_back(ACTION_TURN_RIGHT);
        }
    }

    return actions;
}
