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


#include <teachers/TeacherFollowTheBlobsInBlobsRoom.h>
#include <Athena-Entities/Transforms.h>
#include <Athena-Core/Utils/StringConverter.h>

using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Utils;
using namespace std;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

TeacherFollowTheBlobsInBlobsRoom::TeacherFollowTheBlobsInBlobsRoom(Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
: Teacher(pMap, pCamera), current_target_index(0)
{
    target.x = m_pMap->properties.get("spot0_x")->toInt();
    target.y = m_pMap->properties.get("spot0_y")->toInt();
}


TeacherFollowTheBlobsInBlobsRoom::~TeacherFollowTheBlobsInBlobsRoom()
{
}


/************************************** METHODS ****************************************/

tAction TeacherFollowTheBlobsInBlobsRoom::computeNextAction()
{
    int spots_count = m_pMap->properties.get("spots_count")->toInt();

    // Determine if a change of target is possible
    if (current_target_index < spots_count)
    {
        if ((m_robot_position.x >= target.x - 1) && (m_robot_position.x <= target.x + 1) &&
            (m_robot_position.y >= target.y - 1) && (m_robot_position.y <= target.y + 1))
        {
            current_target_index++;

            if (current_target_index < spots_count)
            {
                string spot_name = string("spot") + StringConverter::toString(current_target_index);

                target.x = m_pMap->properties.get(spot_name + "_x")->toInt();
                target.y = m_pMap->properties.get(spot_name + "_y")->toInt();
            }
            else
            {
                target.x = -1;
                target.y = -1;
            }
        }
    }

    if ((current_target_index >= spots_count) && (target.x == -1))
    {
        if (!m_detected_targets.empty())
            target = m_detected_targets[0];
    }

    if (target.x >= 0)
    {
        // Select the action to perform
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


Mash::tActionsList TeacherFollowTheBlobsInBlobsRoom::notRecommendedActions()
{
    Mash::tActionsList actions;

    if (m_nextAction == ACTIONS_COUNT)
        nextAction();

    actions.push_back(ACTION_GO_BACKWARD);

    if (m_nextAction == ACTION_TURN_RIGHT)
    {
        actions.push_back(ACTION_TURN_LEFT);
    }
    else if (m_nextAction == ACTION_TURN_LEFT)
    {
        actions.push_back(ACTION_TURN_RIGHT);
    }

    return actions;
}
