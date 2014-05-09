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


/** @file   SimulationServer.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'SimulationServer' class
*/

#ifndef _SIMULATION_SERVER_H_
#define _SIMULATION_SERVER_H_

#include <mash-appserver/application_server_interface.h>
#include <Simulator.h>


//------------------------------------------------------------------------------
/// @brief Concrete implementation of the Simulation Server
//------------------------------------------------------------------------------
class SimulationServer: public Mash::IApplicationServer
{
    //_____ Construction / Destruction __________
public:
    //--------------------------------------------------------------------------
    /// @brief Constructor
    //--------------------------------------------------------------------------
    SimulationServer();

    //--------------------------------------------------------------------------
    /// @brief Destructor
    //--------------------------------------------------------------------------
    virtual ~SimulationServer();


    //_____ Static methods __________
public:
    static IApplicationServer* create();


    //_____ Methods __________
public:
    //--------------------------------------------------------------------------
    /// @brief Sets the global seed
    //--------------------------------------------------------------------------
    virtual void setGlobalSeed(unsigned int seed);

    //--------------------------------------------------------------------------
    /// @brief Returns a list containing the name of the available goals
    //--------------------------------------------------------------------------
    virtual Mash::tStringList getGoals();

    //--------------------------------------------------------------------------
    /// @brief Returns a list containing the name of the environment
    ///        suitable for the given goal
    ///
    /// @param goal         The name of the goal
    //--------------------------------------------------------------------------
    virtual Mash::tStringList getEnvironments(const std::string& goal);

    //----------------------------------------------------------------------
    /// @brief Returns the path to the dataset corresponding to the given
    ///        goal and environment (if any)
    ///
    /// @param goal         The name of the goal
    /// @param environment  The name of the environment
    /// @return             The path, empty if not applicable
    //----------------------------------------------------------------------
    virtual std::string getDataset(const std::string& goal,
                                   const std::string& environment);

    //--------------------------------------------------------------------------
    /// @brief Returns a list containing the possible actions in a task
    ///
    /// @param goal         The name of the goal
    /// @param environment  The name of the environment
    //--------------------------------------------------------------------------
    virtual Mash::tStringList getActions(const std::string& goal,
                                         const std::string& environment);

    //--------------------------------------------------------------------------
    /// @brief Returns the capabilities of a task
    ///
    /// @param goal         The name of the goal
    /// @param environment  The name of the environment
    //--------------------------------------------------------------------------
    virtual Mash::tIASCapabilities capabilities(const std::string& goal,
                                                const std::string& environment);

    //--------------------------------------------------------------------------
    /// @brief Returns a list containing the possible actions in a task
    ///
    /// @param goal         The name of the goal
    /// @param environment  The name of the environment
    //--------------------------------------------------------------------------
    virtual Mash::tViewsList getViews(const std::string& goal,
                                      const std::string& environment);

    //--------------------------------------------------------------------------
    /// @brief Initialize a task
    ///
    /// @param goal         The name of the goal
    /// @param environment  The name of the environment
    /// @param settings     Task-specific settings
    ///
    /// @remark Might be called several times. Cleanup the previous task as
    ///         needed.
    //--------------------------------------------------------------------------
    virtual bool initializeTask(const std::string& goal,
                                const std::string& environment,
                                const tSettingsList& settings);

    //--------------------------------------------------------------------------
    /// @brief Reset the task in its initial state
    ///
    /// @return 'true' if successful
    //--------------------------------------------------------------------------
    virtual bool resetTask();

    //--------------------------------------------------------------------------
    /// @brief Returns the number of trajectories available
    ///
    /// Only available when the IAS_CAP_TRAJECTORIES capability flag is
    /// present
    //--------------------------------------------------------------------------
    virtual unsigned int getNbTrajectories()
    {
        return 0;
    }

    //--------------------------------------------------------------------------
    /// @brief Returns the number of actions in a trajectory
    ///
    /// @param trajectory   Index of the trajectory
    ///
    /// Only available when the IAS_CAP_TRAJECTORIES capability flag is
    /// present
    //--------------------------------------------------------------------------
    virtual unsigned int getTrajectoryLength(unsigned int trajectory)
    {
        return 0;
    }

    //--------------------------------------------------------------------------
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
    //--------------------------------------------------------------------------
    virtual unsigned char* getView(const std::string& view, size_t &nbBytes,
                                   std::string &mimetype);

    //--------------------------------------------------------------------------
    /// @brief Performs an action
    ///
    /// @param      action          The name of the action to perform
    /// @param[out] reward          The reward
    /// @param[out] finished        'true' if the goal was reached
    /// @param[out] failed          'true' if the goal isn't reacheable anymore
    /// @param[out] event           Human-readable description of what happened
    ///                             (optional)
    /// @return                     'false' in case of error
    //--------------------------------------------------------------------------
    virtual bool performAction(const std::string& action, float &reward,
                               bool &finished, bool &failed, std::string &event);


    //--------------------------------------------------------------------------
    /// @brief Returns the action that must be suggested to the client
    ///
    /// Only available when the IAS_CAP_SUGGESTED_ACTION capability flag is
    /// present
    ///
    /// @remark When the IAS_CAP_INTERACTION capability flag isn't present,
    ///         it's impossible to do any other action than the "suggested"
    ///         one
    //--------------------------------------------------------------------------
    virtual std::string getSuggestedAction();

    //----------------------------------------------------------------------
    /// @brief  Returns the list of not recommended actions
    ///
    /// Only available when the IAS_CAP_NOT_RECOMMENDED_ACTIONS capability
    /// flag is present
    //----------------------------------------------------------------------
    virtual Mash::tStringList notRecommendedActions();

    //--------------------------------------------------------------------------
    /// @brief  Called when a timeout occured while waiting for a command
    //--------------------------------------------------------------------------
    virtual void onTimeout();


    //_____ Attributes __________
protected:
    Simulator*      m_pSimulator;
    unsigned int    m_globalSeed;

public:
    static bool bEnableSecrets;
};

#endif
