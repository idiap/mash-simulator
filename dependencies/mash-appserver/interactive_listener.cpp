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


/** @file   interactive_listener.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'InteractiveListener' class
*/

#include "interactive_listener.h"
#include <mash-network/server.h>
#include <mash-utils/stringutils.h>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>


using namespace std;
using namespace Mash;


/********************************** CONSTANTS *********************************/

const char* PROTOCOL = "1.5";


/****************************** STATIC ATTRIBUTES *****************************/

InteractiveListener::tCommandHandlersList   InteractiveListener::handlers;
bool                                        InteractiveListener::bVerbose = false;
tApplicationServerConstructor*              InteractiveListener::pConstructor = 0;
struct timeval                              InteractiveListener::timeout = { 0 };


/************************* CONSTRUCTION / DESTRUCTION *************************/

InteractiveListener::InteractiveListener(int socket)
: ServerListener(socket), _pApplicationServer(0), _bDoingSetup(false),
  _bGlobalSeedSelected(false), _capabilities(0)
{
    char buffer1[50];
    char buffer2[50];

    sprintf(buffer1, "ApplicationServer #%d", socket);
    sprintf(buffer2, "listener_%d_$TIMESTAMP.log", socket);

    _outStream.setVerbosityLevel(1);
    _outStream.open(buffer1, Server::strLogFolder + buffer2, 200 * 1024);

    _timeout = InteractiveListener::timeout;

    _pApplicationServer = pConstructor();
}


InteractiveListener::~InteractiveListener()
{
    delete _pApplicationServer;
}


/********************** IMPLEMENTATION OF ServerListener **********************/

ServerListener::tAction InteractiveListener::handleCommand(
                                        const std::string& strCommand,
                                        const ArgumentsList& arguments)
{
    tCommandHandlersIterator iter = handlers.find(strCommand);
    if (iter != handlers.end())
    {
        tCommandHandler handler = iter->second;
        return (this->*handler)(arguments);
    }

    if (!_bDoingSetup)
    {
        if  (!sendResponse("UNKNOWN_COMMAND", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;
    }
    else
    {
        _taskParameters[strCommand] = arguments;

        if  (!sendResponse("OK", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;
    }

    return ACTION_NONE;
}


void InteractiveListener::onTimeout()
{
    _pApplicationServer->onTimeout();
}


/******************************* STATIC METHODS *******************************/

void InteractiveListener::initialize(bool bVerbose,
                                     tApplicationServerConstructor* applicationServerConstructor,
                                     struct timeval* pTimeout)
{
    handlers["STATUS"]                  = &InteractiveListener::handleStatusCommand;
    handlers["INFO"]                    = &InteractiveListener::handleInfoCommand;
    handlers["DONE"]                    = &InteractiveListener::handleDoneCommand;
    handlers["LOGS"]                    = &InteractiveListener::handleLogsCommand;
    handlers["SLEEP"]                   = &InteractiveListener::handleSleepCommand;
    handlers["RESET"]                   = &InteractiveListener::handleResetCommand;
    handlers["USE_GLOBAL_SEED"]         = &InteractiveListener::handleUseGlobalSeedCommand;
    handlers["LIST_GOALS"]              = &InteractiveListener::handleListGoalsCommand;
    handlers["LIST_ENVIRONMENTS"]       = &InteractiveListener::handleListEnvironmentsCommand;
    handlers["GET_DATASET"]             = &InteractiveListener::handleGetDatasetCommand;
    handlers["INITIALIZE_TASK"]         = &InteractiveListener::handleInitializeTaskCommand;
    handlers["BEGIN_TASK_SETUP"]        = &InteractiveListener::handleBeginTaskSetupCommand;
    handlers["END_TASK_SETUP"]          = &InteractiveListener::handleEndTaskSetupCommand;
    handlers["RESET_TASK"]              = &InteractiveListener::handleResetTaskCommand;
    handlers["GET_TRAJECTORIES_COUNT"]  = &InteractiveListener::handleGetNbTrajectoriesCommand;
    handlers["GET_TRAJECTORY_LENGTH"]   = &InteractiveListener::handleGetTrajectoryLengthCommand;
    handlers["GET_VIEW"]                = &InteractiveListener::handleGetViewCommand;
    handlers["ACTION"]                  = &InteractiveListener::handleActionCommand;

    InteractiveListener::bVerbose      = bVerbose;
    InteractiveListener::pConstructor  = applicationServerConstructor;

    if (pTimeout)
    {
        InteractiveListener::timeout = *pTimeout;
    }
    else
    {
        InteractiveListener::timeout.tv_sec = 0;
        InteractiveListener::timeout.tv_usec = 0;
    }
}


ServerListener* InteractiveListener::createListener(int socket)
{
    return new InteractiveListener(socket);
}


std::string InteractiveListener::getProtocol()
{
    return PROTOCOL;
}


/********************************** METHODS ***********************************/

ServerListener::tAction InteractiveListener::handleStatusCommand(const ArgumentsList& arguments)
{
    if (!sendResponse("READY", ArgumentsList()))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleInfoCommand(const ArgumentsList& arguments)
{
    if (!sendResponse("TYPE", ArgumentsList("ApplicationServer")))
        return ACTION_CLOSE_CONNECTION;

    if (!sendResponse("SUBTYPE", ArgumentsList("Interactive")))
        return ACTION_CLOSE_CONNECTION;

    if (!sendResponse("PROTOCOL", ArgumentsList(PROTOCOL)))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleDoneCommand(const ArgumentsList& arguments)
{
    sendResponse("GOODBYE", ArgumentsList());
    return ACTION_CLOSE_CONNECTION;
}


ServerListener::tAction InteractiveListener::handleLogsCommand(const ArgumentsList& arguments)
{
    const int MAX_SIZE = 200 * 1024;

    unsigned char* pData = 0;
    int size = 0;

    size = _outStream.dump(&pData, MAX_SIZE);
    if (size > 0)
    {
        ArgumentsList args;
        args.add("ApplicationServer.log");
        args.add(size);
        sendResponse("LOG_FILE", args);
        sendData((const unsigned char*) pData, size);
        delete[] pData;
    }

    if (!sendResponse("END_LOGS", ArgumentsList()))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleSleepCommand(const ArgumentsList& arguments)
{
    sendResponse("OK", ArgumentsList());
    return ACTION_SLEEP;
}


ServerListener::tAction InteractiveListener::handleResetCommand(const Mash::ArgumentsList& arguments)
{
    delete _pApplicationServer;

    _bDoingSetup = false;
    _bGlobalSeedSelected = false;

    _pApplicationServer = pConstructor();

    sendResponse("OK", ArgumentsList());

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleUseGlobalSeedCommand(const Mash::ArgumentsList& arguments)
{
    // Check the arguments
    if (arguments.size() != 1)
    {
        if (!sendResponse("INVALID_ARGUMENTS", arguments))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Check that the global seed wasn't already set
    if (_bGlobalSeedSelected)
    {
        if (!sendResponse("GLOBAL_SEED_ALREADY_SET", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the global seed
    unsigned int seed = (unsigned int) arguments.getInt(0);

    // Provides it to the application server implementation
    _pApplicationServer->setGlobalSeed(seed);

    _bGlobalSeedSelected = true;

    sendResponse("OK", ArgumentsList());

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleListGoalsCommand(const ArgumentsList& arguments)
{
    // Check that the global seed was set
    if (!_bGlobalSeedSelected)
        chooseGlobalSeed();

    tStringList goals = _pApplicationServer->getGoals();

    tStringIterator iter, iterEnd;
    for (iter = goals.begin(), iterEnd = goals.end(); iter != iterEnd; ++iter)
    {
        if (!sendResponse("GOAL", ArgumentsList(*iter)))
            return ACTION_CLOSE_CONNECTION;
    }

    if (!sendResponse("END_LIST_GOALS", ArgumentsList()))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleListEnvironmentsCommand(const ArgumentsList& arguments)
{
    // Check that the global seed was set
    if (!_bGlobalSeedSelected)
        chooseGlobalSeed();

    // Check the arguments
    if (arguments.size() != 1)
    {
        if (!sendResponse("INVALID_ARGUMENTS", arguments))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the name of the goal and check it
    string strGoal = arguments.getString(0);

    tStringList goals = _pApplicationServer->getGoals();
    tStringIterator iter, iterEnd;
    for (iter = goals.begin(), iterEnd = goals.end(); iter != iterEnd; ++iter)
    {
        if (strGoal == *iter)
            break;
    }

    if (iter == iterEnd)
    {
        if (!sendResponse("UNKNOWN_GOAL", strGoal))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    tStringList environments = _pApplicationServer->getEnvironments(strGoal);
    for (iter = environments.begin(), iterEnd = environments.end(); iter != iterEnd; ++iter)
    {
        if (!sendResponse("ENVIRONMENT", ArgumentsList(*iter)))
            return ACTION_CLOSE_CONNECTION;
    }

    if (!sendResponse("END_LIST_ENVIRONMENTS", ArgumentsList()))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleGetDatasetCommand(const ArgumentsList& arguments)
{
    // Check the arguments
    if (arguments.size() != 2)
    {
        if (!sendResponse("INVALID_ARGUMENTS", arguments))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the name of the goal and check it
    string strGoal = arguments.getString(0);

    tStringList goals = _pApplicationServer->getGoals();
    tStringIterator iter, iterEnd;
    for (iter = goals.begin(), iterEnd = goals.end(); iter != iterEnd; ++iter)
    {
        if (strGoal == *iter)
            break;
    }

    if (iter == iterEnd)
    {
        if (!sendResponse("UNKNOWN_GOAL", strGoal))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the name of the environment and check it
    string strEnvironment = arguments.getString(1);

    tStringList environments = _pApplicationServer->getEnvironments(strGoal);
    for (iter = environments.begin(), iterEnd = environments.end(); iter != iterEnd; ++iter)
    {
        if (strEnvironment == *iter)
            break;
    }

    if (iter == iterEnd)
    {
        if (!sendResponse("UNKNOWN_ENVIRONMENT", strEnvironment))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the dataset
    string strDataset = _pApplicationServer->getDataset(strGoal, strEnvironment);
    if (!strDataset.empty())
    {
        if (!sendResponse("DATASET", ArgumentsList(strDataset)))
            return ACTION_CLOSE_CONNECTION;
    }
    else
    {
        if (!sendResponse("NO_DATASET", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;
    }

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleInitializeTaskCommand(const ArgumentsList& arguments)
{
    // Check that the global seed was set
    if (!_bGlobalSeedSelected)
        chooseGlobalSeed();

    // Check the arguments
    if (arguments.size() != 2)
    {
        if (!sendResponse("INVALID_ARGUMENTS", arguments))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the name of the goal and check it
    string strGoal = arguments.getString(0);

    tStringList goals = _pApplicationServer->getGoals();
    tStringIterator iter, iterEnd;
    for (iter = goals.begin(), iterEnd = goals.end(); iter != iterEnd; ++iter)
    {
        if (strGoal == *iter)
            break;
    }

    if (iter == iterEnd)
    {
        if (!sendResponse("UNKNOWN_GOAL", strGoal))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the name of the environment and check it
    string strEnvironment = arguments.getString(1);

    tStringList environments = _pApplicationServer->getEnvironments(strGoal);
    for (iter = environments.begin(), iterEnd = environments.end(); iter != iterEnd; ++iter)
    {
        if (strEnvironment == *iter)
            break;
    }

    if (iter == iterEnd)
    {
        if (!sendResponse("UNKNOWN_ENVIRONMENT", strEnvironment))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Send the list of actions to the client
    ArgumentsList responseArguments;

    _actions = _pApplicationServer->getActions(strGoal, strEnvironment);
    for (iter = _actions.begin(), iterEnd = _actions.end(); iter != iterEnd; ++iter)
        responseArguments.add(*iter);

    if (!sendResponse("AVAILABLE_ACTIONS", responseArguments))
        return ACTION_CLOSE_CONNECTION;

    // Send the list of views to the client
    _views = _pApplicationServer->getViews(strGoal, strEnvironment);
    responseArguments.clear();

    tViewsIterator iter2, iterEnd2;
    for (iter2 = _views.begin(), iterEnd2 = _views.end(); iter2 != iterEnd2; ++iter2)
    {
        responseArguments.add(iter2->name + ":" + StringUtils::toString(iter2->width) +
                              "x" + StringUtils::toString(iter2->height));
    }

    if (!sendResponse("AVAILABLE_VIEWS", responseArguments))
        return ACTION_CLOSE_CONNECTION;

    // Send the mode to the client
    _capabilities = _pApplicationServer->capabilities(strGoal, strEnvironment);
    responseArguments.clear();

    if (_capabilities & IAS_CAP_INTERACTION)
        responseArguments.add("INTERACTION");

    if (_capabilities & IAS_CAP_TRAJECTORIES)
        responseArguments.add("TRAJECTORIES");

    if (_capabilities & IAS_CAP_SUGGESTED_ACTION)
        responseArguments.add("SUGGESTED_ACTION");

    if (_capabilities & IAS_CAP_NOT_RECOMMENDED_ACTIONS)
        responseArguments.add("NOT_RECOMMENDED_ACTIONS");

    if (!sendResponse("CAPABILITIES", responseArguments))
        return ACTION_CLOSE_CONNECTION;

    _strGoalName        = strGoal;
    _strEnvironmentName = strEnvironment;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleBeginTaskSetupCommand(const ArgumentsList& arguments)
{
    // Check that a task was selected
    if (_strGoalName.empty() || _strEnvironmentName.empty())
    {
        if (!sendResponse("NO_TASK_SELECTED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Check that we aren't already doing the setup of the task
    if (_bDoingSetup)
    {
        if (!sendResponse("ERROR", ArgumentsList("Task setup already started")))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Starts the setup
    _bDoingSetup = true;
    _taskParameters.clear();

    if (!sendResponse("OK", ArgumentsList()))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleEndTaskSetupCommand(const ArgumentsList& arguments)
{
    // Check that we are doing the setup of the task
    if (!_bDoingSetup)
    {
        if (!sendResponse("ERROR", ArgumentsList("Task setup not started")))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Ends the setup
    _bDoingSetup = false;

    // Setup the application server
    if (!_pApplicationServer->initializeTask(_strGoalName, _strEnvironmentName, _taskParameters))
    {
        if (!sendResponse("ERROR", ArgumentsList("Failed to initialize the task")))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    _taskParameters.clear();

    if (_capabilities & IAS_CAP_SUGGESTED_ACTION)
    {
        if (!sendResponse("SUGGESTED_ACTION", ArgumentsList(_pApplicationServer->getSuggestedAction())))
            return ACTION_CLOSE_CONNECTION;
    }

    if (_capabilities & IAS_CAP_NOT_RECOMMENDED_ACTIONS)
    {
        tStringList actions = _pApplicationServer->notRecommendedActions();
        ArgumentsList args;

        if (!actions.empty())
        {
            for (unsigned int i = 0; i < actions.size(); ++i)
                args.add(actions[i]);
        }
        else
        {
            args.add("-");
        }

        if (!sendResponse("NOT_RECOMMENDED_ACTIONS", args))
            return ACTION_CLOSE_CONNECTION;
    }

    if (!sendResponse("STATE_UPDATED", ArgumentsList()))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleResetTaskCommand(const ArgumentsList& arguments)
{
    // Check that a task was selected
    if (_strGoalName.empty() || _strEnvironmentName.empty())
    {
        if (!sendResponse("NO_TASK_SELECTED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Reset the application server
    _pApplicationServer->resetTask();

    if (_capabilities & IAS_CAP_SUGGESTED_ACTION)
    {
        if (!sendResponse("SUGGESTED_ACTION", ArgumentsList(_pApplicationServer->getSuggestedAction())))
            return ACTION_CLOSE_CONNECTION;
    }

    if (_capabilities & IAS_CAP_NOT_RECOMMENDED_ACTIONS)
    {
        tStringList actions = _pApplicationServer->notRecommendedActions();
        ArgumentsList args;

        if (!actions.empty())
        {
            for (unsigned int i = 0; i < actions.size(); ++i)
                args.add(actions[i]);
        }
        else
        {
            args.add("-");
        }

        if (!sendResponse("NOT_RECOMMENDED_ACTIONS", args))
            return ACTION_CLOSE_CONNECTION;
    }

    // Tell the client that the state of the task has been updated
    if (!sendResponse("STATE_UPDATED", ArgumentsList()))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleGetNbTrajectoriesCommand(const Mash::ArgumentsList& arguments)
{
    // Check that a task was selected
    if (_strGoalName.empty() || _strEnvironmentName.empty())
    {
        if (!sendResponse("NO_TASK_SELECTED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Send the number of trajectories
    if (!sendResponse("NB_TRAJECTORIES", ArgumentsList((int) _pApplicationServer->getNbTrajectories())))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleGetTrajectoryLengthCommand(const Mash::ArgumentsList& arguments)
{
    // Check the arguments
    if (arguments.size() != 1)
    {
        if (!sendResponse("INVALID_ARGUMENTS", arguments))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    int trajectory = arguments.getInt(0);
    if ((trajectory < 0) || (trajectory >= _pApplicationServer->getNbTrajectories()))
    {
        if (!sendResponse("UNKNOWN_TRAJECTORY", arguments))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Check that a task was selected
    if (_strGoalName.empty() || _strEnvironmentName.empty())
    {
        if (!sendResponse("NO_TASK_SELECTED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Send the length of the trajectory
    if (!sendResponse("TRAJECTORY_LENGTH", ArgumentsList((int) _pApplicationServer->getTrajectoryLength((unsigned) trajectory))))
        return ACTION_CLOSE_CONNECTION;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleGetViewCommand(const ArgumentsList& arguments)
{
    // Check the arguments
    if (arguments.size() != 1)
    {
        if (!sendResponse("INVALID_ARGUMENTS", arguments))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Check that a task was selected
    if (_strGoalName.empty() || _strEnvironmentName.empty())
    {
        if (!sendResponse("NO_TASK_SELECTED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the name of the view and check it
    string strView = arguments.getString(0);

    tViewsIterator iter, iterEnd;
    for (iter = _views.begin(), iterEnd = _views.end(); iter != iterEnd; ++iter)
    {
        if (iter->name == strView)
            break;
    }

    if (iter == iterEnd)
    {
        if (!sendResponse("UNKNOWN_VIEW", strView))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Send the view to the client
    size_t data_size = 0;
    string mime_type;
    bool bRaw = false;
    unsigned char* pImage = _pApplicationServer->getView(iter->name, data_size, mime_type);
    if (!pImage)
        return ACTION_CLOSE_CONNECTION;

    if ((mime_type == "raw") || (mime_type == "image/rgb"))
    {
        bRaw = true;
        mime_type = "image/mif";
        data_size = 8 + 3 * iter->width * iter->height;
    }

    ArgumentsList responseArgs;
    responseArgs.add(iter->name);
    responseArgs.add(mime_type);
    responseArgs.add((int) data_size);

    if (!sendResponse("VIEW", responseArgs))
    {
        delete[] pImage;
        return ACTION_CLOSE_CONNECTION;
    }

    if (bRaw)
    {
        unsigned char header[8];
        header[0] = 'M';
        header[1] = 'I';
        header[2] = 'F';
        header[3] = 1;
        header[4] = iter->width % 256;
        header[5] = iter->width / 256;
        header[6] = iter->height % 256;
        header[7] = iter->height / 256;

        if (!sendData((const unsigned char*) header, 8))
        {
            delete[] pImage;
            return ACTION_CLOSE_CONNECTION;
        }

        if (!sendData(pImage, data_size - 8))
        {
            delete[] pImage;
            return ACTION_CLOSE_CONNECTION;
        }
    }
    else
    {
        if (!sendData(pImage, data_size))
        {
            delete[] pImage;
            return ACTION_CLOSE_CONNECTION;
        }
    }

    delete[] pImage;

    return ACTION_NONE;
}


ServerListener::tAction InteractiveListener::handleActionCommand(const ArgumentsList& arguments)
{
    // Check the arguments
    if (arguments.size() != 1)
    {
        if (!sendResponse("INVALID_ARGUMENTS", arguments))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Check that a task was selected
    if (_strGoalName.empty() || _strEnvironmentName.empty())
    {
        if (!sendResponse("NO_TASK_SELECTED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Retrieve the name of the action and check it
    string strAction = arguments.getString(0);

    tStringIterator iter, iterEnd;
    for (iter = _actions.begin(), iterEnd = _actions.end(); iter != iterEnd; ++iter)
    {
        if (*iter == strAction)
            break;
    }

    if (iter == iterEnd)
    {
        if (!sendResponse("UNKNOWN_ACTION", strAction))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Perform the action
    float reward;
    bool bFinished;
    bool bFailed;
    string strEvent;

    if (!_pApplicationServer->performAction(strAction, reward, bFinished,
                                            bFailed, strEvent))
    {
        if (!sendResponse("ERROR", ArgumentsList("Failed to perform the action")))
            return ACTION_CLOSE_CONNECTION;

        return ACTION_NONE;
    }

    // Send the reward to the client
    if (!sendResponse("REWARD", ArgumentsList(reward)))
        return ACTION_CLOSE_CONNECTION;

    // Send the event to the client (if any)
    if (!strEvent.empty())
    {
        if (!sendResponse("EVENT", ArgumentsList(strEvent)))
            return ACTION_CLOSE_CONNECTION;
    }

    // Send the teacher action to the client
    if (_capabilities & IAS_CAP_SUGGESTED_ACTION)
    {
        ArgumentsList args;

        if (!_pApplicationServer->getSuggestedAction().empty())
            args.add(_pApplicationServer->getSuggestedAction());
        else
            args.add("-");

        if (!sendResponse("SUGGESTED_ACTION", args))
            return ACTION_CLOSE_CONNECTION;
    }

    // Send the not recommend actions to the client
    if (_capabilities & IAS_CAP_NOT_RECOMMENDED_ACTIONS)
    {
        tStringList actions = _pApplicationServer->notRecommendedActions();
        ArgumentsList args;

        if (!actions.empty())
        {
            for (unsigned int i = 0; i < actions.size(); ++i)
                args.add(actions[i]);
        }
        else
        {
            args.add("-");
        }

        if (!sendResponse("NOT_RECOMMENDED_ACTIONS", args))
            return ACTION_CLOSE_CONNECTION;
    }

    if (bFinished)
    {
        // Tell the client that the goal has been reached
        if (!sendResponse("FINISHED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;
    }
    else if (bFailed)
    {
        // Tell the client that the goal can't be reached anymore
        if (!sendResponse("FAILED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;
    }
    else
    {
        // Tell the client that the state of the world has been updated
        if (!sendResponse("STATE_UPDATED", ArgumentsList()))
            return ACTION_CLOSE_CONNECTION;
    }

    return ACTION_NONE;
}


void InteractiveListener::chooseGlobalSeed()
{
    if (!_bGlobalSeedSelected)
    {
        _pApplicationServer->setGlobalSeed(time(0));

        _bGlobalSeedSelected = true;
    }
}
