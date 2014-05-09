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


#include <teachers/TeacherReachOneFlagTwoRooms.h>
#include <Athena-Entities/Transforms.h>

using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Utils;
using namespace std;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

TeacherReachOneFlagTwoRooms::TeacherReachOneFlagTwoRooms(Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
: Teacher(pMap, pCamera), flag_found(false), door_reached(false)
{
    door.x = -1;
    door.y = -1;
}


TeacherReachOneFlagTwoRooms::~TeacherReachOneFlagTwoRooms()
{
}


/************************************** METHODS ****************************************/

tAction TeacherReachOneFlagTwoRooms::computeNextAction()
{
    if (!flag_found)
    {
        if (!m_detected_targets.empty())
            flag_found = true;
    }

    if (!door_reached && (door.x < 0))
    {
        if (!m_new_waypoints.empty())
        {
            int index = m_new_waypoints[0].y * m_pMap->width + m_new_waypoints[0].x;
            door.x = m_pMap->grid[index].main_x;
            door.y = m_pMap->grid[index].main_y;
            m_new_waypoints.clear();
        }
    }

    tPoint target;
    target.x = -1;

    if (flag_found)
    {
        int index_flag = m_detected_targets[0].y * m_pMap->width + m_detected_targets[0].x;
        int index_robot = m_robot_position.y * m_pMap->width + m_robot_position.x;

        if ((m_pMap->grid[index_robot].type == CELL_WAYPOINT) || (m_pMap->grid[index_flag].room == m_pMap->grid[index_robot].room))
            target = m_detected_targets[0];
    }

    if ((target.x < 0) && (door.x >= 0))
    {
        int index_robot = m_robot_position.y * m_pMap->width + m_robot_position.x;
        if (m_pMap->grid[index_robot].type == CELL_WAYPOINT)
        {
            door.x = -1;
            door.y = -1;
            door_reached = true;
        }
        else
        {
            target = door;
        }
    }

    if (target.x >= 0)
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


Mash::tActionsList TeacherReachOneFlagTwoRooms::notRecommendedActions()
{
    Mash::tActionsList actions;

    if (m_nextAction == ACTIONS_COUNT)
        nextAction();

    if (flag_found)
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
