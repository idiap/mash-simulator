# mash-simulator

*Version 1.0.0*

http://github.com/idiap/mash-simulator

---------------------------------------

*mash-simulator* is a 3D simulator for **Linux** and **MacOS** where a robot must complete
a certain number of **tasks** in different **randomized environments**.

It was developed during the **MASH project** (http://www.mash-project.eu) at the
**Idiap Research Institute** (http://www.idiap.ch).

Its purpose is to let **machine learning algorithms** explore the 3D environments and
learn to solve the tasks by themselves.


## Usage

(Note: see below for instructions about downloading the required dependencies and compiling
the code).

*mash-simulator* is designed for two different scenarios:

 * the robot is controlled by a human, using its keyboard (**FPS mode**)
 * the robot is controlled by an algorithm, that communicates with the simulator through
   a TCP socket (**Server mode**)

**On MacOS**: The simulator is built as a bundle application. To launch it from the
command-line, you must type ```./simulator.app/Contents/MacOS/simulator``` instead
of ```./simulator```.

For a list of all available options, do:

    build$ cd bin
    bin$ ./simulator --help


### Retrieve the list of available goals and environments

The user must specify which task he wants to solve. This is done by using a valid
combination of a *goal* and an *environment*. To retrieve the list of available goals
and environments, do:

    bin$ ./simulator --list

A description of those goals and environments can be found below in this file.


### Start the simulator in FPS mode

To start the simulator in **FPS mode**, do:

    bin$ ./simulator --game --goal=reach_1_flag --environment=SingleRoom --viewsize=800x600

The default value for ```--viewsize``` is ```320x240```.

You can specify any valid combination of goal and environment using ```--goal``` and
```-environment``` (see ```--list``` for the complete list of valid combinations).

The robot can be moved using the arrow keys. The task can be reset (a new environment
is generated) using the ```Tab``` key. To quit the application, use ```Esc```.


### Start the simulator in Server mode

To start the simulator in **Server mode**, do:

    bin$ ./simulator --host=127.0.0.1 --port=11200 --viewsize=320x240

An appropriate TCP client application can now connect to the address ```127.0.0.1```,
on port ```11200```, and control the robot. For details about the communication
protocol, see the [Network Protocol](docs/network_protocol.md) documentation.


## Available goals

The following goals are available:

 * **reach_1_flag**: The robot must find and reach the flag
   ![reach_1_flag](https://raw.github.com/idiap/mash-simulator/master/docs/reach_1_flag.png)
 * **reach_2_flags_in_order**: The robot must first reach the blue flag, and then
   the red one
   ![reach_2_flags_in_order](https://raw.github.com/idiap/mash-simulator/master/docs/reach_2_flags_in_order.png)
 * **reach_unique_flag**: With three flags available in the environment, two of
   them being of the same color, the robot must reach the one that is unique
   ![reach_unique_flag](https://raw.github.com/idiap/mash-simulator/master/docs/reach_unique_flag.png)
 * **all_you_can_eat**: There are a lot of flags in the environment, which the
   robot must *eat* by reaching them. Each time a flag is eat, it disappear and
   a new one appears elsewhere. If the robot doesn't eat a flag for 10 seconds,
   the task is failed.
   ![all_you_can_eat](https://raw.github.com/idiap/mash-simulator/master/docs/all_you_can_eat.png)
 * **eat_black_disks**: Similar to **all_you_can_eat**, but with black disks on
   the floor instead of flags
   ![eat_black_disks](https://raw.github.com/idiap/mash-simulator/master/docs/eat_black_disks.png)
 * **follow_the_light**: The ambient light colour indicates directly which action
   should be performed: red to turn left, green to turn right, white to go forward
   ![follow_the_light](https://raw.github.com/idiap/mash-simulator/master/docs/follow_the_light.png)
 * **follow_the_arrow**: The robot must reach the flag indicated by an arrow painted
   on the wall
   ![follow_the_arrow](https://raw.github.com/idiap/mash-simulator/master/docs/follow_the_arrow.png)
 * **follow_the_line**: The robot must follow the coloured line on the floor to reach
   the flag. The task is failed if the robot leaves the line. 
   ![follow_the_line](https://raw.github.com/idiap/mash-simulator/master/docs/follow_the_line.png)
 * **follow_the_blobs**: The floor is covered in paint. The robot must reach the flag
   without leaving the paint color it started on. 
   ![follow_the_blobs](https://raw.github.com/idiap/mash-simulator/master/docs/follow_the_blobs.png)
 * **reach_correct_pillar**: The robot must reach the pillar painted with diagonal lines
   ![reach_correct_pillar](https://raw.github.com/idiap/mash-simulator/master/docs/reach_correct_pillar.png)
 * **reach_correct_object**: With two pedestals in the environment, the robot must
   reach the one with a trophy on it. 
   ![reach_correct_object](https://raw.github.com/idiap/mash-simulator/master/docs/reach_correct_object.png)
 * **secret**: A secret goal that must be discovered by the machine learning
   algorithm


## Available environments

All the environments are dynamically generated at the beginning of each task.
Some parameters are randomized, like:

 * the dimensions of the rooms and corridors
 * the position and orientation of the targets (flags, pillars, objects)
 * the starting position and orientation of the robot
 * goal-specific parameters (like the path to follow)


The following environments are available:

 * **SingleRoom**: A small rectangular room
   ![SingleRoom](https://raw.github.com/idiap/mash-simulator/master/docs/reach_1_flag.png)
 * **MediumRoom**: A bigger rectangular room
 * **HugeRoom**: A huge rectangular room
 * **TwoRooms**: Two rectangular rooms connected together by a door 
   ![TwoRooms](https://raw.github.com/idiap/mash-simulator/master/docs/TwoRooms.png)
 * **L-ShapedCorridor**: A L-shaped corridor
   ![LShapedCorridor](https://raw.github.com/idiap/mash-simulator/master/docs/LShapedCorridor.png)
 * **T-ShapedCorridor**: A T-shaped corridor
   ![TShapedCorridor](https://raw.github.com/idiap/mash-simulator/master/docs/follow_the_arrow.png)
 * **LightRoom**: A small rectangular room where the ambient light change its color
   regularly
   ![LightRoom](https://raw.github.com/idiap/mash-simulator/master/docs/follow_the_light.png)
 * **Line**: A rectangular room with a line on the floor going from the starting
   position to the flag
   ![Line](https://raw.github.com/idiap/mash-simulator/master/docs/follow_the_line.png)
 * **BlobsRoom**: A rectangular room where the floor is covered in paint
   ![BlobsRoom](https://raw.github.com/idiap/mash-simulator/master/docs/follow_the_blobs.png)
 * **Secret**: A secret environment that must be discovered by the machine learning
   algorithm


## Implementation details

 * Each goal is implemented by inheriting the ```Goal``` class (see the
   ```include/goals/Goal.h``` file)
 * Environment are generated using the ```MapBuilder``` class (see the
   ```include/MapBuilder.h``` file)
 * Internally, a map is stored as an 2D array of cells of different kind (wall,
   floor, target, waypoint, ...) (see the ```include/Map.h``` file)
 * Hand-made teachers are available for some tasks. They all inherit the
   ```Teacher``` class (see the ```include/teachers/Teacher.h``` file). The
   teachers works under the same condition than the machine learning algorithms
   (they only have access to informations that were visible during the
   exploration of the environment), but have access to more detailed
   informations (once an object is seen, the teacher knows its exact position).
 * The ```media``` folder contains different textures for the walls, the floor
   and the ceiling, but aren't used by the simulator at the moment.


## Dependencies

*mash-simulator* is based on the *Athena Game Framework*
(http://github.com/Kanma/Athena-Framework), which is itself built on the following
open-source libraries:

 * the *Ogre3D* rendering engine (http://www.ogre3d.org)
 * the *Bullet* physics engine (http://bulletphysics.org)
 * the *Object Oriented Input Library System (OIS)* (http://sourceforge.net/projects/wgois/)


Those softwares (and their own dependencies) are provided as **GIT submodules** of
this repository and will be compiled automatically during the compilation of
*mash-simulator*.

To download them, do:

    somewhere$ cd <path/to/the/source/of/mash-simulator>
    mash-simulator$ git submodule init
    mash-simulator$ git submodule update


Here is the full list of dependencies of *mash-simulator*:

| Software         | Version | License                      | URL                                      |
|------------------|:-------:|:----------------------------:|------------------------------------------|
| XMake            | 2.3     | MIT                          | http://github.com/Kanma/XMake            |
| UnitTest++       | 1.4     | MIT                          | http://github.com/Kanma/UnitTest-cpp     |
| zlib             | 1.2.3   | ZLib License                 | http://github.com/Kanma/zlib             |
| zziplib          | 0.13.57 | LGPL2 / MPL                  | http://github.com/Kanma/zziplib          |
| freetype         | 2.3.11  | FreeType License / GPL2      | http://github.com/Kanma/FreeType         |
| FreeImage        | 3.13.1  | FreeImage Public License 1.0 | http://github.com/Kanma/FreeImage        |
| Ogre3D           | 1.8.1   | MIT                          | http://github.com/Kanma/Ogre             |
| Bullet           | 2.76    | ZLib License                 | http://github.com/Kanma/Bullet           |
| OIS              | 1.4     | ZLib License                 | http://github.com/Kanma/OIS              |
| rapidjson        | 0.11    | MIT                          | http://github.com/Kanma/rapidjson        |
| Athena-Math      | 1.1.6   | MIT                          | http://github.com/Kanma/Athena-Math      |
| Athena-Core      | 1.3.2   | MIT                          | http://github.com/Kanma/Athena-Core      |
| Athena-Entities  | 1.2.3   | MIT                          | http://github.com/Kanma/Athena-Entities  |
| Athena-Inputs    | 1.0.1   | MIT                          | http://github.com/Kanma/Athena-Inputs    |
| Athena-Physics   | 1.1.3   | MIT                          | http://github.com/Kanma/Athena-Physics   |
| Athena-Graphics  | 1.2.1   | MIT                          | http://github.com/Kanma/Athena-Graphics  |
| Athena-Framework | 1.0.1   | MIT                          | http://github.com/Kanma/Athena-Framework |
| SimpleOpt        | 3.4     | MIT                          | (provided in this repository)            |

A few MASH-related files are provided as part of this repository, and covered by the GPLv3 license too.


## Compilation

(Note: you'll need to install *CMake* to compile *mash-simulator*)

To compile *mash-simulator* (and all its dependencies), do:

    somewhere$ mkdir build
    somewhere$ cd build
    build$ cmake <path/to/the/source/of/mash-simulator>
    build$ make

The executable (and all the libraries) will be put in ```build/bin/```


## License

*mash-simulator* is is made available under the GPLv3 License. The text of the license
is in the file 'COPYING'.

    Copyright (c) 2014 Idiap Research Institute, http://www.idiap.ch/
    Written by Philip Abbet <philip.abbet@idiap.ch>

    mash-simulator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as
    published by the Free Software Foundation.

    mash-simulator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mash-simulator. If not, see <http://www.gnu.org/licenses/>.

