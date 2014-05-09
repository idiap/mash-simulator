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


/** @file   declarations.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declarations of some types common to all the MASH libraries
*/

#ifndef _MASH_UTILS_DECLARATIONS_H_
#define _MASH_UTILS_DECLARATIONS_H_

#include "platform.h"

#ifndef MASH_SDK
    #include "arguments_list.h"
#endif

#include <map>
#include <string>
#include <vector>


//------------------------------------------------------------------------------
/// @brief  Contains all the types of MASH
//------------------------------------------------------------------------------
namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Represents the dimensions of an image
    //--------------------------------------------------------------------------
    struct dim_t
    {
        unsigned int width;
        unsigned int height;
    };


#ifndef MASH_SDK

    //--------------------------------------------------------------------------
    /// @brief  Represents a list of parameters for an experiment
    ///
    /// Let's assume the following parameters:
    ///     PARAM1 10
    ///     PARAM2 4.5 12.9
    ///     PARAM3 mash
    ///     PARAM4 "hello world"
    ///
    /// Here's how you'll process this list:
    /// <code>
    ///     tExperimentParametersIterator iter;
    ///
    ///     iter = parameters.find("PARAM1");
    ///     if (iter != parameters.end())
    ///     {
    ///         int value = iter->second.getInt(0);
    ///         ... do something with 'value' ...
    ///     }
    ///
    ///     iter = parameters.find("PARAM2");
    ///     if (iter != parameters.end())
    ///     {
    ///         float value1 = iter->second.getFloat(0);
    ///         float value2 = iter->second.getFloat(1);
    ///         ... do something with 'value1' and 'value2' ...
    ///     }
    ///
    ///     iter = parameters.find("PARAM3");
    ///     if (iter != parameters.end())
    ///     {
    ///         std::string value = iter->second.getString(0);
    ///         ... do something with 'value' ...
    ///     }
    ///
    ///     iter = parameters.find("PARAM4");
    ///     if (iter != parameters.end())
    ///     {
    ///         std::string value = iter->second.getString(0);
    ///         ... do something with 'value' ...
    ///     }
    /// </code>
    //--------------------------------------------------------------------------
    typedef std::map<std::string, ArgumentsList>        tExperimentParametersList;
    typedef tExperimentParametersList::const_iterator   tExperimentParametersIterator;


    //--------------------------------------------------------------------------
    /// @brief  Used to report the features used by a predictor
    ///
    /// See tFeaturesList below for an example
    //--------------------------------------------------------------------------
    struct tFeature
    {
        tFeature(unsigned int the_heuristic, unsigned int the_feature_index)
        : heuristic(the_heuristic), feature_index(the_feature_index)
        {
        }

        unsigned int heuristic;
        unsigned int feature_index;
    };


    //--------------------------------------------------------------------------
    /// @brief  Contains the list of the features used by the predictor
    ///
    /// Usage: list.push_back(tFeature(<heuristic_index>, <feature_index>));
    ///
    /// Example (let's say we use the feature 2 and 34 of heuristic 4 and
    /// the feature 12 of heuristic 7):
    /// @code
    ///    list.push_back(tFeature(4, 2));
    ///    list.push_back(tFeature(4, 34));
    ///    list.push_back(tFeature(7, 12));
    /// @endcode
    //--------------------------------------------------------------------------
    typedef std::vector<tFeature> tFeatureList;


    //--------------------------------------------------------------------------
    /// @brief  The supported type of plugins
    //--------------------------------------------------------------------------
    enum tPluginType
    {
        PLUGIN_HEURISTIC,
        PLUGIN_CLASSIFIER,
        PLUGIN_GOALPLANNER,
        PLUGIN_INSTRUMENT,
    };


    //--------------------------------------------------------------------------
    /// @brief  Capabilities of an Interactive Application Server
    ///
    /// Those flags can be OR'ed thogeter to describe the capabilities of the
    /// Server (usually this is task-dependent)
    //--------------------------------------------------------------------------
    enum tIASCapability
    {
        IAS_CAP_INTERACTION             = 1,    ///< Full interaction (can perform any action)
        IAS_CAP_TRAJECTORIES            = 2,    ///< The Server provides prerecorded trajectories
        IAS_CAP_SUGGESTED_ACTION        = 4,    ///< A teacher is available, and provides one "suggested action" per frame
        IAS_CAP_NOT_RECOMMENDED_ACTIONS = 8,    ///< A teacher is available, and provides a list of "not recommended actions" per frame
    };

    //--------------------------------------------------------------------------
    /// @brief  Represents the capabilities of a specific task of an Interactive
    /// Application Server
    ///
    /// This is an OR'ed combination of tIASCapability flags
    //--------------------------------------------------------------------------
    typedef unsigned char tIASCapabilities;

    //--------------------------------------------------------------------------
    /// @brief  Represents a list of goal-planning actions
    //--------------------------------------------------------------------------
    typedef std::vector<unsigned int> tActionsList;

    //--------------------------------------------------------------------------
    /// @brief Contains informations about a view
    //--------------------------------------------------------------------------
    struct tView
    {
        std::string     name;
        unsigned int    width;
        unsigned int    height;
    };

    //--------------------------------------------------------------------------
    /// @brief A list of informations about some views
    //--------------------------------------------------------------------------
    typedef std::vector<tView>      tViewsList;
    typedef tViewsList::iterator    tViewsIterator;

#endif

    //--------------------------------------------------------------------------
    /// @brief  Represents a list of strings
    //--------------------------------------------------------------------------
    typedef std::vector<std::string>    tStringList;
    typedef tStringList::iterator       tStringIterator;
    typedef tStringList::const_iterator tStringConstIterator;


    //--------------------------------------------------------------------------
    /// @brief  The error codes
    //--------------------------------------------------------------------------
    enum tError
    {
        /// No error
        ERROR_NONE,

        /// Failed to fork
        ERROR_FORK,

        /// Failed to create a sandbox
        ERROR_SANDBOX_CREATION,

        /// Invalid sandbox configuration
        ERROR_SANDBOX_INVALID_CONFIGURATION,

        /// The sandboxed object tried to allocate too much memory
        ERROR_SANDBOX_MEMORY_LIMIT_REACHED,

        /// The sandboxed object tried to call a forbidden system call
        ERROR_SANDBOX_FORBIDDEN_SYSTEM_CALL,

        /// The warden failed to monitor an operation
        ERROR_WARDEN,

        /// Unexpected response received via a communication channel
        ERROR_CHANNEL_UNEXPECTED_RESPONSE,

        /// The slave endpoint of a communication channel crashed
        ERROR_CHANNEL_SLAVE_CRASHED,

        /// The slave endpoint of a communication channel didn't respond fast enough
        ERROR_CHANNEL_SLAVE_TIMEOUT,

        /// Protocol error on a communication channel
        ERROR_CHANNEL_PROTOCOL,

        /// Failed to load a dynamic library
        ERROR_DYNLIB_LOADING,

        /// Failed to open a file
        ERROR_FILE_NOT_OPENED,

        /// Failed to retrieve the construction function of the heuristic
        ERROR_HEURISTIC_CONSTRUCTOR,

        /// A heuristic crashed
        ERROR_HEURISTIC_CRASHED,

        /// A heuristic took too much computation time
        ERROR_HEURISTIC_TIMEOUT,

        /// A heuristic returned a feature equal to NaN
        ERROR_FEATURE_IS_NAN,

        /// Failed to load a classifier from a dynamic library
        ERROR_CLASSIFIER_LOADING,

        /// Failed to retrieve the construction function of the classifier
        ERROR_CLASSIFIER_CONSTRUCTOR,

        /// The classifier failed to setup itself
        ERROR_CLASSIFIER_SETUP_FAILED,

        /// The classifier failed to load a model
        ERROR_CLASSIFIER_MODEL_LOADING_FAILED,

        /// Some heuristics referenced by the model are missing
        ERROR_CLASSIFIER_MODEL_MISSING_HEURISTIC,

        /// The classifier failed to train itself
        ERROR_CLASSIFIER_TRAINING_FAILED,

        /// The classifier failed to classify an image
        ERROR_CLASSIFIER_CLASSIFICATION_FAILED,

        /// The classifier failed to report the list of features it use
        ERROR_CLASSIFIER_REPORTING_FAILED,

        /// The classifier failed to save its model
        ERROR_CLASSIFIER_MODEL_SAVING_FAILED,

        /// The classifier crashed
        ERROR_CLASSIFIER_CRASHED,

        /// Failed to load a goal-planner from a dynamic library
        ERROR_PLANNER_LOADING,

        /// Failed to retrieve the construction function of the goal-planner
        ERROR_PLANNER_CONSTRUCTOR,

        /// The goal-planner failed to setup itself
        ERROR_PLANNER_SETUP_FAILED,

        /// The goal-planner failed to load a model
        ERROR_PLANNER_MODEL_LOADING_FAILED,

        /// Some heuristics referenced by the model are missing
        ERROR_PLANNER_MODEL_MISSING_HEURISTIC,

        /// The goal-planner failed to learn how to solve the task
        ERROR_PLANNER_LEARNING_FAILED,

        /// The goal-planner failed to report the list of features it use
        ERROR_PLANNER_REPORTING_FAILED,

        /// The goal-planner failed to save its model
        ERROR_PLANNER_MODEL_SAVING_FAILED,

        /// The goal-planner crashed
        ERROR_PLANNER_CRASHED,

        /// Failed to load an instrument from a dynamic library
        ERROR_INSTRUMENT_LOADING,

        /// Failed to retrieve the construction function of the instrument
        ERROR_INSTRUMENT_CONSTRUCTOR,

        /// The instrument failed to setup itself
        ERROR_INSTRUMENT_SETUP_FAILED,

        /// The instrument crashed
        ERROR_INSTRUMENT_CRASHED,

        /// Failed to send a request
        ERROR_NETWORK_REQUEST_FAILURE,

        /// Failed to wait a response
        ERROR_NETWORK_RESPONSE_FAILURE,

        /// Connected to an incorrect type of server
        ERROR_SERVER_INCORRECT_TYPE,

        /// Connected to a busy server
        ERROR_SERVER_BUSY,

        /// Incorrect subtype served by the application server
        ERROR_APPSERVER_INCORRECT_SUBTYPE,

        /// The application server uses an unsupported version of the protocol
        ERROR_APPSERVER_UNSUPPORTED_PROTOCOL,

        /// The application server sent an unexpected response
        ERROR_APPSERVER_UNEXPECTED_RESPONSE,

        /// The application server returned an error
        ERROR_APPSERVER_ERROR,

        /// Problem while processing an experiment parameter
        ERROR_EXPERIMENT_PARAMETER,
    };
}

#endif
