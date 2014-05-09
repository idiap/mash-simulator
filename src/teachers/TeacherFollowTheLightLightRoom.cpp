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


#include <teachers/TeacherFollowTheLightLightRoom.h>
#include <Athena-Graphics/Visual/World.h>
#include <Athena-Graphics/Visual/PointLight.h>

using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace std;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

TeacherFollowTheLightLightRoom::TeacherFollowTheLightLightRoom(Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
: Teacher(pMap, pCamera)
{
}


TeacherFollowTheLightLightRoom::~TeacherFollowTheLightLightRoom()
{
}


/************************************** METHODS ****************************************/

tAction TeacherFollowTheLightLightRoom::computeNextAction()
{
    // Retrieve the color of the ambient light
    Visual::World* pWorld = dynamic_cast<Visual::World*>(m_pMap->pScene->getMainComponent(COMP_VISUAL));
    Color color = pWorld->getAmbientLight();

    if (color.b > 0.5f)
        return ACTION_GO_FORWARD;
    else if (color.g > 0.5f)
        return ACTION_TURN_RIGHT;
    else
        return ACTION_TURN_LEFT;
}


Mash::tActionsList TeacherFollowTheLightLightRoom::notRecommendedActions()
{
    Mash::tActionsList actions;

    if (m_nextAction == ACTIONS_COUNT)
        nextAction();

    if (m_nextAction == ACTION_GO_FORWARD)
    {
        actions.push_back(ACTION_GO_BACKWARD);
        actions.push_back(ACTION_TURN_LEFT);
        actions.push_back(ACTION_TURN_RIGHT);
    }
    else if (m_nextAction == ACTION_TURN_RIGHT)
    {
        actions.push_back(ACTION_GO_FORWARD);
        actions.push_back(ACTION_GO_BACKWARD);
        actions.push_back(ACTION_TURN_LEFT);
    }
    else if (m_nextAction == ACTION_TURN_LEFT)
    {
        actions.push_back(ACTION_GO_FORWARD);
        actions.push_back(ACTION_GO_BACKWARD);
        actions.push_back(ACTION_TURN_RIGHT);
    }

    return actions;
}
