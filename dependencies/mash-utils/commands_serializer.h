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


/** @file   commands_serializer.h
    @author Philip Abbet (philip.abbet@idiap.ch)

    Declaration of the 'CommandsSerializer' class
*/

#ifndef _MASH_COMMANDSSERIALIZER_H_
#define _MASH_COMMANDSSERIALIZER_H_

#include <mash-utils/arguments_list.h>
#include <vector>


namespace Mash
{
    //--------------------------------------------------------------------------
    /// @brief  Used to deserialize a list of commands (and their arguments)
    ///         from a text file
    //--------------------------------------------------------------------------
    class MASH_SYMBOL CommandsSerializer
    {
        //_____ Public internal types __________
    public:
        struct tCommand
        {
            std::string     strCommand;
            ArgumentsList   arguments;
        };


        //_____ Construction / Destruction __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Constructor
        //----------------------------------------------------------------------
        CommandsSerializer();

        //----------------------------------------------------------------------
        /// @brief  Destructor
        //----------------------------------------------------------------------
        ~CommandsSerializer();


        //_____ Methods __________
    public:
        //----------------------------------------------------------------------
        /// @brief  Deserialize commands from a file
        ///
        /// @param  strFileName     Path to the file containing the commands
        /// @return                 'true' if successful
        //----------------------------------------------------------------------
        bool deserialize(const std::string& strFileName);

        //----------------------------------------------------------------------
        /// @brief  Returns one of the command
        ///
        /// @param  index   Index of the command
        /// @return         The command, empty in case of failure
        //----------------------------------------------------------------------
        tCommand getCommand(unsigned int index);

        //----------------------------------------------------------------------
        /// @brief  Returns the number of deserialized commands
        //----------------------------------------------------------------------
        inline unsigned int nbCommands() const
        {
            return _commands.size();
        }


        //_____ Internal types __________
    private:
        typedef std::vector<tCommand>   tCommandsList;
        typedef tCommandsList::iterator tCommandsIterator;


        //_____ Attributes __________
    private:
        tCommandsList _commands;
    };
}

#endif
