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


/** @file   utils.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the utility functions
*/

#include "errors.h"
#include <assert.h>

using namespace std;

namespace Mash
{
    std::string getErrorDescription(tError error)
    {
        switch (error)
        {
            case ERROR_NONE:
                return "No error";

            case ERROR_FORK:
                return "Failed to fork";

            case ERROR_SANDBOX_CREATION:
                return "Failed to create a sandbox";

            case ERROR_SANDBOX_INVALID_CONFIGURATION:
                return "Invalid sandbox configuration";

            case ERROR_SANDBOX_MEMORY_LIMIT_REACHED:
                return "The sandboxed object tried to allocate too much memory";

            case ERROR_SANDBOX_FORBIDDEN_SYSTEM_CALL:
                return "The sandboxed object tried to call a forbidden system call";

            case ERROR_CHANNEL_UNEXPECTED_RESPONSE:
                return "Unexpected response received via the communication channel";

            case ERROR_CHANNEL_SLAVE_CRASHED:
                return "The slave endpoint of the communication channel crashed";

            case ERROR_CHANNEL_SLAVE_TIMEOUT:
                return "The slave endpoint of the communication channel didn't respond fast enough";

            case ERROR_CHANNEL_PROTOCOL:
                return "Protocol error on the communication channel";

            case ERROR_DYNLIB_LOADING:
                return "Failed to load the dynamic library";

            case ERROR_FILE_NOT_OPENED:
                return "Failed to open a file";

            case ERROR_HEURISTIC_CONSTRUCTOR:
                return "Failed to retrieve the construction function of the heuristic";

            case ERROR_HEURISTIC_CRASHED:
                return "A heuristic crashed";

            case ERROR_HEURISTIC_TIMEOUT:
                return "A heuristic took too much computation time";

            case ERROR_FEATURE_IS_NAN:
                return "A heuristic returned a feature equal to NaN";

            case ERROR_CLASSIFIER_LOADING:
                return "Failed to load the classifier";

            case ERROR_CLASSIFIER_CONSTRUCTOR:
                return "Failed to retrieve the construction function of the classifier";

            case ERROR_CLASSIFIER_SETUP_FAILED:
                return "The classifier failed to setup itself";

            case ERROR_CLASSIFIER_MODEL_LOADING_FAILED:
                return "The classifier failed to load a model";

            case ERROR_CLASSIFIER_MODEL_MISSING_HEURISTIC:
                return "Some heuristics referenced by the model are missing";

            case ERROR_CLASSIFIER_TRAINING_FAILED:
                return "The classifier failed to train itself";

            case ERROR_CLASSIFIER_CLASSIFICATION_FAILED:
                return "The classifier failed to classify an image";

            case ERROR_CLASSIFIER_REPORTING_FAILED:
                return "The classifier failed to report the list of features it use";

            case ERROR_CLASSIFIER_MODEL_SAVING_FAILED:
                return "The classifier failed to save its model";

            case ERROR_CLASSIFIER_CRASHED:
                return "The classifier crashed";

            case ERROR_PLANNER_LOADING:
                return "Failed to load the goal-planner";

            case ERROR_PLANNER_CONSTRUCTOR:
                return "Failed to retrieve the construction function of the goal-planner";

            case ERROR_PLANNER_SETUP_FAILED:
                return "The goal-planner failed to setup itself";

            case ERROR_PLANNER_MODEL_LOADING_FAILED:
                return "The goal-planner failed to load a model";

            case ERROR_PLANNER_MODEL_MISSING_HEURISTIC:
                return "Some heuristics referenced by the model are missing";

            case ERROR_PLANNER_LEARNING_FAILED:
                return "The goal-planner failed to learn how to solve the task";

            case ERROR_PLANNER_REPORTING_FAILED:
                return "The goal-planner failed to report the list of features it use";

            case ERROR_PLANNER_MODEL_SAVING_FAILED:
                return "The goal-planner failed to save its model";

            case ERROR_PLANNER_CRASHED:
                return "The goal-planner crashed";

            case ERROR_INSTRUMENT_LOADING:
                return "Failed to load the instrument";

            case ERROR_INSTRUMENT_CONSTRUCTOR:
                return "Failed to retrieve the construction function of the instrument";

            case ERROR_INSTRUMENT_SETUP_FAILED:
                return "The instrument failed to setup itself";

            case ERROR_INSTRUMENT_CRASHED:
                return "The instrument crashed";

            case ERROR_NETWORK_REQUEST_FAILURE:
                return "Failed to send a request";

            case ERROR_NETWORK_RESPONSE_FAILURE:
                return "Failed to wait a response";

            case ERROR_SERVER_INCORRECT_TYPE:
                return "Connected to an incorrect type of server";

            case ERROR_SERVER_BUSY:
                return "Connected to a busy server";

            case ERROR_APPSERVER_INCORRECT_SUBTYPE:
                return "Incorrect subtype served by the application server";

            case ERROR_APPSERVER_UNSUPPORTED_PROTOCOL:
                return "The application server uses an unsupported version of the protocol";

            case ERROR_APPSERVER_UNEXPECTED_RESPONSE:
                return "The application server sent an unexpected response";

            case ERROR_APPSERVER_ERROR:
                return "The application server returned an error";

            case ERROR_EXPERIMENT_PARAMETER:
                return "Problem while processing an experiment parameter";

            default:
                assert(false);
                return "Unknown error";
        }
    }


    std::string getErrorDetailedDescription(tError error)
    {
        string strDetails = getErrorDescription(error);

        switch (error)
        {
            case ERROR_DYNLIB_LOADING:
            case ERROR_CLASSIFIER_LOADING:
                strDetails += "\n\nCheck that the dynamic library exists.";
                break;

            case ERROR_HEURISTIC_CONSTRUCTOR:
                strDetails += "\n\nCheck that a 'new_heuristic' function is implemented.";
                break;

            case ERROR_CLASSIFIER_CONSTRUCTOR:
                strDetails += "\n\nCheck that a 'new_classifier' function is implemented.";
                break;

            default:
                break;
        }

        return strDetails;
    }
}
