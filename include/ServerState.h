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


#ifndef _SERVERSTATE_H_
#define _SERVERSTATE_H_

#include <Athena/GameStates/IGameState.h>
#include <Map.h>
#include <goals/Goal.h>
#include <teachers/Teacher.h>
#include <Ogre/OgreTexture.h>


class ServerState: public Athena::GameStates::IGameState
{
        //_____ Construction / Destruction __________
public:
    ServerState(bool bEnableSecrets);
    virtual ~ServerState();


    //_____ Methods __________
public:
    void setup(const std::string& goal, const std::string& environment,
               unsigned int globalSeed);

    inline Map* getMap() const
    {
        return m_pMap;
    }

    inline Goal* getGoal() const
    {
        return m_pGoal;
    }

    void reset();
    void resetTask();

    bool performAction(tAction action, float elapsedMilliseconds);

    inline tResult result() const
    {
        return m_result;
    }

    inline float getLastReward() const
    {
        return m_fReward;
    }

    inline std::string getLastEvent() const
    {
        return m_strEvent;
    }

    unsigned char* getAvatarView(size_t &nbBytes);

    tAction getTeacherAction();

    Mash::tActionsList getNotRecommendedActions();


protected:
    bool retrieveCurrentView();


    //_____ Methods to be overriden by each state __________
public:
    virtual void enter();
    virtual void exit();
    virtual void pause();
    virtual void resume();

    virtual void process();


    //_____ Internal types __________
public:
    enum tSeed
    {
        SEED_GOAL,
        SEED_MAP,
        SEED_MOVEMENTS,

        COUNT_SEEDS
    };


    //_____ Attributes __________
private:
    unsigned int                      m_seeds[COUNT_SEEDS];
    Ogre::TexturePtr                  m_texture;
    Ogre::RenderTexture*              m_pRenderTexture;
    Athena::Entities::Entity*         m_pAvatar;
    Athena::Physics::Body*            m_pAvatarBody;
    Athena::Physics::GhostObject*     m_pAvatarGhost;
    Ogre::Overlay*                    m_pOverlay;
    std::string                       m_selectedMap;
    std::string                       m_selectedGoal;
    Teacher*                          m_pTeacher;
    Map*                              m_pMap;
    Goal*                             m_pGoal;
    bool                              m_bEnableSecrets;
    tResult                           m_result;
    float                             m_fReward;
    std::string                       m_strEvent;
    unsigned char*                    m_pCurrentView;
};

#endif
