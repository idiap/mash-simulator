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


#ifndef _MAP_H_
#define _MAP_H_

#include <Athena/Prerequisites.h>
#include <Athena-Core/Utils/Iterators.h>
#include <Athena-Core/Utils/PropertiesList.h>
#include <Athena-Math/RandomNumberGenerator.h>
#include <Athena-Entities/Entity.h>
#include <Ogre/OgreEntity.h>


enum tCellType
{
    CELL_UNREACHEABLE = 0,
    CELL_UNKNOWN,
    CELL_FLOOR,
    CELL_WALL,
    CELL_ROBOT,
    CELL_TARGET,
    CELL_WAYPOINT,
    CELL_SPOT,
};


enum tTargetType
{
    TARGET_FLAG,
    TARGET_DISK,
    TARGET_PILLAR,
    TARGET_OBJECT,
};


struct tTarget
{
    tTargetType                 type;
    Athena::Entities::Entity*   pEntity;
    std::string                 strMaterial;
    std::string                 strMesh;
    unsigned int                goal_specific;
    int                         zone;
};

typedef std::vector<tTarget>                              tTargetsList;
typedef Athena::Utils::VectorIterator<tTargetsList>       tTargetsIterator;
typedef Athena::Utils::ConstVectorIterator<tTargetsList>  tConstTargetsIterator;


struct tCell
{
    tCellType       type;
    unsigned int    room;           // CELL_FLOOR only
    int             main_x;         // CELL_WAYPOINT or CELL_TARGET
    int             main_y;         // CELL_WAYPOINT or CELL_TARGET
    unsigned int    infos_index;    // CELL_TARGET or CELL_SPOT
};


struct tZone
{
    unsigned int top;
    unsigned int left;
    unsigned int width;
    unsigned int height;
};


struct tPoint
{
    tPoint() {}

    tPoint(int xx, int yy)
    : x(xx), y(yy)
    {
    }

    int x;
    int y;
};


struct tPointF
{
    float x;
    float y;
};


struct tSpot
{
    tPoint       position;
    unsigned int goal_specific;
};

typedef std::vector<tSpot>                              tSpotsList;
typedef Athena::Utils::VectorIterator<tSpotsList>       tSpotsIterator;
typedef Athena::Utils::ConstVectorIterator<tSpotsList>  tConstSpotsIterator;


typedef std::map<std::string, Athena::Utils::Variant>   tSettingsList;
typedef Athena::Utils::MapIterator<tSettingsList>       tSettingsIterator;


class Map
{
    //_____ Construction / Destruction __________
public:
    Map(unsigned int cell_size, unsigned int map_width, unsigned int map_height);
    ~Map();


    //_____ Methods __________
public:
    unsigned char* getImageOfGrid(unsigned int &width, unsigned int &height);

    void moveTarget(tTarget* pTarget, Athena::Entities::Entity* pAvatar);

    void putDisk(unsigned int center_x, unsigned int center_y, bool present,
                 unsigned int infos_index);


    //_____ Attributes __________
public:
    Athena::Math::RandomNumberGenerator generator;

    // Grid
    unsigned int                            cell_size;
    unsigned int                            width;
    unsigned int                            height;
    tCell*                                  grid;

    // Zones
    std::vector<tZone>                      start_zones;
    std::vector<tZone>                      target_zones;

    // Targets and spots
    tTargetsList                            targets;
    tSpotsList                              spots;

    // Scene
    Athena::Entities::Scene*                pScene;
    Athena::Entities::Entity*               pEntity;
    Athena::Entities::Entity::tEntitiesList lights;
    Athena::Utils::PropertiesList           properties;


    //_____ Internal types __________
public:
    struct tColor
    {
        unsigned char r, g, b;
    };

    static tColor COLORS[];
};

#endif
