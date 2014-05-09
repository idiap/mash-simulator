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


/** @file   Simulator.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the class 'Simulator'
*/

#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <Athena/Prerequisites.h>
#include <Athena/Engine.h>
#include <Athena-Inputs/Declarations.h>
#include <mash-utils/declarations.h>
#include <ServerState.h>


//---------------------------------------------------------------------------------------
/// @brief  Main class of the simulator
//---------------------------------------------------------------------------------------
class Simulator
{
    //_____ Construction / Destruction __________
public:
    Simulator();
    ~Simulator();


    //_____ Methods __________
public:
    bool init(bool bGame, const std::string& goal = "",
              const std::string& environment = "", bool bEnableSecrets = false);

    //--------------------------------------------------------------------------
    /// @brief Returns a list containing the name of the available goals
    //--------------------------------------------------------------------------
    Mash::tStringList getGoals();

    //--------------------------------------------------------------------------
    /// @brief Returns a list containing the name of the environment
    ///        suitable for the given goal
    ///
    /// @param goal         The name of the goal
    //--------------------------------------------------------------------------
    Mash::tStringList getEnvironments(const std::string& goal);

    //--------------------------------------------------------------------------
    /// @brief Returns the capabilities of a task
    ///
    /// @param goal         The name of the goal
    /// @param environment  The name of the environment
    //--------------------------------------------------------------------------
    Mash::tIASCapabilities capabilities(const std::string& goal,
                                        const std::string& environment);

    bool isGoalSecret(const std::string& strGoal);

    bool isEnvironmentSecret(const std::string& strEnvironment);


    //_____ Game mode methods __________
public:
    bool run();


    //_____ Server mode methods __________
public:
    void setup(const std::string& goal, const std::string& environment,
               unsigned int globalSeed);

    inline Map* getMap() const
    {
        assert(m_pServerState);

        return m_pServerState->getMap();
    }

    inline void reset()
    {
        assert(m_pServerState);

        m_pServerState->reset();
    }

    inline void restart()
    {
        assert(m_pServerState);

        m_pServerState->resetTask();
        stepOneFrame();
    }

    tResult performAction(tAction action, float &fReward, std::string &strEvent);

    unsigned char* getAvatarView(size_t &nbBytes);

    tAction getTeacherAction();

    Mash::tActionsList getNotRecommendedActions();


private:
    bool stepOneFrame();


    //_____ Attributes __________
private:
    bool                                m_bGame;
    Athena::Engine                      m_engine;
    Athena::Inputs::VirtualController*  m_pController;
    ServerState*                        m_pServerState;

    static const Athena::Utils::tID     STATE_FPS       = 0;
    static const Athena::Utils::tID     STATE_SERVER    = 1;
};

#endif
