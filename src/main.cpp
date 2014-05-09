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


#include <Simulator.h>
#include <SimulationServer.h>
#include <Declarations.h>
#include <mash-appserver/interactive_application_server.h>
#include <mash-utils/stringutils.h>
#include <Ogre/OgreException.h>
#include <SimpleOpt.h>
#include <stdlib.h>
#include <sys/stat.h>

using namespace Mash;
using namespace Athena;
using namespace std;


/**************************** COMMAND-LINE PARSING ****************************/

enum tOptions
{
    // General
    OPT_LIST,
    OPT_GAME,
    OPT_VIEW_SIZE,
    OPT_VERBOSE,
    OPT_HELP,

    // Game mode
    OPT_GOAL,
    OPT_ENVIRONMENT,
    OPT_SECRET,

    // Server mode
    OPT_HOST,
    OPT_PORT,
    OPT_LOG_FOLDER,
    OPT_NB_MAX_CLIENTS,

#if ATHENA_PLATFORM == ATHENA_PLATFORM_LINUX
    OPT_XAUTHORITY,
    OPT_DISPLAY,
#endif
};

CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] =
{
    // General
    { OPT_LIST,             "--list",        SO_NONE    },
    { OPT_GAME,             "--game",        SO_NONE    },
    { OPT_VIEW_SIZE,        "--viewsize",    SO_REQ_CMB },
    { OPT_VERBOSE,          "--verbose",     SO_NONE    },
    { OPT_HELP,             "--help",        SO_NONE    },
    { OPT_HELP,             "-h",            SO_NONE    },

    // Game mode
    { OPT_GOAL,             "--goal",        SO_REQ_CMB },
    { OPT_ENVIRONMENT,      "--environment", SO_REQ_CMB },
    { OPT_SECRET,           "--secret",      SO_NONE },

    // Server mode
    { OPT_HOST,             "--host",        SO_REQ_CMB },
    { OPT_PORT,             "--port",        SO_REQ_CMB },
    { OPT_LOG_FOLDER,       "--logfolder",   SO_REQ_CMB },
    { OPT_NB_MAX_CLIENTS,   "--maxclients",  SO_REQ_CMB },

#if ATHENA_PLATFORM == ATHENA_PLATFORM_LINUX
    { OPT_XAUTHORITY,       "--xauthority",  SO_REQ_CMB },
    { OPT_DISPLAY,          "--display",     SO_REQ_CMB },
#endif

    SO_END_OF_OPTIONS
};


/********************************** FUNCTIONS *********************************/

void showUsage(const std::string& strApplicationName)
{
    cout << "MASH 3D Simulator" << endl
         << "Usage: " << strApplicationName << " [options]" << endl
         << endl
         << "Options:" << endl
         << "    --help, -h:                   Display this help" << endl
         << "    --list:                       List the available goals and environments." << endl
         << "    --game:                       Don't start a server, act like a game (display, keyboard, ...)." << endl
         << "    --viewsize=<width>x<height>:  Size of the window/images" << endl
         << "    --verbose:                    Verbose output" << endl
         << endl
         << "Game mode options:" << endl
         << "    --goal=<goal>:                Name of the goal." << endl
         << "    --environment=<environment>:  Name of the environment." << endl
         << endl
         << "Server mode options:" << endl
         << "    --host=<host>:                The host name or IP address that the server must listen on." << endl
         << "                                  If not specified, the first available is used." << endl
         << "    --port=<port>:                The port that the server must listen on (default: 11200)" << endl
         << "    --logfolder=<path>:           Path to the location of the log files (default: 'logs/')" << endl
         << "    --maxclients=<nb>:            Maximum number of clients allowed (default: 1)" << endl

#if ATHENA_PLATFORM == ATHENA_PLATFORM_LINUX
         << "    --xauthorithy=<path>:         Path to the xauthority file (default: none)" << endl
         << "    --display=<display>:          Name of the display to use (default: 'none')" << endl
#endif

         << endl;
}


int main(int argc, char** argv)
{
    // Declarations
    bool            bList           = false;
    bool            bGame           = false;
    bool            bVerbose        = false;
    bool            bSecret         = false;
    string          strGoal         = "";
    string          strEnvironment  = "";
    string          strHost         = "";
    unsigned int    port            = 11200;
    unsigned int    nbMaxClients    = 1;
    unsigned int    width           = VIEW_WIDTH;
    unsigned int    height          = VIEW_HEIGHT;

    // Parse the command-line arguments
    CSimpleOpt args(argc, argv, COMMAND_LINE_OPTIONS);
    while (args.Next())
    {
        if (args.LastError() == SO_SUCCESS)
        {
            switch (args.OptionId())
            {
                case OPT_HELP:
                    showUsage(argv[0]);
                    return 0;

                case OPT_LIST:
                    bList = true;
                    break;

                case OPT_GAME:
                    bGame = true;
                    break;

                case OPT_SECRET:
                    bSecret = true;
                    break;

                case OPT_VIEW_SIZE:
                {
                    tStringList parts = StringUtils::split(args.OptionArg(), "x");
                    if ((parts.size() == 2) && !parts[0].empty() && !parts[1].empty())
                    {
                        width  = StringUtils::parseUnsignedInt(parts[0]);
                        height = StringUtils::parseUnsignedInt(parts[1]);
                    }
                    break;
                }

                case OPT_GOAL:
                    strGoal = args.OptionArg();
                    break;

                case OPT_ENVIRONMENT:
                    strEnvironment = args.OptionArg();
                    break;

                case OPT_HOST:
                    strHost = args.OptionArg();
                    break;

                case OPT_PORT:
                    port = StringUtils::parseUnsignedInt(args.OptionArg());
                    break;

                case OPT_LOG_FOLDER:
                    Server::strLogFolder = args.OptionArg();
                    if (Server::strLogFolder[Server::strLogFolder.size() - 1] != '/')
                        Server::strLogFolder += "/";
                    break;

                case OPT_NB_MAX_CLIENTS:
                    nbMaxClients = StringUtils::parseUnsignedInt(args.OptionArg());
                    break;

#if ATHENA_PLATFORM == ATHENA_PLATFORM_LINUX
                case OPT_XAUTHORITY:
                    setenv("XAUTHORITY", args.OptionArg(), 1);
                    break;

                case OPT_DISPLAY:
                    setenv("DISPLAY", args.OptionArg(), 1);
                    break;
#endif

                case OPT_VERBOSE:
                    bVerbose = true;
                    break;
            }
        }
        else
        {
            cerr << "Invalid argument: " << args.OptionText() << endl;
            return -1;
        }
    }


    setResolution(width, height);


    if (bList)
    {
        Simulator simulator;

        tStringList goals = simulator.getGoals();

        cout << "********************************************************************************" << endl
             << "* MASH 3D Simulator" << endl
             << "********************************************************************************" << endl
             << endl
             << "Available goals and environments:" << endl
             << endl;

        tStringIterator iter, iterEnd;
        for (iter = goals.begin(), iterEnd = goals.end(); iter != iterEnd; ++iter)
        {
            if (simulator.isGoalSecret(*iter))
                cout << "+ " << *iter << "*" << endl;
            else
                cout << "+ " << *iter << endl;

            tStringList environments = simulator.getEnvironments(*iter);

            tStringIterator iter2, iterEnd2;
            for (iter2 = environments.begin(), iterEnd2 = environments.end(); iter2 != iterEnd2; ++iter2)
            {
                if (simulator.isGoalSecret(*iter))
                    cout << "  - " << *iter2 << "*" << endl;
                else
                    cout << "  - " << *iter2 << endl;
            }

            cout << endl;
        }

        cout << "*: not available in game mode" << endl << endl;

        return 0;
    }
    else if (bGame)
    {
        Simulator simulator;

        if (strGoal.empty() || strEnvironment.empty())
        {
            cout << "You must specify a goal and an environment (using --goal and --environment)" << endl;
            return -1;
        }

        if (!bSecret && simulator.isGoalSecret(strGoal))
        {
            cout << "Can't use a secret goal in game mode" << endl;
            return -1;
        }

        tStringList environments = simulator.getEnvironments(strGoal);
        if (environments.empty())
        {
            cout << "Unknown goal: " << strGoal << endl;
            return -1;
        }

        tStringIterator iter, iterEnd;
        bool bFound = false;
        for (iter = environments.begin(), iterEnd = environments.end(); iter != iterEnd; ++iter)
        {
            if (*iter == strEnvironment)
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            cout << "Environment not suitable for the selected goal: " << strEnvironment << endl;
            return -1;
        }

        if (!bSecret && simulator.isEnvironmentSecret(strEnvironment))
        {
            cout << "Can't use a secret environment in game mode" << endl;
            return -1;
        }

        // Initialisation of the simulator
        if (!simulator.init(bGame, strGoal, strEnvironment, bSecret))
            return -1;

        // Start the simulator like a game
        return (simulator.run() ? 0 : -1);
    }
    else
    {
        InteractiveApplicationServer server(nbMaxClients);

        cout << "********************************************************************************" << endl
             << "* MASH 3D Simulator" << endl
             << "* Protocol: " << server.getProtocol() << endl
             << "********************************************************************************" << endl
             << endl;

        SimulationServer::bEnableSecrets = bSecret;

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        // Start the server
        return (server.listen(strHost, port, SimulationServer::create, bVerbose, &timeout) ? 0 : -1);
    }
}
