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


/** @file   commands_serializer.cpp
    @author Philip Abbet (philip.abbet@idiap.ch)

    Implementation of the 'CommandsSerializer' class
*/

#include "commands_serializer.h"
#include "data_buffer.h"
#include "stringutils.h"
#include <fstream>
#include <assert.h>

using namespace std;
using namespace Mash;


/************************* CONSTRUCTION / DESTRUCTION *************************/

CommandsSerializer::CommandsSerializer()
{
}


CommandsSerializer::~CommandsSerializer()
{
}


/*********************************** METHODS **********************************/

bool CommandsSerializer::deserialize(const std::string& strFileName)
{
    // Assertions
    assert(!strFileName.empty());

    // Declarations
    ifstream file;
    string strLine;
    char buffer[256] = { 0 };

    _commands.clear();

    // Open the file
    file.open(strFileName.c_str(), ifstream::in);
    if (!file.good())
        return false;

    // Read the commands
    while (!file.eof())
    {
        // Read a full line
        file.getline(buffer, 255, '\n');
        strLine += buffer;
        if (file.fail())
            continue;

        if (StringUtils::endsWith(strLine, "\r"))
            strLine = strLine.substr(0, strLine.size() - 1);

        strLine = StringUtils::trim(strLine) + "\n";

        // Check that the line isn't a comment
        if (strLine.at(0) == '#')
        {
            strLine = "";
            continue;
        }

        // Check that the line isn't empty
        if (strLine == "\n")
        {
            strLine = "";
            continue;
        }

        tCommand command;
        DataBuffer buffer2(strLine);
        buffer2.extractMessage(&command.strCommand, &command.arguments);
        _commands.push_back(command);

        strLine = strLine.substr(strLine.size() - buffer2.size());
    }

    return true;
}


CommandsSerializer::tCommand CommandsSerializer::getCommand(unsigned int index)
{
    tCommand command;

    if (index < _commands.size())
        command = _commands[index];

    return command;
}
