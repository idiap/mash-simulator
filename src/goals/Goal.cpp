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


#include <goals/Goal.h>
#include <Map.h>
#include <Athena-Entities/Entity.h>
#include <Athena-Entities/Transforms.h>
#include <Athena-Physics/Body.h>
#include <Athena-Physics/World.h>
#include <Athena-Physics/GhostObject.h>


using namespace Athena;
using namespace Athena::Entities;
using namespace Athena::Physics;
using namespace Athena::Math;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

Goal::Goal()
: m_pPhysicalWorld(0), m_pAvatar(0), m_pAvatarGhost(0), m_pAvatarBody(0), m_pMap(0),
  m_bFalling(false), m_bInitialized(false), m_fTimeout(-1.0f),
  m_bNegativeCollisionRewards(true)
{
}


Goal::~Goal()
{
}


/************************************** METHODS ****************************************/

void Goal::finalize(Map* pMap)
{
    // Assertions
    assert(!m_pPhysicalWorld);
    assert(!m_pAvatar);
    assert(!m_pAvatarGhost);
    assert(!m_pAvatarBody);
    assert(!m_pMap);

    m_pMap           = pMap;
    m_pPhysicalWorld = Physics::World::cast(m_pMap->pScene->getMainComponent(COMP_PHYSICAL));
    m_pAvatar        = m_pMap->pScene->getEntity("Avatar");
    m_pAvatarGhost   = GhostObject::cast(m_pAvatar->getComponent(tComponentID(COMP_PHYSICAL, m_pAvatar->getName(), "Ghost")));
    m_pAvatarBody    = Body::cast(m_pAvatar->getComponent(tComponentID(COMP_PHYSICAL, m_pAvatar->getName(), "Body")));
}


tResult Goal::process(float &reward, Teacher* pTeacher)
{
    // Initialisations
    m_bFalling = true;
    reward = 0.0f;
    tResult result = RESULT_NONE;
    tContactsList currentContacts;

    // Retrieve the contact points
    for (unsigned int i = 0; i < m_pAvatarGhost->getNbOverlappingObjects(); ++i)
    {
        PhysicalComponent* pComponent = m_pAvatarGhost->getOverlappingObject(i);

        // No collision on ourself
        if (pComponent == m_pAvatarBody)
            continue;

        // Determine if we are falling (if we don't know yet)
        if (m_bFalling)
        {
            Physics::World::tContactPointsList contacts;
            bool bGhostIsA = m_pPhysicalWorld->getContacts(m_pAvatarGhost, pComponent, contacts);

            for (int j = 0; j < contacts.size(); ++j)
            {
                const btManifoldPoint& pt = contacts[j];
                const btVector3& point = (bGhostIsA ? pt.getPositionWorldOnA() : pt.getPositionWorldOnB());

                const Vector3& offset = fromBullet(point) - m_pAvatarBody->getTransforms()->getWorldPosition();

                if (MathUtils::RealEqual(offset.y, 0.0f, 0.05f) &&
                    MathUtils::RealEqual(offset.x, 0.0f, 0.1f) &&
                    MathUtils::RealEqual(offset.z, 0.0f, 0.1f))
                {
                    m_bFalling = false;
                    break;
                }
            }
        }

        // Add the physical component to the list of current contacts
        currentContacts.push_back(pComponent);

        // Determine if we were previously already colliding with this physical component
        tContactsIterator iter(m_contacts.begin(), m_contacts.end());
        bool bFound = false;
        while (iter.hasMoreElements())
        {
            PhysicalComponent* pTestedComponent = iter.getNext();
            if (pTestedComponent == pComponent)
            {
                bFound = true;
                break;
            }
        }

        if (bFound)
            continue;

        // Determine the collision group of the physical component
        ::tCollisionGroup group = (::tCollisionGroup) CollisionObject::cast(pComponent)->getCollisionGroup();

        // Compute the reward
        switch (group)
        {
            case GROUP_STATIC:
            case GROUP_AVATAR:
            case GROUP_DYNAMIC:
                if (m_bNegativeCollisionRewards)
                    reward += -1.0f;
                break;

            case GROUP_TRIGGER:
            {
                // Test if the component belongs to a target
                tTargetsIterator iter(m_pMap->targets.begin(), m_pMap->targets.end());
                while (iter.hasMoreElements())
                {
                    tTarget* pTarget = iter.peekNextPtr();

                    if (pTarget->pEntity == pComponent->getList()->getEntity())
                    {
                        float target_reward = 0.0f;
                        result = onTargetReached(pTarget, target_reward);
                        reward += target_reward;

                        if (pTeacher)
                            pTeacher->onTargetReached(pTarget);

                        break;
                    }

                    iter.moveNext();
                }

                // At this point the only triggers are targets, but additional
                // stuff must be done here once we have others.

                break;
            }

            default:
                break;
        }
    }

    m_contacts = currentContacts;

    if (!m_bFalling && (result == RESULT_NONE))
    {
        float move_reward = 0.0f;
        result = onAvatarMoved(move_reward);
        reward += move_reward;
    }

    if (!m_bInitialized)
        m_bInitialized = (MathUtils::RealEqual(0.0f, reward) && !m_bFalling);

    update();

    return result;
}


bool Goal::updateTimeout(float elapsedMilliseconds)
{
    if (m_fTimeout > 0.0f)
    {
        m_fTimeout -= elapsedMilliseconds;
        return (m_fTimeout <= 0.0f);
    }

    return false;
}


/******************************** METHODS TO IMPLEMENT *********************************/

tResult Goal::onTargetReached(tTarget* pTarget, float &reward)
{
    // Most basic behavior: positive reward and task solved
    reward = 10.0f;
    return RESULT_SUCCESS;
}


tResult Goal::onAvatarMoved(float &reward)
{
    return RESULT_NONE;
}
