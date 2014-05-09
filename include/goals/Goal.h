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


#ifndef _GOAL_H_
#define _GOAL_H_

#include <Athena/Prerequisites.h>
#include <MapBuilder.h>
#include <Declarations.h>
#include <teachers/Teacher.h>


class Goal
{
    //_____ Internal types __________
protected:
    typedef std::vector<Athena::Physics::PhysicalComponent*>    tContactsList;
    typedef Athena::Utils::VectorIterator<tContactsList>        tContactsIterator;
    typedef tContactsList::iterator                             tContactsNativeIterator;


    //_____ Construction / Destruction __________
public:
    Goal();
    virtual ~Goal();


    //_____ Methods __________
public:
    void finalize(Map* pMap);

    tResult process(float &reward, Teacher* pTeacher = 0);

    bool updateTimeout(float elapsedMilliseconds);

    inline bool isInitialized() const
    {
        return m_bInitialized;
    }

    inline bool isAvatarFalling() const
    {
        return m_bFalling;
    }


    //_____ Methods to implement __________
public:
    virtual void setup(MapBuilder* pMapBuilder) = 0;

    virtual void update() {}

    virtual void reset() {}

protected:
    virtual tResult onTargetReached(tTarget* pTarget, float &reward);
    virtual tResult onAvatarMoved(float &reward);


    //_____ Attributes __________
protected:
    Athena::Physics::World*             m_pPhysicalWorld;
    Athena::Entities::Entity*           m_pAvatar;
    Athena::Physics::GhostObject*       m_pAvatarGhost;
    Athena::Physics::Body*              m_pAvatarBody;
    Map*                                m_pMap;
    tContactsList                       m_contacts;
    bool                                m_bFalling;
    bool                                m_bInitialized;
    float                               m_fTimeout;
    bool                                m_bNegativeCollisionRewards;
};

#endif
