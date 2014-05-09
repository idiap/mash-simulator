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


#include <MapBuilder.h>
#include <Athena-Entities/Scene.h>
#include <Athena-Entities/Entity.h>
#include <Athena-Entities/Transforms.h>
#include <Athena-Entities/ComponentsManager.h>
#include <Athena-Graphics/Visual/Camera.h>
#include <Athena-Graphics/Visual/World.h>
#include <Athena-Graphics/Visual/Plane.h>
#include <Athena-Graphics/Visual/PointLight.h>
#include <Athena-Graphics/Visual/Object.h>
#include <Athena-Physics/World.h>
#include <Athena-Physics/CollisionManager.h>
#include <Athena-Physics/Body.h>
#include <Athena-Physics/GhostObject.h>
#include <Athena-Physics/CompoundShape.h>
#include <Athena-Physics/StaticTriMeshShape.h>
#include <Athena-Physics/PrimitiveShape.h>
#include <Athena-Physics/Conversions.h>
#include <Athena-Core/Utils/StringConverter.h>
#include <Ogre/OgreSubEntity.h>
#include <algorithm>

using namespace Athena;
using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Physics;
using namespace Athena::Utils;



const char* FLOOR_MATERIAL      = "Floors/Floor5/Basic";
const char* CEILING_MATERIAL    = "Ceilings/Ceiling1/Basic";
const char* WALLS_MATERIAL1     = "Walls/Wall10/Basic";
const float MAP_HEIGHT          = 3.0f;


#define TO_METERS(dim)  0.001f * ((dim) * m_pMap->cell_size)
#define FROM_METERS(dim)  (int) (1000 * ((dim) / m_pMap->cell_size))


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

MapBuilder::MapBuilder(unsigned int cell_size, unsigned int map_width,
                       unsigned int map_height)
: m_pMap(0), m_nbRooms(0), m_startOrientation(Quaternion::ZERO)
{
    // Create the map object
    m_pMap = new Map(cell_size, map_width, map_height);
    m_pMap->properties.set("min_target_squared_distance", new Variant(4.0f));

    // Create the scene
    m_pMap->pScene = new Scene("Main");

    Visual::World* pVisualWorld = new Visual::World("", m_pMap->pScene->getComponentsList());

    Ogre::SceneManager* pSceneManager = pVisualWorld->createSceneManager(Ogre::ST_GENERIC);
    pSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    pSceneManager->setShadowFarDistance(20.0f);

    pVisualWorld->setAmbientLight(Color(0.4f, 0.4f, 0.4f));

    Physics::World* pPhysicalWorld = new Physics::World("", m_pMap->pScene->getComponentsList());
    pPhysicalWorld->setWorldType(Physics::World::WORLD_RIGID_BODY);

    // Setup the collision manager
    CollisionManager* pCollisionManager = pPhysicalWorld->getCollisionManager();

    pCollisionManager->enableCollision(GROUP_AVATAR, GROUP_STATIC);
    pCollisionManager->enableCollision(GROUP_AVATAR, GROUP_AVATAR);
    pCollisionManager->enableCollision(GROUP_AVATAR, GROUP_TRIGGER);
    pCollisionManager->enableCollision(GROUP_AVATAR, GROUP_DYNAMIC);

    pCollisionManager->enableCollision(GROUP_DYNAMIC, GROUP_STATIC);
    pCollisionManager->enableCollision(GROUP_DYNAMIC, GROUP_TRIGGER);
    pCollisionManager->enableCollision(GROUP_DYNAMIC, GROUP_DYNAMIC);

    pCollisionManager->enableCollision(GROUP_GHOST, GROUP_STATIC);
    pCollisionManager->enableCollision(GROUP_GHOST, GROUP_AVATAR);
    pCollisionManager->enableCollision(GROUP_GHOST, GROUP_TRIGGER);
    pCollisionManager->enableCollision(GROUP_GHOST, GROUP_TRIGGER_ONLY);
    pCollisionManager->enableCollision(GROUP_GHOST, GROUP_DYNAMIC);

    // Create the entity holding the map
    m_pMap->pEntity = m_pMap->pScene->create("Map");
}


MapBuilder::~MapBuilder()
{
}


/************************************** METHODS ****************************************/

void MapBuilder::addRoom(const std::string& strName, const tRoomAttributes& attributes)
{
    // Assertions
    assert(!strName.empty());
    assert(attributes.width <= m_pMap->width - attributes.left);
    assert(attributes.height <= m_pMap->height - attributes.top);
    assert(attributes.wallSize >= 1);
    assert(attributes.width > 2 * attributes.wallSize);
    assert(attributes.height > 2 * attributes.wallSize);


    // Declarations
    Ogre::Entity* pOgreEntity = 0;
    Ogre::Entity* pFloorEntity = 0;

    unsigned int internal_left   = attributes.left + attributes.wallSize;
    unsigned int internal_top    = attributes.top + attributes.wallSize;
    unsigned int internal_width  = attributes.width - 2 * attributes.wallSize;
    unsigned int internal_height = attributes.height - 2 * attributes.wallSize;

    unsigned int floor_left   = internal_left;
    unsigned int floor_top    = internal_top;
    unsigned int floor_width  = internal_width;
    unsigned int floor_height = internal_height;

    if (!filterDoors(&attributes.doors, WALL_NORTH).empty())
    {
        floor_top -= attributes.wallSize;
        floor_height += attributes.wallSize;
    }

    if (!filterDoors(&attributes.doors, WALL_SOUTH).empty())
        floor_height += attributes.wallSize;

    if (!filterDoors(&attributes.doors, WALL_WEST).empty())
    {
        floor_left -= attributes.wallSize;
        floor_width += attributes.wallSize;
    }

    if (!filterDoors(&attributes.doors, WALL_EAST).empty())
        floor_width += attributes.wallSize;

    // Scene - Floor
    pFloorEntity = createPlane(strName + "/Floor",
                               attributes.floorMaterial.empty() ? FLOOR_MATERIAL : attributes.floorMaterial,
                               TO_METERS(floor_width), TO_METERS(floor_height),
                               Vector3(TO_METERS(floor_left), 0.0f, TO_METERS(floor_top)),
                               Quaternion(Degree(0.0f), Vector3::UNIT_Y), false);

    // Scene - Floor - Decals
    tDecalList filteredDecals = filterDecals(&attributes.decals, FLOOR);

    for (unsigned int i = 0; i< filteredDecals.size(); ++i)
    {
        tDecal decal = filteredDecals[i];

        Vector3 offset = Quaternion(decal.orientation, Vector3::UNIT_Y) * Quaternion(Degree(90.0f), Vector3::UNIT_Y) * Vector3(-decal.width * 0.5f, 0.0f, -decal.height * 0.5f);

        createDecal(strName + "/Floor/Decal/" + StringConverter::toString(i + 1),
                    decal.material, decal.width, decal.height, decal.u, decal.v,
                    Vector3(TO_METERS(attributes.left + decal.center_x) + offset.x, 0.001f * decal.z, TO_METERS(attributes.top + decal.center_y) + offset.z),
                    Quaternion(decal.orientation, Vector3::UNIT_Y) * Quaternion(Degree(90.0f), Vector3::UNIT_Y));
    }

    // Scene - Ceiling
    createPlane(strName + "/Ceiling", CEILING_MATERIAL,
                TO_METERS(floor_width),
                TO_METERS(floor_height),
                Vector3(TO_METERS(floor_left + floor_width), MAP_HEIGHT, TO_METERS(floor_top)),
                Quaternion(Degree(180.0f), Vector3::UNIT_Z), false);

    // Grid - Floor
    for (unsigned int y = internal_top; y < internal_top + internal_height; ++y)
    {
        for (unsigned int x = internal_left; x < internal_left + internal_width; ++x)
        {
            tCell* pCell = &m_pMap->grid[y * m_pMap->width + x];
            pCell->type  = CELL_FLOOR;
            pCell->room  = m_nbRooms;
        }
    }


    // ________________ Lights ________________

    unsigned int dist_x = internal_width / 2;
    if (dist_x > 10)
        dist_x = 10;

    unsigned int dist_y = internal_height / 2;
    if (dist_y > 10)
        dist_y = 10;

    unsigned int dx = (internal_width >= 100 ? internal_width / 4 : 20);
    unsigned int dy = (internal_height >= 100 ? internal_height / 4 : 20);

    unsigned int nb_lights_x = (internal_width - 2 * dist_x) / dx + 1;
    unsigned int nb_lights_y = (internal_height - 2 * dist_y) / dy + 1;

    for (unsigned int y = 0; y < nb_lights_y; ++y)
    {
        for (unsigned int x = 0; x < nb_lights_x; ++x)
        {
            createLight(strName + "/Light_" + StringConverter::toString(x) + "_" + StringConverter::toString(y),
                        Vector3(TO_METERS(internal_left + dist_x + x * dx), MAP_HEIGHT - 0.1f, TO_METERS(internal_top + dist_y + y * dy)));
        }
    }


    // ________________ North Wall ________________

    tDoorList filteredDoors = filterDoors(&attributes.doors, WALL_NORTH);

    unsigned int start = internal_left + internal_width - 1;

    for (int i = filteredDoors.size() - 1; i >= 0; --i)
    {
        tDoor door = filteredDoors[i];

        unsigned int length = start - door.center - ((door.width - 1) >> 1);

        // Wall - Scene
        pOgreEntity = createPlane(strName + "/NorthWall/" + StringConverter::toString(i + 2),
                                  WALLS_MATERIAL1, TO_METERS(length), MAP_HEIGHT,
                                  Vector3(TO_METERS(start + 1), 0.0f, TO_METERS(internal_top)),
                                  Quaternion(Degree(90.0f), Vector3::UNIT_X) * Quaternion(Degree(180.0f), Vector3::UNIT_Y));

        createPlane(strName + "/NorthWall/Door/" + StringConverter::toString(i + 1) + "/Wall1",
                    WALLS_MATERIAL1, TO_METERS(attributes.wallSize), MAP_HEIGHT,
                    Vector3(TO_METERS(start - length + 1), 0.0f, TO_METERS(internal_top)),
                    Quaternion(Degree(90.0f), Vector3::UNIT_Z) * Quaternion(Degree(90.0f), Vector3::UNIT_Y));

        createPlane(strName + "/NorthWall/Door/" + StringConverter::toString(i + 1) + "/Wall2",
                    WALLS_MATERIAL1, TO_METERS(attributes.wallSize), MAP_HEIGHT,
                    Vector3(TO_METERS(start - length - door.width + 1), 0.0f, TO_METERS(internal_top - attributes.wallSize)),
                    Quaternion(Degree(-90.0f), Vector3::UNIT_Z) * Quaternion(Degree(-90.0f), Vector3::UNIT_Y));

        // Wall - Grid
        for (int x = start; x > start - length; --x)
        {
            tCell* pCell = &m_pMap->grid[(internal_top - 1) * m_pMap->width + x];
            pCell->type  = CELL_WALL;
        }

        // Door - Grid
        for (int y = attributes.top; y < internal_top; ++y)
        {
            for (int x = 0; x < door.width; ++x)
            {
                tCell* pCell       = &m_pMap->grid[y * m_pMap->width + start - length - x];
                pCell->type        = CELL_WAYPOINT;
                pCell->main_x      = start - length - ((door.width - 1) >> 1);
                pCell->main_y      = y;
            }

            tCell* pCell = &m_pMap->grid[y * m_pMap->width + start - length + 1];
            pCell->type  = CELL_WALL;

            pCell        = &m_pMap->grid[y * m_pMap->width + start - length - door.width];
            pCell->type  = CELL_WALL;
        }

        start -= length + door.width;
    }

    unsigned int length = start - internal_left + 1;

    // Scene
    pOgreEntity = createPlane(strName + "/NorthWall/1",
                              WALLS_MATERIAL1, TO_METERS(length), MAP_HEIGHT,
                              Vector3(TO_METERS(start + 1), 0.0f, TO_METERS(internal_top)),
                              Quaternion(Degree(90.0f), Vector3::UNIT_X) * Quaternion(Degree(180.0f), Vector3::UNIT_Y));

    // Decals
    filteredDecals = filterDecals(&attributes.decals, WALL_NORTH);

    for (unsigned int i = 0; i< filteredDecals.size(); ++i)
    {
        tDecal decal = filteredDecals[i];

        unsigned int center_x = internal_left + internal_width - 1 - decal.center_x;

        createDecal(strName + "/NorthWall/Decal/" + StringConverter::toString(i + 1),
                    decal.material, decal.width, decal.height, decal.u, decal.v,
                    Vector3(TO_METERS(attributes.left + decal.center_x + 1) +  decal.width * 0.5f, TO_METERS(decal.center_y) - decal.height * 0.5f, TO_METERS(internal_top) + 0.001f),
                    Quaternion(Degree(90.0f), Vector3::UNIT_X) * Quaternion(Degree(180.0f), Vector3::UNIT_Y));
    }

    // Grid
    for (int x = start; x > start - length; --x)
    {
        tCell* pCell = &m_pMap->grid[(internal_top - 1) * m_pMap->width + x];
        pCell->type  = CELL_WALL;
    }

    m_pMap->grid[(internal_top - 1) * m_pMap->width + internal_left - 1].type = CELL_WALL;
    m_pMap->grid[(internal_top - 1) * m_pMap->width + internal_left + internal_width].type = CELL_WALL;


    // ________________ South Wall ________________

    filteredDoors = filterDoors(&attributes.doors, WALL_SOUTH);

    start = internal_left;

    for (int i = 0; i < filteredDoors.size(); ++i)
    {
        tDoor door = filteredDoors[i];

        unsigned int length = door.center - ((door.width - 1) >> 1) - start;

        // Wall - Scene
        pOgreEntity = createPlane(strName + "/SouthWall/" + StringConverter::toString(i + 1),
                                  WALLS_MATERIAL1, TO_METERS(length), MAP_HEIGHT,
                                  Vector3(TO_METERS(start), 0.0f, TO_METERS(internal_top + internal_height)),
                                  Quaternion(Degree(-90.0f), Vector3::UNIT_X));

        createPlane(strName + "/SouthWall/Door/" + StringConverter::toString(i + 1) + "/Wall1",
                    WALLS_MATERIAL1, TO_METERS(attributes.wallSize), MAP_HEIGHT,
                    Vector3(TO_METERS(start + length), 0.0f, TO_METERS(attributes.top + attributes.height - attributes.wallSize)),
                    Quaternion(Degree(-90.0f), Vector3::UNIT_Z) * Quaternion(Degree(-90.0f), Vector3::UNIT_Y));

        createPlane(strName + "/SouthWall/Door/" + StringConverter::toString(i + 1) + "/Wall2",
                    WALLS_MATERIAL1, TO_METERS(attributes.wallSize), MAP_HEIGHT,
                    Vector3(TO_METERS(start + length + door.width), 0.0f, TO_METERS(internal_top + internal_height + attributes.wallSize)),
                    Quaternion(Degree(90.0f), Vector3::UNIT_Z) * Quaternion(Degree(90.0f), Vector3::UNIT_Y));

        // Wall - Grid
        for (int x = start; x < start + length; ++x)
        {
            tCell* pCell = &m_pMap->grid[(internal_top + internal_height) * m_pMap->width + x];
            pCell->type  = CELL_WALL;
        }

        // Door - Grid
        for (int y = internal_top + internal_height; y < attributes.top + attributes.height; ++y)
        {
            for (int x = 0; x < door.width; ++x)
            {
                tCell* pCell  = &m_pMap->grid[y * m_pMap->width + start + length + x];
                pCell->type   = CELL_WAYPOINT;
                pCell->main_x = start + length + ((door.width - 1) >> 1);
                pCell->main_y = y;
            }

            tCell* pCell = &m_pMap->grid[y * m_pMap->width + start + length - 1];
            pCell->type  = CELL_WALL;

            pCell        = &m_pMap->grid[y * m_pMap->width + start + length + door.width];
            pCell->type  = CELL_WALL;
        }

        start += length + door.width;
    }

    length = internal_left + internal_width - start;

    // Scene
    pOgreEntity = createPlane(strName + "/SouthWall/" + StringConverter::toString(filteredDoors.size() + 1),
                              WALLS_MATERIAL1, TO_METERS(length), MAP_HEIGHT,
                              Vector3(TO_METERS(start), 0.0f, TO_METERS(internal_top + internal_height)),
                              Quaternion(Degree(-90.0f), Vector3::UNIT_X));

    // Grid
    for (int x = start; x < start + length; ++x)
    {
        tCell* pCell = &m_pMap->grid[(internal_top + internal_height) * m_pMap->width + x];
        pCell->type  = CELL_WALL;
    }

    m_pMap->grid[(internal_top + internal_height) * m_pMap->width + internal_left - 1].type = CELL_WALL;
    m_pMap->grid[(internal_top + internal_height) * m_pMap->width + internal_left + internal_width].type = CELL_WALL;


    // ________________ West Wall ________________

    filteredDoors = filterDoors(&attributes.doors, WALL_WEST);

    start = internal_top;

    for (int i = 0; i < filteredDoors.size(); ++i)
    {
        tDoor door = filteredDoors[i];

        unsigned int length = door.center - ((door.width - 1) >> 1) - start;

        // Wall - Scene
        pOgreEntity = createPlane(strName + "/WestWall/" + StringConverter::toString(i + 1),
                                  WALLS_MATERIAL1, TO_METERS(length), MAP_HEIGHT,
                                  Vector3(TO_METERS(internal_left), 0.0f, TO_METERS(start)),
                                  Quaternion(Degree(-90.0f), Vector3::UNIT_Z) * Quaternion(Degree(-90.0f), Vector3::UNIT_Y));

        createPlane(strName + "/WestWall/Door/" + StringConverter::toString(i + 1) + "/Wall1",
                    WALLS_MATERIAL1, TO_METERS(attributes.wallSize), MAP_HEIGHT,
                    Vector3(TO_METERS(internal_left), 0.0f, TO_METERS(start + length)),
                    Quaternion(Degree(90.0f), Vector3::UNIT_X) * Quaternion(Degree(180.0f), Vector3::UNIT_Y));

        createPlane(strName + "/WestWall/Door/" + StringConverter::toString(i + 1) + "/Wall2",
                    WALLS_MATERIAL1, TO_METERS(attributes.wallSize), MAP_HEIGHT,
                    Vector3(TO_METERS(attributes.left), 0.0f, TO_METERS(start + length + door.width)),
                    Quaternion(Degree(-90.0f), Vector3::UNIT_X));

        // Wall - Grid
        for (int y = start; y < start + length; ++y)
        {
            tCell* pCell = &m_pMap->grid[y * m_pMap->width + internal_left - 1];
            pCell->type  = CELL_WALL;
        }

        // Door - Grid
        for (int x = attributes.left; x < internal_left; ++x)
        {
            for (int y = 0; y < door.width; ++y)
            {
                tCell* pCell  = &m_pMap->grid[(start + length + y) * m_pMap->width + x];
                pCell->type   = CELL_WAYPOINT;
                pCell->main_x = x;
                pCell->main_y = start + length + ((door.width - 1) >> 1);
            }

            tCell* pCell = &m_pMap->grid[(start + length - 1) * m_pMap->width + x];
            pCell->type  = CELL_WALL;

            pCell        = &m_pMap->grid[(start + length + door.width) * m_pMap->width + x];
            pCell->type  = CELL_WALL;
        }

        start += length + door.width;
    }

    length = internal_top + internal_height - start;

    // Scene
    pOgreEntity = createPlane(strName + "/WestWall/" + StringConverter::toString(filteredDoors.size() + 1),
                              WALLS_MATERIAL1, TO_METERS(length), MAP_HEIGHT,
                              Vector3(TO_METERS(internal_left), 0.0f, TO_METERS(start)),
                              Quaternion(Degree(-90.0f), Vector3::UNIT_Z) * Quaternion(Degree(-90.0f), Vector3::UNIT_Y));

    // Grid
    for (int y = start; y < start + length; ++y)
    {
        tCell* pCell = &m_pMap->grid[y * m_pMap->width + internal_left - 1];
        pCell->type  = CELL_WALL;
    }


    // ________________ East Wall ________________

    filteredDoors = filterDoors(&attributes.doors, WALL_EAST);

    start = internal_top + internal_height - 1;

    for (int i = filteredDoors.size() - 1; i >= 0; --i)
    {
        tDoor door = filteredDoors[i];

        unsigned int length = start - door.center - ((door.width - 1) >> 1);

        // Wall - Scene
        pOgreEntity = createPlane(strName + "/EastWall/" + StringConverter::toString(i + 2),
                                  WALLS_MATERIAL1, TO_METERS(length), MAP_HEIGHT,
                                  Vector3(TO_METERS(internal_left + internal_width), 0.0f, TO_METERS(start + 1)),
                                  Quaternion(Degree(90.0f), Vector3::UNIT_Z) * Quaternion(Degree(90.0f), Vector3::UNIT_Y));

        createPlane(strName + "/EastWall/Door/" + StringConverter::toString(i + 1) + "/Wall1",
                    WALLS_MATERIAL1, TO_METERS(attributes.wallSize), MAP_HEIGHT,
                    Vector3(TO_METERS(internal_left + internal_width), 0.0f, TO_METERS(start - length + 1)),
                    Quaternion(Degree(-90.0f), Vector3::UNIT_X));

        createPlane(strName + "/EastWall/Door/" + StringConverter::toString(i + 1) + "/Wall2",
                    WALLS_MATERIAL1, TO_METERS(attributes.wallSize), MAP_HEIGHT,
                    Vector3(TO_METERS(attributes.left + attributes.width), 0.0f, TO_METERS(start - length - door.width + 1)),
                    Quaternion(Degree(90.0f), Vector3::UNIT_X) * Quaternion(Degree(180.0f), Vector3::UNIT_Y));

        // Wall - Grid
        for (int y = start; y > start - length; --y)
        {
            tCell* pCell = &m_pMap->grid[y * m_pMap->width + internal_left + internal_width];
            pCell->type  = CELL_WALL;
        }

        // Door - Grid
        for (int x = internal_left + internal_width; x < attributes.left + attributes.width; ++x)
        {
            for (int y = 0; y < door.width; ++y)
            {
                tCell* pCell  = &m_pMap->grid[(start - length - y) * m_pMap->width + x];
                pCell->type   = CELL_WAYPOINT;
                pCell->main_x = x;
                pCell->main_y = start - length - ((door.width - 1) >> 1);
            }

            tCell* pCell = &m_pMap->grid[(start - length + 1) * m_pMap->width + x];
            pCell->type  = CELL_WALL;

            pCell        = &m_pMap->grid[(start - length - door.width) * m_pMap->width + x];
            pCell->type  = CELL_WALL;
        }

        start -= length + door.width;
    }

    length = start - internal_top + 1;

    // Scene
    pOgreEntity = createPlane(strName + "/EastWall/1",
                              WALLS_MATERIAL1, TO_METERS(length), MAP_HEIGHT,
                              Vector3(TO_METERS(internal_left + internal_width), 0.0f, TO_METERS(start + 1)),
                              Quaternion(Degree(90.0f), Vector3::UNIT_Z) * Quaternion(Degree(90.0f), Vector3::UNIT_Y));

    // Grid
    for (int y = start; y > start - length; --y)
    {
        tCell* pCell = &m_pMap->grid[y * m_pMap->width + internal_left + internal_width];
        pCell->type  = CELL_WALL;
    }

    ++m_nbRooms;
}


void MapBuilder::addTargets(tTargetType type, unsigned int number, const std::string& strMaterial,
                            unsigned int goal_specific, int zone, const std::string& strMesh)
{
    // Assertions
    assert(m_pMap);

    for (unsigned int i = 0; i < number; ++i)
    {
        tTarget target;
        target.type          = type;
        target.pEntity       = 0;
        target.strMaterial   = strMaterial;
        target.strMesh       = strMesh;
        target.goal_specific = goal_specific;
        target.zone          = zone;

        m_pMap->targets.push_back(target);
    }
}


void MapBuilder::addSpot(unsigned int left, unsigned int top, unsigned int goal_specific)
{
    tSpot spot;
    spot.position.x    = left;
    spot.position.y    = top;
    spot.goal_specific = goal_specific;

    m_pMap->spots.push_back(spot);


    for (unsigned int y = top - 2; y < top + 2; ++y)
    {
        for (unsigned int x = left - 2; x < left + 2; ++x)
        {
            tCell* pCell       = &m_pMap->grid[y * m_pMap->width + x];
            pCell->type        = CELL_SPOT;
            pCell->infos_index = m_pMap->spots.size() - 1;
        }
    }
}


void MapBuilder::finalize()
{
    // Assertions
    assert(m_pMap);
    assert(!m_pMap->start_zones.empty());

    // Generation of the positions of the avatar and the targets
    unsigned int nbPositions = m_pMap->targets.size() + 1;
    tPoint* positions = new tPoint[nbPositions];

    int min_squared_dist = 0;
    int threshold = FROM_METERS(m_pMap->properties.get("min_target_squared_distance")->toFloat());
    while (min_squared_dist <= threshold)
    {
        Entity* pParent = 0;

        for (unsigned int i = 0; i < nbPositions; ++i)
        {
            tZone zone;

            if (i == 0)
            {
                unsigned int zone_index = m_pMap->generator.randomize(0, m_pMap->start_zones.size() * 10 - 1) / 10;
                zone = m_pMap->start_zones[zone_index];
            }
            else if ((m_pMap->targets[i - 1].zone >= 0) && (m_pMap->targets[i - 1].zone < m_pMap->targets.size()))
            {
                zone = m_pMap->target_zones[m_pMap->targets[i - 1].zone];
            }
            else
            {
                unsigned int zone_index = m_pMap->generator.randomize(0, m_pMap->target_zones.size() * 10 - 1) / 10;
                zone = m_pMap->target_zones[zone_index];
            }

            positions[i].x = m_pMap->generator.randomize(zone.left, zone.left + zone.width - 1);
            positions[i].y = m_pMap->generator.randomize(zone.top, zone.top + zone.height - 1);
        }

        min_squared_dist = 100000;

        for (unsigned int i = 0; i < nbPositions; ++i)
        {
            for (unsigned int j = 0; j < nbPositions; ++j)
            {
                if (i == j)
                    continue;

                int squared_dist = (positions[i].x - positions[j].x) * (positions[i].x - positions[j].x) +
                                   (positions[i].y - positions[j].y) * (positions[i].y - positions[j].y);

                if (squared_dist < min_squared_dist)
                    min_squared_dist = squared_dist;
            }
        }
    }


    // Put the avatar in place
    m_startPosition = Vector3(TO_METERS(positions[0].x), 0.0f, TO_METERS(positions[0].y));
    if (m_startOrientation == Quaternion::ZERO)
        m_startOrientation = Quaternion(Degree(m_pMap->generator.randomize(0.0, 360.0f)), Vector3::UNIT_Y);

    // Create the targets
    tTargetsIterator iter(m_pMap->targets.begin(), m_pMap->targets.end());
    unsigned int n = 1;
    while (iter.hasMoreElements())
    {
        tTarget* pTarget = iter.peekNextPtr();

        Quaternion orientation = Quaternion::IDENTITY;
        Vector3 position;

        if (pTarget->type == TARGET_FLAG)
        {
            orientation = Quaternion(Degree(m_pMap->generator.randomize(0.0, 360.0f)), Vector3::UNIT_Y);

            position = Vector3(TO_METERS(0.5f + positions[n].x), 0.0f, TO_METERS(0.5f + positions[n].y));
            position -= orientation * Vector3(0.25f, 0.0f, 0.0f);
        }
        else if (pTarget->type == TARGET_DISK)
        {
            position = Vector3(TO_METERS(positions[n].x), 0.1f, TO_METERS(positions[n].y));
        }
        else if ((pTarget->type == TARGET_PILLAR) || (pTarget->type == TARGET_OBJECT))
        {
            position = Vector3(TO_METERS(0.5f + positions[n].x), 0.0f, TO_METERS(0.5f + positions[n].y));
        }

        pTarget->pEntity = createTarget(pTarget->type, "Target" + StringConverter::toString(n),
                                        pTarget->strMaterial, pTarget->strMesh, m_pMap->pEntity,
                                        position, orientation);

        if ((pTarget->type == TARGET_FLAG) || (pTarget->type == TARGET_PILLAR) || (pTarget->type == TARGET_OBJECT))
        {
            tCell* pCell       = &m_pMap->grid[positions[n].y * m_pMap->width + positions[n].x];
            pCell->type        = CELL_TARGET;
            pCell->infos_index = n - 1;
            pCell->main_x      = positions[n].x;
            pCell->main_y      = positions[n].y;
        }
        else if (pTarget->type == TARGET_DISK)
        {
            m_pMap->putDisk(positions[n].x, positions[n].y, true, n-1);
        }

        ++n;
        iter.moveNext();
    }


    delete[] positions;
}


Ogre::Entity* MapBuilder::createPlane(const std::string& strPrefix, const std::string& strMaterial,
                                      float fDimX, float fDimZ, const Athena::Math::Vector3& position,
                                      const Athena::Math::Quaternion& orientation, bool bWall)
{
    Transforms* pTransforms = new Transforms(strPrefix + "/Transforms", m_pMap->pEntity->getComponentsList());
    pTransforms->setTransforms(m_pMap->pEntity->getTransforms());
    pTransforms->translate(position);
    pTransforms->rotate(orientation);

    Transforms* pTransforms2 = new Transforms(strPrefix + "/Offset", m_pMap->pEntity->getComponentsList());
    pTransforms2->setTransforms(pTransforms);
    pTransforms2->translate(fDimX / 2, 0.0f, fDimZ / 2);

    float uFactor = fDimZ / 4;
    if (uFactor < 1.0f)
        uFactor = 1.0f;
    float vFactor = (fDimX / fDimZ) * uFactor;

    Visual::Plane* pPlane = new Visual::Plane(strPrefix + "/Plane", m_pMap->pEntity->getComponentsList());
    if (pPlane->createPlane(strMaterial, Vector3::UNIT_Y, 0.0f, fDimX, fDimZ, std::max(1.0f, fDimX * 4),
                            std::max(1.0f, fDimZ * 4), true, 1, (bWall ? fDimX / fDimZ : vFactor),
                            (bWall ? 1.0f : uFactor), Vector3::UNIT_Z))
    {
        pPlane->setTransforms(pTransforms2);
    }
    else
    {
        ComponentsManager::getSingletonPtr()->destroy(pPlane);
    }

    Body* pBody = new Body(strPrefix + "/Body", m_pMap->pEntity->getComponentsList());
    pBody->setTransforms(pTransforms);
    pBody->setCollisionGroup(GROUP_STATIC);

    float* pVertices = new float[4 * 3];
    pVertices[0]    = 0.0f;
    pVertices[1]    = 0.0f;
    pVertices[2]    = 0.0f;
    pVertices[3]    = 0.0f;
    pVertices[4]    = 0.0f;
    pVertices[5]    = fDimZ;
    pVertices[6]    = fDimX;
    pVertices[7]    = 0.0f;
    pVertices[8]    = fDimZ;
    pVertices[9]    = fDimX;
    pVertices[10]   = 0.0f;
    pVertices[11]   = 0.0f;

    int* pIndices = new int[6];
    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;
    pIndices[3] = 2;
    pIndices[4] = 3;
    pIndices[5] = 0;

    StaticTriMeshShape* pShape = new StaticTriMeshShape(strPrefix + "/Shape", m_pMap->pEntity->getComponentsList());
    pShape->addMesh(4, pVertices, 6, pIndices, StaticTriMeshShape::MEM_TAKE_OWNERSHIP);
    pBody->setCollisionShape(pShape);

    return (pPlane ? pPlane->getOgreEntity() : 0);
}


void MapBuilder::createDecal(const std::string& strPrefix, const std::string& strMaterial,
                             float width, float height, float u, float v,
                             const Athena::Math::Vector3& position,
                             const Athena::Math::Quaternion& orientation)
{
    Transforms* pTransforms = new Transforms(strPrefix + "/Transforms", m_pMap->pEntity->getComponentsList());
    pTransforms->setTransforms(m_pMap->pEntity->getTransforms());
    pTransforms->translate(position);
    pTransforms->rotate(orientation);

    Transforms* pTransforms2 = new Transforms(strPrefix + "/Offset", m_pMap->pEntity->getComponentsList());
    pTransforms2->setTransforms(pTransforms);
    pTransforms2->translate(width / 2, 0.0f, height / 2);

    Visual::Plane* pPlane = new Visual::Plane(strPrefix + "/Plane", m_pMap->pEntity->getComponentsList());
    if (pPlane->createPlane(strMaterial, Vector3::UNIT_Y, 0.0f, width, height, std::max(1.0f, width * 4),
                            std::max(1.0f, height * 4), true, 1, u, v, Vector3::UNIT_Z))
    {
        pPlane->setTransforms(pTransforms2);
    }
    else
    {
        ComponentsManager::getSingletonPtr()->destroy(pPlane);
    }
}


bool sortByCenter(MapBuilder::tDoor i, MapBuilder::tDoor j)
{
    return (i.center < j.center);
}


bool sortByDecalCenter(MapBuilder::tDecal i, MapBuilder::tDecal j)
{
    return (i.center_x < j.center_x) && (i.center_y < j.center_y);
}


MapBuilder::tDoorList MapBuilder::filterDoors(const tDoorList* doors, tLocation location)
{
    tDoorList filtered;

    if (!doors)
        return filtered;

    tDoorList::const_iterator iter, iterEnd;
    for (iter = doors->begin(), iterEnd = doors->end(); iter != iterEnd; ++iter)
    {
        if (iter->location == location)
            filtered.push_back(*iter);
    }

    std::sort(filtered.begin(), filtered.end(), sortByCenter);

    return filtered;
}


MapBuilder::tDecalList MapBuilder::filterDecals(const tDecalList* decals, tLocation location)
{
    tDecalList filtered;

    if (!decals)
        return filtered;

    tDecalList::const_iterator iter, iterEnd;
    for (iter = decals->begin(), iterEnd = decals->end(); iter != iterEnd; ++iter)
    {
        if (iter->location == location)
            filtered.push_back(*iter);
    }

    std::sort(filtered.begin(), filtered.end(), sortByDecalCenter);

    return filtered;
}


void MapBuilder::createLight(const std::string& strName, const Athena::Math::Vector3& position)
{
    // Create the target entity
    Entity* pEntity = m_pMap->pScene->create(strName);

    pEntity->getTransforms()->translate(position);

    Visual::PointLight* pLight = new Visual::PointLight("PointLight", pEntity->getComponentsList());
    pLight->setDiffuseColor(Color(0.7f, 0.7f, 0.7f));
    pLight->setSpecularColor(Color(0.95f, 0.95f, 0.95f));
    pLight->setAttenuation(20.0f, 0.8f, 0.1f, 0.05f);

    m_pMap->lights.push_back(pEntity);
}


Entity* MapBuilder::createTarget(tTargetType type,
                                 const std::string& strName,
                                 const std::string& strMaterial,
                                 const std::string& strMesh,
                                 Entity* pParentEntity,
                                 const Vector3& position,
                                 const Quaternion& orientation)
{
    // Assertions
    assert(m_pMap);

    // Create the target entity
    Entity* pTarget = m_pMap->pScene->create(strName);

    if (pParentEntity)
        pParentEntity->addChild(pTarget);

    // Set the transforms
    pTarget->getTransforms()->translate(position);
    pTarget->getTransforms()->rotate(orientation);

    // Create the physical representation (the actual shape is dependent of the target type)
    Body* pBody = new Body("Body", pTarget->getComponentsList());
    pBody->setMass(0.0f);
    pBody->setCollisionGroup(GROUP_TRIGGER);

    PrimitiveShape* pShape = new PrimitiveShape("Shape", pTarget->getComponentsList());

    // Create the visual representation and the physical shape
    if (type == TARGET_FLAG)
    {
        pTarget->getTransforms()->scale(0.1f, 0.1f, 0.1f);

        Visual::Object* pObject = new Visual::Object("Mesh", pTarget->getComponentsList());
        if (pObject->loadMesh("flag.mesh"))
        {
            if (!strMaterial.empty())
                pObject->getOgreEntity()->getSubEntity(0)->setMaterialName(strMaterial);

            pObject->setCastShadows(true);
        }
        else
        {
            ComponentsManager::getSingletonPtr()->destroy(pObject);
        }

        pShape->createCylinder(0.25f, 2.0f);
    }
    else if (type == TARGET_DISK)
    {
        Visual::Plane* pPlane = new Visual::Plane("Plane", pTarget->getComponentsList());
        if (!pPlane->createPlane(strMaterial, Vector3::UNIT_Y, 0.0f, 3.0f, 3.0f, 1.0f, 1.0f,
                                 true, 1, 1.0f, 1.0f, Vector3::UNIT_Z))
        {
            ComponentsManager::getSingletonPtr()->destroy(pPlane);
        }

        pShape->createCylinder(3.0f, 2.0f);
    }
    else if (type == TARGET_PILLAR)
    {
        pTarget->getTransforms()->scale(0.01f, 0.01f, 0.01f);

        Visual::Object* pObject = new Visual::Object("Mesh", pTarget->getComponentsList());
        if (pObject->loadMesh("pillar.mesh"))
        {
            if (!strMaterial.empty())
                pObject->getOgreEntity()->getSubEntity(0)->setMaterialName(strMaterial);

            pObject->setCastShadows(true);
        }
        else
        {
            ComponentsManager::getSingletonPtr()->destroy(pObject);
        }

        pShape->createBox(Vector3(0.4f, 3.0f, 0.4f));
    }
    else if (type == TARGET_OBJECT)
    {
        float height = m_pMap->generator.randomize(1.4f, 1.7f);

        Visual::Object* pPedestal = new Visual::Object("Pedestal", pTarget->getComponentsList());
        if (pPedestal->loadMesh("pedestal.mesh"))
        {
            Transforms* pTransforms2 = new Transforms("PedestalOffset", pTarget->getComponentsList());
            pTransforms2->scale(0.01f, 0.01f * height / 1.5f, 0.01f);

            pPedestal->setCastShadows(true);
            pPedestal->setTransforms(pTransforms2);

            Visual::Object* pObject = new Visual::Object("Object", pTarget->getComponentsList());
            if (pObject->loadMesh(strMesh.empty() ? "trophy.mesh" : strMesh))
            {
                if (!strMaterial.empty())
                    pObject->getOgreEntity()->getSubEntity(0)->setMaterialName(strMaterial);

                Transforms* pTransforms3 = new Transforms("ObjectOffset", pTarget->getComponentsList());
                pTransforms3->translate(0.0f, height, 0.0f);
                pTransforms3->scale(0.01f, 0.01f, 0.01f);
                pTransforms3->rotate(Vector3::UNIT_Y, Degree(m_pMap->generator.randomize(0.0f, 360.0f)));

                pObject->setCastShadows(true);
                pObject->setTransforms(pTransforms3);
            }
            else
            {
                ComponentsManager::getSingletonPtr()->destroy(pObject);
            }
        }
        else
        {
            ComponentsManager::getSingletonPtr()->destroy(pPedestal);
        }

        pShape->createCylinder(0.4f, 2.0f);
    }


    pBody->setCollisionShape(pShape);

    return pTarget;
}
