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


#include <teachers/TeacherEatAllTargets.h>
#include <Athena-Entities/Transforms.h>
#include <Athena-Graphics/Debug/Axes.h>
#include <Athena-Entities/ComponentsManager.h>

using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Utils;
using namespace std;


static Debug::Axes* pAxes = 0;

/***************************** CONSTRUCTION / DESTRUCTION ******************************/

TeacherEatAllTargets::TeacherEatAllTargets(Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
: Teacher(pMap, pCamera), current_target(-1)
{
}


TeacherEatAllTargets::~TeacherEatAllTargets()
{
}


/************************************** METHODS ****************************************/

tAction TeacherEatAllTargets::computeNextAction()
{
    if ((current_target == -1) && !m_detected_targets.empty())
    {
        float min_distance = 0.0f;

        for (int i = 0; i < m_detected_targets.size(); ++i)
        {
            tPoint target = m_detected_targets[i];

            if (isCellVisible(target.x, target.y))
            {
                float distance = getDistanceToTarget(target);
                if ((current_target == -1) || (distance < min_distance))
                {
                    min_distance = distance;
                    current_target = i;
                }
            }
        }
    }

    if (current_target >= 0)
    {
        Degree angle = getAngleToTarget(m_detected_targets[current_target]);

        if (angle.valueDegrees() >= 3.0f)
            return ACTION_TURN_RIGHT;
        else if (angle.valueDegrees() <= -3.0f)
            return ACTION_TURN_LEFT;
        else
            return ACTION_GO_FORWARD;
    }

    return ACTION_TURN_LEFT;
}


Mash::tActionsList TeacherEatAllTargets::notRecommendedActions()
{
    Mash::tActionsList actions;

    if (m_nextAction == ACTIONS_COUNT)
        nextAction();

    if (current_target >= 0)
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


void TeacherEatAllTargets::onTargetReached(tTarget* pTarget)
{
    if (current_target >= 0)
        m_detected_targets.erase(m_detected_targets.begin() + current_target);

    current_target = -1;

    for (unsigned int i = 0; i < m_pMap->width * m_pMap->height; ++i)
    {
        if ((m_grid[i] != CELL_UNKNOWN) && (m_grid[i] != m_pMap->grid[i].type))
            m_grid[i] = CELL_UNKNOWN;
    }
}
