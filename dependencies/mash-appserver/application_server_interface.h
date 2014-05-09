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


/** @file   application_server_interface.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'IApplicationServer' interface
*/

#ifndef _MASH_APPLICATION_SERVER_INTERFACE_H_
#define _MASH_APPLICATION_SERVER_INTERFACE_H_

#include <mash-utils/arguments_list.h>
#include <mash-utils/declarations.h>
#include <vector>
#include <map>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief Interface that must be implemented by the 'Interactive
    ///        Application Servers'
    //--------------------------------------------------------------------------
    class IApplicationServer
    {
        //_____ Internal types __________
    public:
        //----------------------------------------------------------------------
        /// @brief A list of settings
        //----------------------------------------------------------------------
        typedef std::map<std::string, ArgumentsList>    tSettingsList;
        typedef tSettingsList::const_iterator           tSettingsIterator;


        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief Constructor
        //----------------------------------------------------------------------
        IApplicationServer()
        {
        }

        //----------------------------------------------------------------------
        /// @brief Destructor
        //----------------------------------------------------------------------
        virtual ~IApplicationServer()
        {
        }


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief Sets the global seed
        //----------------------------------------------------------------------
        virtual void setGlobalSeed(unsigned int seed) = 0;

        //----------------------------------------------------------------------
        /// @brief Returns a list containing the name of the available goals
        //----------------------------------------------------------------------
        virtual tStringList getGoals() = 0;

        //----------------------------------------------------------------------
        /// @brief Returns a list containing the name of the environment
        ///        suitable for the given goal
        ///
        /// @param goal         The name of the goal
        //----------------------------------------------------------------------
        virtual tStringList getEnvironments(const std::string& goal) = 0;

        //----------------------------------------------------------------------
        /// @brief Returns the path to the dataset corresponding to the given
        ///        goal and environment (if any)
        ///
        /// @param goal         The name of the goal
        /// @param environment  The name of the environment
        /// @return             The path, empty if not applicable
        //----------------------------------------------------------------------
        virtual std::string getDataset(const std::string& goal,
                                       const std::string& environment) = 0;

        //----------------------------------------------------------------------
        /// @brief Returns a list containing the possible actions in a task
        ///
        /// @param goal         The name of the goal
        /// @param environment  The name of the environment
        //----------------------------------------------------------------------
        virtual tStringList getActions(const std::string& goal,
                                       const std::string& environment) = 0;

        //----------------------------------------------------------------------
        /// @brief Returns a list containing the available views in a task
        ///
        /// @param goal         The name of the goal
        /// @param environment  The name of the environment
        //----------------------------------------------------------------------
        virtual tViewsList getViews(const std::string& goal,
                                    const std::string& environment) = 0;

        //----------------------------------------------------------------------
        /// @brief Returns the capabilities of a task
        ///
        /// @param goal         The name of the goal
        /// @param environment  The name of the environment
        //----------------------------------------------------------------------
        virtual tIASCapabilities capabilities(const std::string& goal,
                                              const std::string& environment) = 0;

        //----------------------------------------------------------------------
        /// @brief Initialize a task
        ///
        /// @param goal         The name of the goal
        /// @param environment  The name of the environment
        /// @param settings     Task-specific settings
        ///
        /// @remark Might be called several times. Cleanup the previous task as
        ///         needed.
        //----------------------------------------------------------------------
        virtual bool initializeTask(const std::string& goal,
                                    const std::string& environment,
                                    const tSettingsList& settings) = 0;

        //----------------------------------------------------------------------
        /// @brief Reset the task in its initial state
        ///
        /// @return 'true' if successful
        //----------------------------------------------------------------------
        virtual bool resetTask() = 0;

        //----------------------------------------------------------------------
        /// @brief Returns the number of trajectories available
        ///
        /// Only available when the IAS_CAP_TRAJECTORIES capability flag is
        /// present
        //----------------------------------------------------------------------
        virtual unsigned int getNbTrajectories() = 0;

        //----------------------------------------------------------------------
        /// @brief Returns the number of actions in a trajectory
        ///
        /// @param trajectory   Index of the trajectory
        ///
        /// Only available when the IAS_CAP_TRAJECTORIES capability flag is
        /// present
        //----------------------------------------------------------------------
        virtual unsigned int getTrajectoryLength(unsigned int trajectory) = 0;

        //----------------------------------------------------------------------
        /// @brief Returns one of the views
        ///
        /// @param[in]  view        The name of the view
        /// @param[out] nbBytes     The size of the returned data buffer, in
        ///                         bytes
        /// @param[out] mimetype    The MIME type of the data buffer (if 'raw',
        ///                         assume a RGB buffer with the dimensions of
        ///                         the view)
        /// @return                 Pointer to the data buffer, 0 in case of
        ///                         error (must be released by the caller)
        //----------------------------------------------------------------------
        virtual unsigned char* getView(const std::string& view, size_t &nbBytes,
                                       std::string &mimetype) = 0;

        //----------------------------------------------------------------------
        /// @brief Performs an action
        ///
        /// @param      action          The name of the action to perform
        /// @param[out] reward          The reward
        /// @param[out] finished        'true' if the goal was reached
        /// @param[out] failed          'true' if the goal isn't reacheable anymore
        /// @param[out] event           Human-readable description of what happened
        ///                             (optional)
        /// @return                     'false' in case of error
        //----------------------------------------------------------------------
        virtual bool performAction(const std::string& action, float &reward,
                                   bool &finished, bool &failed,
                                   std::string &event) = 0;

        //----------------------------------------------------------------------
        /// @brief Returns the action that must be suggested to the client
        ///
        /// Only available when the IAS_CAP_SUGGESTED_ACTION capability flag is
        /// present
        ///
        /// @remark When the IAS_CAP_INTERACTION capability flag isn't present,
        ///         it's impossible to do any other action than the "suggested"
        ///         one
        //----------------------------------------------------------------------
        virtual std::string getSuggestedAction() = 0;

        //----------------------------------------------------------------------
        /// @brief  Returns the list of not recommended actions
        ///
        /// Only available when the IAS_CAP_NOT_RECOMMENDED_ACTIONS capability
        /// flag is present
        //----------------------------------------------------------------------
        virtual tStringList notRecommendedActions() = 0;

        //----------------------------------------------------------------------
        /// @brief  Called when a timeout occured while waiting for a command
        //----------------------------------------------------------------------
        virtual void onTimeout() {}
    };


    //--------------------------------------------------------------------------
    /// @brief  Construction function for the actual implementation of the
    ///         Application Server
    //--------------------------------------------------------------------------
    typedef IApplicationServer* tApplicationServerConstructor();
}

#endif
