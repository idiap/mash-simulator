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


/** @file   SimulationServer.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'SimulationServer' class
*/

#include <Declarations.h>
#include <SimulationServer.h>
#include <mash-utils/random_number_generator.h>
#include <Ogre/OgreWindowEventUtilities.h>
#include <assert.h>

using namespace std;
using namespace Mash;

using Ogre::WindowEventUtilities;


bool SimulationServer::bEnableSecrets = false;


/************************* CONSTRUCTION / DESTRUCTION *************************/

SimulationServer::SimulationServer()
: m_pSimulator(0)
{
    setGlobalSeed(time(0));
}


SimulationServer::~SimulationServer()
{
    delete m_pSimulator;
}


/******************************* STATIC METHODS *******************************/

IApplicationServer* SimulationServer::create()
{
    return new SimulationServer();
}


/********************************** METHODS ***********************************/

void SimulationServer::setGlobalSeed(unsigned int seed)
{
    m_globalSeed = seed;
}


tStringList SimulationServer::getGoals()
{
    Simulator simulator;

    return simulator.getGoals();
}


tStringList SimulationServer::getEnvironments(const std::string& goal)
{
    Simulator simulator;

    return simulator.getEnvironments(goal);
}


std::string SimulationServer::getDataset(const std::string& goal,
                                         const std::string& environment)
{
    return "";
}


tStringList SimulationServer::getActions(const std::string& goal,
                                         const std::string& environment)
{
    tStringList actions;

    // We have the same actions for all the tasks
    actions.push_back("GO_FORWARD");
    actions.push_back("GO_BACKWARD");
    actions.push_back("TURN_LEFT");
    actions.push_back("TURN_RIGHT");

    return actions;
}


tViewsList SimulationServer::getViews(const std::string& goal,
                                      const std::string& environment)
{
    tViewsList views;

    tView view;
    view.name = "main";
    view.width = VIEW_WIDTH;
    view.height = VIEW_HEIGHT;

    views.push_back(view);

    return views;
}


tIASCapabilities SimulationServer::capabilities(const std::string& goal,
                                                const std::string& environment)
{
    Simulator simulator;

    return simulator.capabilities(goal, environment);
}


bool SimulationServer::initializeTask(const std::string& goal,
                                      const std::string& environment,
                                      const IApplicationServer::tSettingsList& settings)
{
    // Cleanup the previous world, if any
    if (m_pSimulator)
        delete m_pSimulator;

    // Create the simulator
    m_pSimulator = new Simulator();
    m_pSimulator->init(false, "", "", SimulationServer::bEnableSecrets);
    m_pSimulator->setup(goal, environment, m_globalSeed);

    return true;
}


bool SimulationServer::resetTask()
{
    // Restart the simulator
    m_pSimulator->restart();

    return true;
}


unsigned char* SimulationServer::getView(const std::string& view, size_t &nbBytes,
                                         std::string &mimetype)
{
    mimetype = "raw";

    // Retrieve the image of the view
    unsigned char* pImage = m_pSimulator->getAvatarView(nbBytes);
    if (!pImage)
        return 0;

    // Make a copy, because our caller expect to release the memory, and the
    // simulator wants to keep a copy of the image for caching purposes
    unsigned char* pCopy = new unsigned char[nbBytes];
    memcpy(pCopy, pImage, nbBytes);

    return pCopy;
}


bool SimulationServer::performAction(const std::string& action, float &reward,
                                     bool &finished, bool &failed,
                                     std::string &event)
{
    tAction theAction;

    if (action == "GO_FORWARD")
        theAction = ACTION_GO_FORWARD;
    else if (action == "GO_BACKWARD")
        theAction = ACTION_GO_BACKWARD;
    else if (action == "TURN_LEFT")
        theAction = ACTION_TURN_LEFT;
    else if (action == "TURN_RIGHT")
        theAction = ACTION_TURN_RIGHT;

    // Perform the action
    tResult result = m_pSimulator->performAction(theAction, reward, event);

    finished = (result == RESULT_SUCCESS);
    failed = (result == RESULT_FAILED);

    return true;
}


std::string SimulationServer::getSuggestedAction()
{
    tAction action = m_pSimulator->getTeacherAction();

    switch (action)
    {
        case ACTION_GO_FORWARD:  return "GO_FORWARD";
        case ACTION_GO_BACKWARD: return "GO_BACKWARD";
        case ACTION_TURN_LEFT:   return "TURN_LEFT";
        case ACTION_TURN_RIGHT:  return "TURN_RIGHT";

        default:
            break;
    }

    return "";
}


tStringList SimulationServer::notRecommendedActions()
{
    tStringList strActions;

    tActionsList actions = m_pSimulator->getNotRecommendedActions();
    tActionsList::iterator iter, iterEnd;

    for (iter = actions.begin(), iterEnd = actions.end(); iter != iterEnd; ++iter)
    {
        switch (*iter)
        {
            case ACTION_GO_FORWARD:  strActions.push_back("GO_FORWARD"); break;
            case ACTION_GO_BACKWARD: strActions.push_back("GO_BACKWARD"); break;
            case ACTION_TURN_LEFT:   strActions.push_back("TURN_LEFT"); break;
            case ACTION_TURN_RIGHT:  strActions.push_back("TURN_RIGHT"); break;
        }
    }

    return strActions;
}


void SimulationServer::onTimeout()
{
    WindowEventUtilities::messagePump();
}
