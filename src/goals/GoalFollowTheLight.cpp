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


#include <goals/GoalFollowTheLight.h>
#include <Athena-Graphics/Visual/World.h>
#include <Athena-Graphics/Visual/PointLight.h>
#include <Athena-Entities/Entity.h>
#include <Athena-Math/Color.h>

using namespace Athena;
using namespace Athena::Graphics;
using namespace Athena::Entities;
using namespace Athena::Math;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

GoalFollowTheLight::GoalFollowTheLight()
: m_phase(PHASE_FORWARD), m_counter(0)
{
    m_bNegativeCollisionRewards = false;
}


GoalFollowTheLight::~GoalFollowTheLight()
{
}


/******************************** METHODS TO IMPLEMENT *********************************/

void GoalFollowTheLight::setup(MapBuilder* pMapBuilder)
{
    assert(pMapBuilder);

    m_counter = 0;
}


void GoalFollowTheLight::update()
{
    if (m_counter == 0)
    {
        m_phase = tPhase(m_pMap->generator.randomize(0, PHASES_COUNT * 10 - 1) / 10);
        m_counter = m_pMap->generator.randomize(10, 40);

        // Determine the color of the lights
        Color color;
        if (m_phase == PHASE_FORWARD)
            color = Color(0.7f, 0.7f, 0.7f);
        else if (m_phase == PHASE_LEFT)
            color = Color(0.7f, 0.0f, 0.0f);
        else if (m_phase == PHASE_RIGHT)
            color = Color(0.0f, 0.7f, 0.0f);

        // Change the color of the ambient light
        Visual::World* pWorld = dynamic_cast<Visual::World*>(m_pMap->pScene->getMainComponent(COMP_VISUAL));
        pWorld->setAmbientLight(color);

        // Change the color of each light
        Entity::tEntitiesIterator iter(m_pMap->lights.begin(), m_pMap->lights.end());
        while (iter.hasMoreElements())
        {
            Entity* pEntity = iter.getNext();

            Visual::PointLight* pLight = Visual::PointLight::cast(pEntity->getComponent(tComponentID(COMP_VISUAL, pEntity->getName(), "PointLight")));
            pLight->setDiffuseColor(color);
        }
    }
    else
    {
        --m_counter;
    }
}
