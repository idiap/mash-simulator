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


/** @file   Simulator.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the class 'Simulator'
*/

#include <Declarations.h>
#include <Simulator.h>
#include <FPSState.h>
#include <Athena-Inputs/InputsUnit.h>
#include <Athena/Tasks/TaskManager.h>
#include <Athena/GameStates/GameStateManager.h>
#include <Ogre/OgreException.h>
#include <Ogre/OgreWindowEventUtilities.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::GameStates;
using namespace Mash;

using Ogre::WindowEventUtilities;


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

Simulator::Simulator()
: m_pController(0), m_bGame(false), m_pServerState(0)
{
}


Simulator::~Simulator()
{
}


/************************************** METHODS ****************************************/

bool Simulator::init(bool bGame, const std::string& goal, const std::string& environment,
                     bool bEnableSecrets)
{
    // Assertions
    assert(!m_pController);

    m_bGame = bGame;

    try
    {
        // Game mode
        if (bGame)
        {
            if (goal.empty() || environment.empty())
                return false;

            if (!bEnableSecrets && (isGoalSecret(goal) || isEnvironmentSecret(environment)))
                return false;

            tStringList environments = getEnvironments(goal);
            if (environments.empty())
                return false;

            tStringIterator iter, iterEnd;
            bool bFound = false;
            for (iter = environments.begin(), iterEnd = environments.end(); iter != iterEnd; ++iter)
            {
                if (*iter == environment)
                {
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
                return false;

            // Initialize Athena
            m_engine.setup(std::string("athena.cfg"));

            // Create the main window
            m_engine.createRenderWindow("MainWindow", "MASH Simulator", VIEW_WIDTH, VIEW_HEIGHT, false);

            // Create the controller
            InputsUnit* pInputsUnit = m_engine.getInputsUnit();

            pInputsUnit->registerVirtualID("EXIT",          VKEY_EXIT);
            pInputsUnit->registerVirtualID("UP",            VKEY_UP);
            pInputsUnit->registerVirtualID("DOWN",          VKEY_DOWN);
            pInputsUnit->registerVirtualID("LEFT",          VKEY_LEFT);
            pInputsUnit->registerVirtualID("RIGHT",         VKEY_RIGHT);
            pInputsUnit->registerVirtualID("RESET_TASK",    VKEY_RESET_TASK);

            m_pController = pInputsUnit->createVirtualController("MainController");
            if (!m_pController)
                return false;

            Controller* pKeyboard = pInputsUnit->getController(OIS::OISKeyboard);

            m_pController->addVirtualKey(VKEY_EXIT,         pKeyboard, Inputs::KEY_ESCAPE);
            m_pController->addVirtualKey(VKEY_UP,           pKeyboard, Inputs::KEY_UP);
            m_pController->addVirtualKey(VKEY_DOWN,         pKeyboard, Inputs::KEY_DOWN);
            m_pController->addVirtualKey(VKEY_LEFT,         pKeyboard, Inputs::KEY_LEFT);
            m_pController->addVirtualKey(VKEY_RIGHT,        pKeyboard, Inputs::KEY_RIGHT);
            m_pController->addVirtualKey(VKEY_RESET_TASK,   pKeyboard, Inputs::KEY_TAB);

            m_pController->enable(true);

            // Create the main state
            GameStateManager* pGameStateManager = m_engine.getGameStateManager();

            FPSState * pFPSState = new FPSState();
            pFPSState->setup(goal, environment);

            pGameStateManager->registerState(STATE_FPS, pFPSState);
            pGameStateManager->pushState(STATE_FPS);
        }

        // Server mode
        else
        {
            // Initialize Athena
            m_engine.setup(std::string("athena.cfg"));

            // Create the main window
            m_engine.createRenderWindow("MainWindow", "MASH Simulator", VIEW_WIDTH, VIEW_HEIGHT, false);

            // Create the main state
            GameStateManager* pGameStateManager = m_engine.getGameStateManager();

            m_pServerState = new ServerState(bEnableSecrets);

            pGameStateManager->registerState(STATE_SERVER, m_pServerState);
            pGameStateManager->pushState(STATE_SERVER);
        }
    }
    catch (Ogre::Exception& e)
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!",
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif

        return false;
    }

    return true;
}


tStringList Simulator::getGoals()
{
    tStringList goals;

    goals.push_back("reach_1_flag");
    goals.push_back("reach_2_flags_in_order");
    goals.push_back("reach_unique_flag");
    goals.push_back("all_you_can_eat");
    goals.push_back("eat_black_disks");
    goals.push_back("follow_the_light");
    goals.push_back("follow_the_arrow");
    goals.push_back("follow_the_line");
    goals.push_back("follow_the_blobs");
    goals.push_back("reach_correct_pillar");
    goals.push_back("reach_correct_object");
    goals.push_back("secret");

    return goals;
}


tStringList Simulator::getEnvironments(const std::string& goal)
{
    tStringList environments;

    if ((goal == "reach_1_flag") ||
        (goal == "reach_2_flags_in_order") ||
        (goal == "reach_unique_flag") ||
        (goal == "all_you_can_eat") ||
        (goal == "reach_correct_pillar") ||
        (goal == "reach_correct_object"))
    {
        environments.push_back("SingleRoom");
        environments.push_back("MediumRoom");
        environments.push_back("L-ShapedCorridor");
        environments.push_back("TwoRooms");
        environments.push_back("HugeRoom");
        // environments.push_back("HalfWalls");
    }
    else if (goal == "follow_the_light")
    {
        environments.push_back("LightRoom");
    }
    else if (goal == "follow_the_arrow")
    {
        environments.push_back("T-ShapedCorridor");
    }
    else if (goal == "follow_the_line")
    {
        environments.push_back("Line");
    }
    else if (goal == "follow_the_blobs")
    {
        environments.push_back("BlobsRoom");
    }
    else if (goal == "eat_black_disks")
    {
        environments.push_back("HugeRoom");
    }
    else if (goal == "secret")
    {
        environments.push_back("Secret");
    }

    return environments;
}


tIASCapabilities Simulator::capabilities(const std::string& goal,
                                         const std::string& environment)
{
    if ((goal == "reach_1_flag") ||
        ((goal == "follow_the_light") && (environment == "LightRoom")) ||
        ((goal == "follow_the_arrow") && (environment == "T-ShapedCorridor")) ||
        ((goal == "follow_the_line") && (environment == "Line")) ||
        ((goal == "follow_the_blobs") && (environment == "BlobsRoom")) ||
        ((goal == "eat_black_disks") && (environment == "HugeRoom")) ||
        ((goal == "reach_correct_pillar") && (environment == "SingleRoom")) ||
        ((goal == "reach_correct_pillar") && (environment == "MediumRoom")) ||
        ((goal == "reach_correct_object") && (environment == "SingleRoom")) ||
        ((goal == "reach_correct_object") && (environment == "MediumRoom")))
    {
        return IAS_CAP_INTERACTION | IAS_CAP_SUGGESTED_ACTION | IAS_CAP_NOT_RECOMMENDED_ACTIONS;
    }

    return IAS_CAP_INTERACTION;
}


bool Simulator::isGoalSecret(const std::string& goal)
{
    return (goal == "secret");
}


bool Simulator::isEnvironmentSecret(const std::string& environment)
{
    return (environment == "Secret");
}


/********************************* GAME MODE METHODS ***********************************/

bool Simulator::run()
{
    // Assertions
    assert(m_engine.getTaskManager());
    assert(m_pController);
    assert(m_bGame);

    try
    {
        m_engine.getTaskManager()->execute();
    }
    catch (Ogre::Exception& e)
    {
#if ATHENA_PLATFORM == ATHENA_PLATFORM_WIN32
        MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!",
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif

        return false;
    }

    return true;
}


/******************************** SERVER MODE METHODS **********************************/

void Simulator::setup(const std::string& goal, const std::string& environment,
                      unsigned int globalSeed)
{
    assert(!goal.empty());
    assert(!environment.empty());
    assert(m_pServerState);

    reset();

    m_pServerState->setup(goal, environment, globalSeed);

    try
    {
         while (!m_pServerState->getGoal()->isInitialized())
             m_engine.getTaskManager()->step(1e5);
    }
    catch (Ogre::Exception& e)
    {
         std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
    }
}


tResult Simulator::performAction(tAction action, float &fReward, std::string &strEvent)
{
    assert(m_pServerState);

    if (!m_pServerState->performAction(action, 100.0))
    {
        fReward = -1000.0f;
        return m_pServerState->result();
    }

    if (!stepOneFrame())
        return RESULT_NONE;

    fReward = m_pServerState->getLastReward();
    strEvent = m_pServerState->getLastEvent();

    return m_pServerState->result();
}


unsigned char* Simulator::getAvatarView(size_t &nbBytes)
{
    assert(m_pServerState);

    return m_pServerState->getAvatarView(nbBytes);
}


tAction Simulator::getTeacherAction()
{
    assert(m_pServerState);

    return m_pServerState->getTeacherAction();
}


tActionsList Simulator::getNotRecommendedActions()
{
    assert(m_pServerState);

    return m_pServerState->getNotRecommendedActions();
}


bool Simulator::stepOneFrame()
{
    try
    {
        WindowEventUtilities::messagePump();
        m_engine.getTaskManager()->step(1e5);
    }
    catch (Ogre::Exception& e)
    {
        std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
        return false;
    }

    return true;
}
