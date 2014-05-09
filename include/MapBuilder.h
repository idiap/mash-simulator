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


#ifndef _MAPBUILDER_H_
#define _MAPBUILDER_H_

#include <Athena/Prerequisites.h>
#include <Athena-Math/Vector3.h>
#include <Athena-Core/Utils/Iterators.h>
#include <Map.h>


enum tCollisionGroup
{
    GROUP_STATIC,
    GROUP_AVATAR,
    GROUP_GHOST,
    GROUP_TRIGGER,
    GROUP_TRIGGER_ONLY,
    GROUP_DYNAMIC,
};



class MapBuilder
{
public:
    enum tLocation
    {
        WALL_NORTH,
        WALL_SOUTH,
        WALL_EAST,
        WALL_WEST,
        FLOOR,
    };

    struct tDoor
    {
        tLocation    location;
        unsigned int center;
        unsigned int width;
    };

    typedef std::vector<tDoor> tDoorList;


    struct tDecal
    {
        tLocation            location;
        std::string          material;
        unsigned int         center_x;
        unsigned int         center_y;
        unsigned int         z;
        float                width;
        float                height;
        float                u;
        float                v;
        Athena::Math::Degree orientation;
    };

    typedef std::vector<tDecal> tDecalList;


    struct tRoomAttributes
    {
        tRoomAttributes()
        : left(0), top(0), width(0), height(0), wallSize(0)
        {
        }

        unsigned int  left;
        unsigned int  top;
        unsigned int  width;
        unsigned int  height;
        unsigned int  wallSize;
        tDoorList     doors;
        tDecalList    decals;
        std::string   floorMaterial;
    };


    //_____ Construction / Destruction __________
public:
    MapBuilder(unsigned int cell_size, unsigned int map_width,
               unsigned int map_height);
    ~MapBuilder();


    //_____ Methods __________
public:
    inline Map* getMap()
    {
        return m_pMap;
    }

    inline Athena::Math::RandomNumberGenerator* getRandomNumberGenerator()
    {
        return &m_pMap->generator;
    }

    inline Athena::Math::Vector3 getStartPosition() const
    {
        return m_startPosition;
    }

    inline void setStartOrientation(const Athena::Math::Quaternion& orientation)
    {
        m_startOrientation = orientation;
    }

    inline Athena::Math::Quaternion getStartOrientation() const
    {
        return m_startOrientation;
    }

    void addRoom(const std::string& strName, const tRoomAttributes& attributes);

    void addTargets(tTargetType type, unsigned int number, const std::string& strMaterial,
                    unsigned int goal_specific, int zone = -1, const std::string& strMesh = "");

    void addSpot(unsigned int left, unsigned int top, unsigned int goal_specific);

    void finalize();

private:
    Ogre::Entity* createPlane(const std::string& strPrefix, const std::string& strMaterial,
                     float fDimX, float fDimY,
                     const Athena::Math::Vector3& position,
                     const Athena::Math::Quaternion& orientation, bool bWall = true);

    void createDecal(const std::string& strPrefix, const std::string& strMaterial,
                     float width, float height, float u, float v,
                     const Athena::Math::Vector3& position,
                     const Athena::Math::Quaternion& orientation);

    tDoorList filterDoors(const tDoorList* doors, tLocation location);
    tDecalList filterDecals(const tDecalList* decals, tLocation location);

    void createLight(const std::string& strName, const Athena::Math::Vector3& position);

    Athena::Entities::Entity* createTarget(tTargetType type,
                                           const std::string& strName,
                                           const std::string& strMaterial,
                                           const std::string& strMesh,
                                           Athena::Entities::Entity* pParentEntity,
                                           const Athena::Math::Vector3& position,
                                           const Athena::Math::Quaternion& orientation);


    //_____ Attributes __________
private:
    Map*                     m_pMap;
    Athena::Math::Vector3    m_startPosition;
    Athena::Math::Quaternion m_startOrientation;
    unsigned int             m_nbRooms;
};

#endif
