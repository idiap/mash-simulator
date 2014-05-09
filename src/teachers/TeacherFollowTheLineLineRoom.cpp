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


#include <teachers/TeacherFollowTheLineLineRoom.h>
#include <Athena-Entities/Transforms.h>

using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Utils;
using namespace std;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

TeacherFollowTheLineLineRoom::TeacherFollowTheLineLineRoom(Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
: Teacher(pMap, pCamera), state(STATE_FORWARD_1)
{
    target.x = -1;
    target.y = -1;
}


TeacherFollowTheLineLineRoom::~TeacherFollowTheLineLineRoom()
{
}


/************************************** METHODS ****************************************/

tAction TeacherFollowTheLineLineRoom::computeNextAction()
{
    // Determine if a change of state is possible
    if (state == STATE_FORWARD_1)
    {
        if (target.x < 0)
        {
            target.x = m_pMap->properties.get("spot1_x")->toInt();
            target.y = m_pMap->properties.get("spot1_y")->toInt();
        }
        else
        {
            if ((m_robot_position.x >= target.x - 1) && (m_robot_position.x <= target.x + 1) &&
                (m_robot_position.y >= target.y - 1) && (m_robot_position.y <= target.y + 1))
            {
                target.x = m_pMap->properties.get("spot2_x")->toInt();
                target.y = m_pMap->properties.get("spot2_y")->toInt();

                state = STATE_FORWARD_2;
            }
        }
    }
    else if (state == STATE_FORWARD_2)
    {
        if ((m_robot_position.x >= target.x - 1) && (m_robot_position.x <= target.x + 1) &&
            (m_robot_position.y >= target.y - 1) && (m_robot_position.y <= target.y + 1))
        {
            target.x = -1;
            target.y = -1;

            state = STATE_LOOK_FOR_FLAG;
        }
    }
    else if (state == STATE_LOOK_FOR_FLAG)
    {
        if (!m_detected_targets.empty())
        {
            target = m_detected_targets[0];
            state = STATE_GO_TOWARD_FLAG;
        }
    }

    // Select the action to perform
    if (state == STATE_LOOK_FOR_FLAG)
    {
        if (m_pMap->properties.get("haxis")->toBool())
            return ACTION_TURN_RIGHT;
        else
            return ACTION_TURN_LEFT;
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


Mash::tActionsList TeacherFollowTheLineLineRoom::notRecommendedActions()
{
    Mash::tActionsList actions;

    if (m_nextAction == ACTIONS_COUNT)
        nextAction();

    actions.push_back(ACTION_GO_BACKWARD);

    if (state == STATE_LOOK_FOR_FLAG)
    {
        actions.push_back(ACTION_GO_FORWARD);

        if (m_pMap->properties.get("haxis")->toBool())
            actions.push_back(ACTION_TURN_LEFT);
        else
            actions.push_back(ACTION_TURN_RIGHT);
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
