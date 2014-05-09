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


#ifndef _FPSSTATE_H_
#define _FPSSTATE_H_

#include <Athena/GameStates/IGameState.h>
#include <Map.h>
#include <goals/Goal.h>
#include <teachers/Teacher.h>
#include <DebugDrawer.h>


class FPSState: public Athena::GameStates::IGameState
{
        //_____ Construction / Destruction __________
public:
    FPSState();
    virtual ~FPSState();


    //_____ Methods __________
public:
    void setup(const std::string& goal, const std::string& environment);

protected:
    void resetTask();


    //_____ Methods to be overriden by each state __________
public:
    virtual void enter();
    virtual void exit();
    virtual void pause();
    virtual void resume();

    virtual void process();


    //_____ Attributes __________
private:
    Ogre::Viewport*                 m_pViewport;
    Teacher*                        m_pTeacher;
    Athena::Entities::Entity*       m_pAvatar;
    Athena::Physics::Body*          m_pAvatarBody;
    Athena::Physics::GhostObject*   m_pAvatarGhost;
    Athena::Entities::Transforms*   m_pCameraAxis;
    Ogre::Overlay*                  m_pOverlay;
    Map*                            m_pMap;
    Goal*                           m_pGoal;
    std::string                     m_selectedMap;
    std::string                     m_selectedGoal;
    tResult                         m_result;
    OgreDebugDrawer*                m_pDebugDrawer;
};

#endif
