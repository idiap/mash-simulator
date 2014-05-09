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


/** @file   interactive_listener.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'Listener' class
*/

#ifndef _MASH_INTERACTIVE_LISTENER_H_
#define _MASH_INTERACTIVE_LISTENER_H_

#include "application_server_interface.h"
#include <mash-network/server_listener.h>
#include <mash-utils/declarations.h>
#include <map>


namespace Mash
{
    typedef std::map<std::string, Mash::ArgumentsList>  tTaskParametersList;
    typedef tTaskParametersList::const_iterator         tTaskParametersIterator;

    //--------------------------------------------------------------------------
    /// @ brief Server listener dedicated to an 'Interactive Application Server'
    //--------------------------------------------------------------------------
    class InteractiveListener: public Mash::ServerListener
    {
        //_____ Construction / Destruction __________
    public:
        InteractiveListener(int socket);
        virtual ~InteractiveListener();


        //_____ Implementation of ServerListener __________
    public:
        virtual tAction handleCommand(const std::string& strCommand,
                                      const Mash::ArgumentsList& arguments);

        virtual void onTimeout();


        //_____ Static methods __________
    public:
        static void initialize(bool bVerbose, tApplicationServerConstructor* applicationServerConstructor,
                               struct timeval* pTimeout);
        static ServerListener* createListener(int socket);
        static std::string getProtocol();


        //_____ Methods __________
    private:
        tAction handleStatusCommand(const Mash::ArgumentsList& arguments);
        tAction handleInfoCommand(const Mash::ArgumentsList& arguments);
        tAction handleDoneCommand(const Mash::ArgumentsList& arguments);
        tAction handleLogsCommand(const Mash::ArgumentsList& arguments);
        tAction handleSleepCommand(const Mash::ArgumentsList& arguments);
        tAction handleResetCommand(const Mash::ArgumentsList& arguments);
        tAction handleUseGlobalSeedCommand(const Mash::ArgumentsList& arguments);
        tAction handleListGoalsCommand(const Mash::ArgumentsList& arguments);
        tAction handleListEnvironmentsCommand(const Mash::ArgumentsList& arguments);
        tAction handleGetDatasetCommand(const Mash::ArgumentsList& arguments);
        tAction handleInitializeTaskCommand(const Mash::ArgumentsList& arguments);
        tAction handleBeginTaskSetupCommand(const Mash::ArgumentsList& arguments);
        tAction handleEndTaskSetupCommand(const Mash::ArgumentsList& arguments);
        tAction handleResetTaskCommand(const Mash::ArgumentsList& arguments);
        tAction handleGetNbTrajectoriesCommand(const Mash::ArgumentsList& arguments);
        tAction handleGetTrajectoryLengthCommand(const Mash::ArgumentsList& arguments);
        tAction handleGetViewCommand(const Mash::ArgumentsList& arguments);
        tAction handleActionCommand(const Mash::ArgumentsList& arguments);

        void chooseGlobalSeed();


        //_____ Internal types __________
    private:
        typedef tAction (InteractiveListener::*tCommandHandler)(const Mash::ArgumentsList&);

        typedef std::map<std::string, tCommandHandler>  tCommandHandlersList;
        typedef tCommandHandlersList::iterator          tCommandHandlersIterator;


        //_____ Attributes __________
    private:
        static tCommandHandlersList             handlers;
        static bool                             bVerbose;
        static tApplicationServerConstructor*   pConstructor;
        static struct timeval                   timeout;

        IApplicationServer*             _pApplicationServer;
        bool                            _bGlobalSeedSelected;
        std::string                     _strGoalName;
        std::string                     _strEnvironmentName;
        bool                            _bDoingSetup;
        tTaskParametersList             _taskParameters;
        tStringList                     _actions;
        tViewsList                      _views;
        tIASCapabilities                _capabilities;
    };


    //--------------------------------------------------------------------------
    /// @brief  Construction function for the actual implementation of the
    ///         Application Server
    //--------------------------------------------------------------------------
    typedef IApplicationServer* tApplicationServerConstructor();
}

#endif
