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


#include <maps.h>
#include <Athena-Math/MathUtils.h>
#include <Athena-Math/Color.h>
#include <Athena-Math/Vector2.h>
#include <Athena-Math/ShuffleBag.h>
#include <Athena-Core/Utils/StringConverter.h>
#include <Athena-Entities/Scene.h>
#include <Athena-Graphics/Visual/World.h>
#include <list>

using namespace Athena::Utils;
using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;


#define TO_METERS(dim)  0.001f * ((dim) * pMapBuilder->getMap()->cell_size)


MapBuilder* createSingleRoom()
{
    MapBuilder* pMapBuilder = new MapBuilder(200, 100, 100);

    MapBuilder::tRoomAttributes attributes;

    attributes.width    = pMapBuilder->getRandomNumberGenerator()->randomize(30, 100);
    attributes.height   = pMapBuilder->getRandomNumberGenerator()->randomize(30, 100);
    attributes.left     = 0;
    attributes.top      = 0;
    attributes.wallSize = 1;

    pMapBuilder->addRoom("Room", attributes);

    tZone zone;
    zone.left = 4;
    zone.top = 4;
    zone.width = attributes.width - 8;
    zone.height = attributes.height - 8;

    pMapBuilder->getMap()->start_zones.push_back(zone);
    pMapBuilder->getMap()->target_zones.push_back(zone);

    return pMapBuilder;
}


MapBuilder* createMediumRoom()
{
    MapBuilder* pMapBuilder = new MapBuilder(200, 80, 80);

    MapBuilder::tRoomAttributes attributes;

    attributes.width    = pMapBuilder->getRandomNumberGenerator()->randomize(50, 80);
    attributes.height   = pMapBuilder->getRandomNumberGenerator()->randomize(50, 80);
    attributes.left     = 0;
    attributes.top      = 0;
    attributes.wallSize = 1;

    pMapBuilder->addRoom("Room", attributes);

    tZone zone;
    zone.left = 4;
    zone.top = 4;
    zone.width = attributes.width - 8;
    zone.height = attributes.height - 8;

    pMapBuilder->getMap()->start_zones.push_back(zone);
    pMapBuilder->getMap()->target_zones.push_back(zone);

    return pMapBuilder;
}


MapBuilder* createTwoRooms()
{
    MapBuilder* pMapBuilder = new MapBuilder(200, 100, 100);

    int center_x = 49;
    int center_y = 49;

    int d1 = pMapBuilder->getRandomNumberGenerator()->randomize(10, center_y);
    int d2 = pMapBuilder->getRandomNumberGenerator()->randomize(10, 99 - center_y);

    int d3 = pMapBuilder->getRandomNumberGenerator()->randomize(10, center_y);
    int d4 = pMapBuilder->getRandomNumberGenerator()->randomize(10, 99 - center_y);

    MapBuilder::tRoomAttributes attributes1, attributes2;

    attributes1.width    = pMapBuilder->getRandomNumberGenerator()->randomize(30, center_x);
    attributes1.height   = d1 + d2;
    attributes1.left     = center_x - attributes1.width;
    attributes1.top      = center_y - d1;
    attributes1.wallSize = 1;

    if (attributes1.height < 30)
        attributes1.height = 30;

    attributes2.width    = pMapBuilder->getRandomNumberGenerator()->randomize(30, 99 - center_x);
    attributes2.height   = d3 + d4;
    attributes2.left     = center_x;
    attributes2.top      = center_y - d3;
    attributes2.wallSize = 1;

    if (attributes2.height < 30)
        attributes2.height = 30;

    MapBuilder::tDoor door;

    door.location   = MapBuilder::WALL_EAST;
    door.center     = center_y;
    door.width      = 9;

    attributes1.doors.push_back(door);

    pMapBuilder->addRoom("Room1", attributes1);

    door.location = MapBuilder::WALL_WEST;
    attributes2.doors.push_back(door);

    pMapBuilder->addRoom("Room2", attributes2);

    tZone zone;

    zone.left   = attributes1.left + 4;
    zone.top    = attributes1.top + 4;
    zone.width  = attributes1.width - 8;
    zone.height = attributes1.height - 8;

    pMapBuilder->getMap()->start_zones.push_back(zone);
    pMapBuilder->getMap()->target_zones.push_back(zone);

    zone.left   = attributes2.left + 4;
    zone.top    = attributes2.top + 4;
    zone.width  = attributes2.width - 8;
    zone.height = attributes2.height - 8;

    pMapBuilder->getMap()->start_zones.push_back(zone);
    pMapBuilder->getMap()->target_zones.push_back(zone);

    return pMapBuilder;
}


MapBuilder* createLShapedCorridor()
{
    MapBuilder* pMapBuilder = new MapBuilder(200, 100, 100);

    MapBuilder::tRoomAttributes attributes1, attributes2;

    attributes1.width    = pMapBuilder->getRandomNumberGenerator()->randomize(15, 25);
    attributes1.height   = pMapBuilder->getRandomNumberGenerator()->randomize(25, 100);
    attributes1.left     = 0;
    attributes1.top      = 0;
    attributes1.wallSize = 1;

    attributes2.width    = pMapBuilder->getRandomNumberGenerator()->randomize((unsigned) 25, 100 - attributes1.width);
    attributes2.height   = pMapBuilder->getRandomNumberGenerator()->randomize(15, 25);
    attributes2.left     = attributes1.left + attributes1.width;
    attributes2.top      = 0;
    attributes2.wallSize = 1;

    if (attributes2.height % 2 == 0)
        ++attributes2.height;

    MapBuilder::tDoor door;

    door.location   = MapBuilder::WALL_EAST;
    door.center     = attributes1.top + (attributes2.height - 1) / 2;
    door.width      = attributes2.height - 2;

    attributes1.doors.push_back(door);

    pMapBuilder->addRoom("Hall1", attributes1);

    door.location = MapBuilder::WALL_WEST;
    attributes2.doors.push_back(door);

    pMapBuilder->addRoom("Hall2", attributes2);

    tZone zone;

    zone.left   = attributes1.left + 4;
    zone.top    = attributes1.top + 4;
    zone.width  = attributes1.width - 8;
    zone.height = attributes1.height - 8;

    pMapBuilder->getMap()->start_zones.push_back(zone);
    pMapBuilder->getMap()->target_zones.push_back(zone);

    zone.left   = attributes2.left + 4;
    zone.top    = attributes2.top + 4;
    zone.width  = attributes2.width - 8;
    zone.height = attributes2.height - 8;

    pMapBuilder->getMap()->start_zones.push_back(zone);
    pMapBuilder->getMap()->target_zones.push_back(zone);

    return pMapBuilder;
}


MapBuilder* createTShapedCorridor()
{
    MapBuilder* pMapBuilder = new MapBuilder(200, 100, 100);

    MapBuilder::tRoomAttributes attributes1, attributes2;

    attributes1.width    = pMapBuilder->getRandomNumberGenerator()->randomize(40, 99);

    if (attributes1.width % 2 == 0)
        ++attributes1.width;

    attributes1.height   = pMapBuilder->getRandomNumberGenerator()->randomize(15, 25);
    attributes1.left     = 0;
    attributes1.top      = 0;
    attributes1.wallSize = 1;

    attributes2.width    = pMapBuilder->getRandomNumberGenerator()->randomize(15, 25);

    if (attributes2.width % 2 == 0)
        ++attributes2.width;

    attributes2.height   = pMapBuilder->getRandomNumberGenerator()->randomize((unsigned) 40, 100 - attributes1.height);
    attributes2.left     = attributes1.left + (attributes1.width - 1) / 2 - (attributes2.width - 1) / 2;;
    attributes2.top      = attributes1.top + attributes1.height;
    attributes2.wallSize = 1;

    MapBuilder::tDoor door;

    door.location   = MapBuilder::WALL_SOUTH;
    door.center     = attributes2.left + (attributes2.width - 1) / 2;
    door.width      = attributes2.width - 2;

    attributes1.doors.push_back(door);

    MapBuilder::tDecal decal;

    bool arrow_on_left = (pMapBuilder->getRandomNumberGenerator()->randomize(-100, 100) >= 0);

    decal.location  = MapBuilder::WALL_NORTH;
    decal.material  = (arrow_on_left ? "Decals/Arrow-Left" : "Decals/Arrow-Right");
    decal.center_x  = (attributes1.width - 1) / 2;
    decal.center_y  = 8;
    decal.width     = 1.6f;
    decal.height    = 0.625f;
    decal.u         = 1.0f;
    decal.v         = 0.61f;

    attributes1.decals.push_back(decal);

    pMapBuilder->addRoom("Hall1", attributes1);

    pMapBuilder->getMap()->properties.set("arrow_on_left", new Variant(arrow_on_left));

    door.location = MapBuilder::WALL_NORTH;
    attributes2.doors.push_back(door);

    pMapBuilder->addRoom("Hall2", attributes2);

    tZone zone;

    zone.left   = attributes2.left + 4;
    zone.top    = attributes2.top + attributes2.height - 8;
    zone.width  = attributes2.width - 8;
    zone.height = 4;

    pMapBuilder->getMap()->start_zones.push_back(zone);

    zone.left   = attributes1.left + 4;
    zone.top    = attributes1.top + 4;
    zone.width  = 4;
    zone.height = attributes1.height - 8;

    pMapBuilder->getMap()->target_zones.push_back(zone);

    zone.left   = attributes1.left + attributes1.width - 8;
    zone.top    = attributes1.top + 4;
    zone.width  = 4;
    zone.height = attributes1.height - 8;

    pMapBuilder->getMap()->target_zones.push_back(zone);

    pMapBuilder->setStartOrientation(Quaternion(Degree(0.0f), Vector3::UNIT_Y));

    pMapBuilder->addSpot(attributes2.left + (attributes2.width - 1) / 2, attributes1.top + (attributes1.height - 1) / 2, 0);

    return pMapBuilder;
}


MapBuilder* createSecretMap()
{
    MapBuilder* pMapBuilder = new MapBuilder(200, 100, 100);

    MapBuilder::tRoomAttributes attributes;

    attributes.width    = pMapBuilder->getRandomNumberGenerator()->randomize(50, 100);
    attributes.height   = pMapBuilder->getRandomNumberGenerator()->randomize(50, 100);
    attributes.left     = 0;
    attributes.top      = 0;
    attributes.wallSize = 1;

    pMapBuilder->addRoom("Room", attributes);

    tZone zone;
    zone.left   = 4;
    zone.top    = 4;
    zone.width  = attributes.width - 8;
    zone.height = attributes.height - 8;

    pMapBuilder->getMap()->start_zones.push_back(zone);

    zone.left   = attributes.width / 2 - 1;
    zone.top    = attributes.height / 2 - 1;
    zone.width  = 2;
    zone.height = 2;

    pMapBuilder->getMap()->target_zones.push_back(zone);


    bool vaxis = (pMapBuilder->getRandomNumberGenerator()->randomize(-100.0f, 100.0f) > 0.0f);
    bool side = (pMapBuilder->getRandomNumberGenerator()->randomize(-100.0f, 100.0f) > 0.0f);

    if (vaxis)
    {
        // North wall
        if (side)
        {
            zone.left   = 4;
            zone.top    = 4;
            zone.width  = attributes.width - 8;
            zone.height = 4;
        }
        // South wall
        else
        {
            zone.left   = 4;
            zone.top    = attributes.height - 8;
            zone.width  = attributes.width - 8;
            zone.height = 4;
        }
    }
    else
    {
        // West wall
        if (side)
        {
            zone.left   = 4;
            zone.top    = 4;
            zone.width  = 4;
            zone.height = attributes.height - 8;
        }
        // East wall
        else
        {
            zone.left   = attributes.width - 8;
            zone.top    = 4;
            zone.width  = 4;
            zone.height = attributes.height - 8;
        }
    }

    pMapBuilder->getMap()->target_zones.push_back(zone);

    return pMapBuilder;
}


MapBuilder* createLightRoom()
{
    MapBuilder* pMapBuilder = new MapBuilder(200, 100, 100);

    MapBuilder::tRoomAttributes attributes;

    attributes.width    = pMapBuilder->getRandomNumberGenerator()->randomize(30, 100);
    attributes.height   = pMapBuilder->getRandomNumberGenerator()->randomize(30, 100);
    attributes.left     = 0;
    attributes.top      = 0;
    attributes.wallSize = 1;

    pMapBuilder->addRoom("Room", attributes);

    tZone zone;
    zone.left = 4;
    zone.top = 4;
    zone.width = attributes.width - 8;
    zone.height = attributes.height - 8;

    pMapBuilder->getMap()->start_zones.push_back(zone);

    return pMapBuilder;
}


MapBuilder* createSimpleLine()
{
    const unsigned int CELL_SIZE = 200;
    const unsigned int DECAL_MARGIN_CELLS = 14;
    const unsigned int DECAL_WIDTH_CELLS = 2 * DECAL_MARGIN_CELLS + 1;
    const float DECAL_WIDTH = 0.001f * (DECAL_WIDTH_CELLS * CELL_SIZE);

    MapBuilder* pMapBuilder = new MapBuilder(CELL_SIZE, 200, 200);
    Map* pMap = pMapBuilder->getMap();

    bool haxis = (pMapBuilder->getRandomNumberGenerator()->randomize(-100.0f, 100.0f) > 0.0f);

    unsigned int map_width = (haxis ? 200 : 160);
    unsigned int map_height = (!haxis ? 200 : 160);


    MapBuilder::tRoomAttributes attributes;

    attributes.width    = pMapBuilder->getRandomNumberGenerator()->randomize(map_width - 50, map_width);
    attributes.height   = pMapBuilder->getRandomNumberGenerator()->randomize(map_height - 50, map_height);
    attributes.left     = 0;
    attributes.top      = 0;
    attributes.wallSize = 1;


    tZone zone_start;
    zone_start.left   = 20;
    zone_start.top    = attributes.height - 20;
    zone_start.width  = 1;
    zone_start.height = 1;

    tZone zone_end;
    zone_end.left   = attributes.width - 20;
    zone_end.top    = 20;
    zone_end.width  = 1;
    zone_end.height = 1;

    pMap->properties.set("haxis", new Variant(haxis));


    std::vector<tPoint> spots;

    if (haxis)
    {
        if ((zone_end.left - zone_start.left + DECAL_MARGIN_CELLS) % 2 == 0)
            zone_end.left++;

        if ((zone_end.top - zone_start.top) % 2 == 0)
            zone_end.top++;


        unsigned int d1 = pMapBuilder->getRandomNumberGenerator()->randomize((unsigned) 40, zone_end.left - zone_start.left - 40);

        if ((d1 - DECAL_MARGIN_CELLS) % 2 == 0)
            d1++;

        unsigned int d2 = zone_end.left - zone_start.left - d1;

        assert((d2 + DECAL_WIDTH_CELLS) % 2 == 1);


        pMap->properties.set("spot1_x", new Variant(zone_start.left + d1 - DECAL_MARGIN_CELLS + 2));
        pMap->properties.set("spot1_y", new Variant(zone_start.top));

        pMap->properties.set("spot2_x", new Variant(zone_start.left + d1));
        pMap->properties.set("spot2_y", new Variant(zone_end.top + DECAL_MARGIN_CELLS - 2));


        MapBuilder::tDecal decal;


        // Decal 1
        decal.location    = MapBuilder::FLOOR;
        decal.material    = "Decals/Hazard-Stripes";
        decal.center_x    = zone_start.left + (d1 - DECAL_MARGIN_CELLS - 1) / 2;
        decal.center_y    = zone_start.top;
        decal.z           = 1;
        decal.width       = DECAL_WIDTH;
        decal.height      = 0.001f * (d1 - DECAL_MARGIN_CELLS) * CELL_SIZE;
        decal.u           = 1.0f;
        decal.v           = decal.height / decal.width;
        decal.orientation = Degree(0.0f);

        attributes.decals.push_back(decal);

        // Flag the corresponding cells on the map
        for (unsigned int y = zone_start.top - DECAL_MARGIN_CELLS; y <= zone_start.top + DECAL_MARGIN_CELLS; ++y)
        {
            for (unsigned int x = zone_start.left; x < zone_start.left + d1 - DECAL_MARGIN_CELLS; ++x)
                spots.push_back(tPoint(x, y));
        }

        // Simplify walking around the corner
        unsigned int xx = zone_start.left + d1 - DECAL_MARGIN_CELLS - 1;
        unsigned int yy = zone_start.top - DECAL_MARGIN_CELLS - 1;

        spots.push_back(tPoint(xx, yy));

        spots.push_back(tPoint(xx, yy - 1));
        spots.push_back(tPoint(xx, yy - 2));
        spots.push_back(tPoint(xx, yy - 3));

        spots.push_back(tPoint(xx - 1, yy));
        spots.push_back(tPoint(xx - 2, yy));
        spots.push_back(tPoint(xx - 3, yy));

        spots.push_back(tPoint(xx - 1, yy - 1));
        spots.push_back(tPoint(xx - 2, yy - 2));
        spots.push_back(tPoint(xx - 3, yy - 3));


        // Decal 2
        decal.center_x    = zone_end.left - d2 / 2;
        decal.center_y    = zone_end.top;
        decal.width       = DECAL_WIDTH;
        decal.height      = 0.001f * (d2 + DECAL_WIDTH_CELLS) * CELL_SIZE;
        decal.u           = 1.0f;
        decal.v           = decal.height / decal.width;
        decal.orientation = Degree(0.0f);

        attributes.decals.push_back(decal);

        // Flag the corresponding cells on the map
        for (unsigned int y = zone_end.top - DECAL_MARGIN_CELLS; y <= zone_end.top + DECAL_MARGIN_CELLS; ++y)
        {
            for (unsigned int x = zone_end.left - d2 - DECAL_MARGIN_CELLS; x <= zone_end.left + DECAL_MARGIN_CELLS; ++x)
                spots.push_back(tPoint(x, y));
        }

        // Simplify walking around the corner
        xx = zone_end.left - d2 + DECAL_MARGIN_CELLS + 1;
        yy = zone_end.top + DECAL_MARGIN_CELLS + 1;

        spots.push_back(tPoint(xx, yy));

        spots.push_back(tPoint(xx, yy + 1));
        spots.push_back(tPoint(xx, yy + 2));
        spots.push_back(tPoint(xx, yy + 3));

        spots.push_back(tPoint(xx + 1, yy));
        spots.push_back(tPoint(xx + 2, yy));
        spots.push_back(tPoint(xx + 3, yy));

        spots.push_back(tPoint(xx + 1, yy + 1));
        spots.push_back(tPoint(xx + 2, yy + 2));
        spots.push_back(tPoint(xx + 3, yy + 3));


        // Decal 3
        unsigned int d3 = zone_start.top - zone_end.top;

        decal.center_x    = zone_start.left + d1;
        decal.center_y    = zone_start.top - (d3 - 1) / 2 + DECAL_MARGIN_CELLS;
        decal.width       = DECAL_WIDTH;
        decal.height      = 0.001f * (d3 * CELL_SIZE);
        decal.u           = 1.0f;
        decal.v           = decal.height / decal.width;
        decal.orientation = Degree(90.0f);

        attributes.decals.push_back(decal);

        // Flag the corresponding cells on the map
        for (unsigned int y = zone_start.top - d3 + DECAL_MARGIN_CELLS + 1; y <= zone_start.top + DECAL_MARGIN_CELLS; ++y)
        {
            for (unsigned int x = zone_start.left + d1 - DECAL_MARGIN_CELLS; x <= zone_start.left + d1 + DECAL_MARGIN_CELLS; ++x)
                spots.push_back(tPoint(x, y));
        }

        pMapBuilder->setStartOrientation(Quaternion(Degree(-90.0f), Vector3::UNIT_Y));
    }
    else
    {
        if ((zone_start.top - zone_end.top + DECAL_MARGIN_CELLS) % 2 == 0)
            zone_end.top++;

        if ((zone_end.left - zone_start.left) % 2 == 0)
            zone_end.left++;


        unsigned int d1 = pMapBuilder->getRandomNumberGenerator()->randomize((unsigned) 40, zone_start.top - zone_end.top - 40);

        if ((d1 - DECAL_MARGIN_CELLS) % 2 == 0)
            d1++;

        unsigned int d2 = zone_start.top - zone_end.top - d1;

        assert((d2 + DECAL_WIDTH_CELLS) % 2 == 1);


        pMap->properties.set("spot1_x", new Variant(zone_start.left));
        pMap->properties.set("spot1_y", new Variant(zone_start.top - d1 + DECAL_MARGIN_CELLS - 2));

        pMap->properties.set("spot2_x", new Variant(zone_end.left - DECAL_MARGIN_CELLS + 2));
        pMap->properties.set("spot2_y", new Variant(zone_start.top - d1));


        MapBuilder::tDecal decal;


        // Decal 1
        decal.location    = MapBuilder::FLOOR;
        decal.material    = "Decals/Hazard-Stripes";
        decal.center_x    = zone_start.left;
        decal.center_y    = zone_start.top - (d1 - DECAL_MARGIN_CELLS - 1) / 2;
        decal.z           = 1;
        decal.width       = DECAL_WIDTH;
        decal.height      = 0.001f * (d1 - DECAL_MARGIN_CELLS) * CELL_SIZE;
        decal.u           = 1.0f;
        decal.v           = decal.height / decal.width;
        decal.orientation = Degree(90.0f);

        attributes.decals.push_back(decal);

        // Flag the corresponding cells on the map
        for (unsigned int y = zone_start.top - d1 + DECAL_MARGIN_CELLS + 1; y <= zone_start.top; ++y)
        {
            for (unsigned int x = zone_start.left - DECAL_MARGIN_CELLS; x <= zone_start.left + DECAL_MARGIN_CELLS; ++x)
                spots.push_back(tPoint(x, y));
        }

        // Simplify walking around the corner
        unsigned int xx = zone_start.left + DECAL_MARGIN_CELLS + 1;
        unsigned int yy = zone_start.top - d1 + DECAL_MARGIN_CELLS + 1;

        spots.push_back(tPoint(xx, yy));

        spots.push_back(tPoint(xx, yy + 1));
        spots.push_back(tPoint(xx, yy + 2));
        spots.push_back(tPoint(xx, yy + 3));

        spots.push_back(tPoint(xx + 1, yy));
        spots.push_back(tPoint(xx + 2, yy));
        spots.push_back(tPoint(xx + 3, yy));

        spots.push_back(tPoint(xx + 1, yy + 1));
        spots.push_back(tPoint(xx + 1, yy + 2));
        spots.push_back(tPoint(xx + 2, yy + 1));


        // Decal 2
        decal.center_x    = zone_end.left;
        decal.center_y    = zone_end.top + d2 / 2;
        decal.width       = DECAL_WIDTH;
        decal.height      = 0.001f * (d2 + DECAL_WIDTH_CELLS) * CELL_SIZE;
        decal.u           = 1.0f;
        decal.v           = decal.height / decal.width;
        decal.orientation = Degree(90.0f);

        attributes.decals.push_back(decal);

        // Flag the corresponding cells on the map
        for (unsigned int y = zone_end.top - DECAL_MARGIN_CELLS; y <= zone_end.top + d2 + DECAL_MARGIN_CELLS; ++y)
        {
            for (unsigned int x = zone_end.left - DECAL_MARGIN_CELLS; x <= zone_end.left + DECAL_MARGIN_CELLS; ++x)
                spots.push_back(tPoint(x, y));
        }

        // Simplify walking around the corner
        xx = zone_end.left - DECAL_MARGIN_CELLS - 1;
        yy = zone_end.top + d2 - DECAL_MARGIN_CELLS - 1;

        spots.push_back(tPoint(xx, yy));

        spots.push_back(tPoint(xx, yy - 1));
        spots.push_back(tPoint(xx, yy - 2));
        spots.push_back(tPoint(xx, yy - 3));

        spots.push_back(tPoint(xx - 1, yy));
        spots.push_back(tPoint(xx - 2, yy));
        spots.push_back(tPoint(xx - 3, yy));

        spots.push_back(tPoint(xx - 1, yy - 1));
        spots.push_back(tPoint(xx - 1, yy - 2));
        spots.push_back(tPoint(xx - 2, yy - 1));


        // Decal 3
        unsigned int d3 = zone_end.left - zone_start.left;

        decal.center_x    = zone_start.left + (d3 - 1) / 2 - DECAL_MARGIN_CELLS;
        decal.center_y    = zone_start.top - d1;
        decal.width       = DECAL_WIDTH;
        decal.height      = 0.001f * (d3 * CELL_SIZE);
        decal.u           = 1.0f;
        decal.v           = decal.height / decal.width;
        decal.orientation = Degree(0.0f);

        attributes.decals.push_back(decal);

        // Flag the corresponding cells on the map
        for (unsigned int y = zone_start.top - d1 - DECAL_MARGIN_CELLS; y <= zone_start.top - d1 + DECAL_MARGIN_CELLS; ++y)
        {
            for (unsigned int x = zone_start.left - DECAL_MARGIN_CELLS; x <= zone_start.left + d3 - DECAL_MARGIN_CELLS - 1; ++x)
                spots.push_back(tPoint(x, y));
        }


        pMapBuilder->setStartOrientation(Quaternion(Degree(0.0f), Vector3::UNIT_Y));
    }


    pMapBuilder->addRoom("Room", attributes);

    Athena::Utils::VectorIterator<std::vector<tPoint> > iter(spots.begin(), spots.end());
    while (iter.hasMoreElements())
    {
        tPoint spot = iter.getNext();
        pMapBuilder->addSpot(spot.x, spot.y, 1);
    }

    pMap->start_zones.push_back(zone_start);
    pMap->target_zones.push_back(zone_end);

    return pMapBuilder;
}


MapBuilder* createHugeRoom()
{
    MapBuilder* pMapBuilder = new MapBuilder(200, 300, 300);
    Map* pMap = pMapBuilder->getMap();

    MapBuilder::tRoomAttributes attributes;

    attributes.width    = pMapBuilder->getRandomNumberGenerator()->randomize(150, 200);
    attributes.height   = pMapBuilder->getRandomNumberGenerator()->randomize(150, 200);
    attributes.left     = 0;
    attributes.top      = 0;
    attributes.wallSize = 1;

    pMapBuilder->addRoom("Room", attributes);

    tZone zone;
    zone.left = 10;
    zone.top = 10;
    zone.width = attributes.width - 20;
    zone.height = attributes.height - 20;

    pMap->start_zones.push_back(zone);
    pMap->target_zones.push_back(zone);

    Visual::World::cast(pMap->pScene->getMainComponent(COMP_VISUAL))->setAmbientLight(Color(0.6f, 0.6f, 0.6f));

    pMap->properties.set("min_target_squared_distance", new Variant(25.0f));

    return pMapBuilder;
}


struct tPathSpot
{
    Vector2      coords;
    unsigned int radius;
};


std::vector<tPathSpot> generatePaintedPath(MapBuilder* pMapBuilder,
                                           MapBuilder::tRoomAttributes* attributes,
                                           const std::string& strMaterial,
                                           unsigned int start_x, unsigned int start_y,
                                           unsigned int end_x, unsigned int end_y,
                                           unsigned int z)
{
    std::vector<tPathSpot> path;

    tPathSpot spot;
    spot.coords.x = start_x;
    spot.coords.y = start_y;
    spot.radius   = pMapBuilder->getRandomNumberGenerator()->randomize(20, 24);
    path.push_back(spot);

    tPathSpot end_spot;
    end_spot.coords.x = end_x;
    end_spot.coords.y = end_y;
    end_spot.radius   = pMapBuilder->getRandomNumberGenerator()->randomize(10, 24);

    float dist = end_spot.coords.distance(spot.coords);
    while (dist > path.back().radius + end_spot.radius - 5)
    {
        spot.radius = pMapBuilder->getRandomNumberGenerator()->randomize(10, 24);

        Vector2 direction = end_spot.coords - path.back().coords;
        direction.normalise();

        float angle = pMapBuilder->getRandomNumberGenerator()->randomize(-0.7f, 0.7f);

        float cosa = cos(angle);
        float sina = sin(angle);
        Vector2 new_direction = Vector2(cosa * direction.x - sina * direction.y,
                                        sina * direction.x + cosa * direction.y);

        Vector2 new_coords = path.back().coords + new_direction * (spot.radius + path.back().radius - 5);
        new_coords.x = int(new_coords.x);
        new_coords.y = int(new_coords.y);

        if ((new_coords.x < 10.0f) || (new_coords.x >= attributes->width - 10.0f) ||
            (new_coords.y < 10.0f) || (new_coords.y >= attributes->height - 10.0f))
            continue;

        std::string spot_name = std::string("spot") + StringConverter::toString(path.size() - 1);
        pMapBuilder->getMap()->properties.set(spot_name + "_x", new Variant(new_coords.x));
        pMapBuilder->getMap()->properties.set(spot_name + "_y", new Variant(new_coords.y));
        printf("ADDED %s\n", spot_name.c_str());

        spot.coords = new_coords;
        path.push_back(spot);

        dist = end_spot.coords.distance(spot.coords);
    }

    std::string spot_name = std::string("spot") + StringConverter::toString(path.size() - 1);
    pMapBuilder->getMap()->properties.set(spot_name + "_x", new Variant(end_spot.coords.x));
    pMapBuilder->getMap()->properties.set(spot_name + "_y", new Variant(end_spot.coords.y));
    printf("ADDED %s\n", spot_name.c_str());

    path.push_back(end_spot);

    pMapBuilder->getMap()->properties.set("spots_count", new Variant((unsigned int) path.size() - 1));

    // Declares the decals on the floor
    MapBuilder::tDecal decal;
    decal.location    = MapBuilder::FLOOR;
    decal.material    = strMaterial;
    decal.z           = z;
    decal.u           = 1.0f;
    decal.v           = 1.0f;

    for (unsigned int i = 0; i < path.size(); ++i)
    {
        decal.center_x    = path[i].coords.x;
        decal.center_y    = path[i].coords.y;
        decal.width       = TO_METERS(path[i].radius * 2.4);
        decal.height      = TO_METERS(path[i].radius * 2.4);
        decal.orientation = Degree(pMapBuilder->getRandomNumberGenerator()->randomize(0.0f, 360.0f));

        attributes->decals.push_back(decal);
    }

    return path;
}


MapBuilder* createBlobsRoom()
{
    const unsigned int CELL_SIZE = 200;

    const char* DECAL_MATERIALS[] = {
        "Decals/Blue-Paint",
        "Decals/Red-Paint",
        "Decals/Yellow-Paint"
    };

    const char* FLOOR_MATERIALS[] = {
        "Floors/BluePaint/Basic",
        "Floors/RedPaint/Basic",
        "Floors/YellowPaint/Basic"
    };


    UIntShuffleBag bag;
    bag.add(0, 10);
    bag.add(1, 10);
    bag.add(2, 10);

    unsigned int validMaterial = bag.next();

    unsigned int invalidMaterial = bag.next();
    while (invalidMaterial == validMaterial)
        invalidMaterial = bag.next();

    unsigned int floorMaterial = bag.next();
    while ((floorMaterial == validMaterial) || (floorMaterial == invalidMaterial))
        floorMaterial = bag.next();


    MapBuilder* pMapBuilder = new MapBuilder(CELL_SIZE, 200, 200);
    Map* pMap = pMapBuilder->getMap();

    MapBuilder::tRoomAttributes attributes;

    attributes.width    = pMapBuilder->getRandomNumberGenerator()->randomize(160, 200);
    attributes.height   = pMapBuilder->getRandomNumberGenerator()->randomize(160, 200);
    attributes.left     = 0;
    attributes.top      = 0;
    attributes.wallSize = 1;
    attributes.floorMaterial = FLOOR_MATERIALS[floorMaterial];


    tZone zone_start;
    zone_start.left   = 10;
    zone_start.top    = attributes.height - 10;
    zone_start.width  = 1;
    zone_start.height = 1;

    tZone zone_end;
    zone_end.left   = attributes.width - 20;
    zone_end.top    = 20;
    zone_end.width  = 1;
    zone_end.height = 1;


    // Generate the valid path
    std::vector<tPathSpot> valid_path = generatePaintedPath(pMapBuilder, &attributes, DECAL_MATERIALS[validMaterial],
                                                            zone_start.left + 10, zone_start.top - 10,
                                                            zone_end.left, zone_end.top,
                                                            2);

    // Add some random paint
    MapBuilder::tDecal decal;
    decal.location    = MapBuilder::FLOOR;
    decal.material    = DECAL_MATERIALS[validMaterial];
    decal.z           = 2;
    decal.u           = 1.0f;
    decal.v           = 1.0f;

    for (unsigned int i = 0; i < 10; )
    {
        unsigned int x = pMapBuilder->getRandomNumberGenerator()->randomize(10, (int) attributes.width - 10);
        unsigned int y = pMapBuilder->getRandomNumberGenerator()->randomize(10, (int) attributes.height - 10);
        unsigned int radius = pMapBuilder->getRandomNumberGenerator()->randomize(14, 24);

        Vector2 coords(x, y);

        bool ok = true;
        for (unsigned j = 0; j < valid_path.size(); ++j)
        {
            if (coords.distance(valid_path[j].coords) < radius + valid_path[j].radius + 15)
            {
                ok = false;
                break;
            }
        }

        if (ok)
        {
            decal.center_x    = x;
            decal.center_y    = y;
            decal.width       = TO_METERS(radius * 2.4);
            decal.height      = TO_METERS(radius * 2.4);
            decal.orientation = Degree(pMapBuilder->getRandomNumberGenerator()->randomize(0.0f, 360.0f));

            attributes.decals.push_back(decal);

            ++i;
        }
    }

    // Creates the room
    pMapBuilder->addRoom("Room", attributes);

    pMap->start_zones.push_back(zone_start);
    pMap->target_zones.push_back(zone_end);

    // Flag the corresponding cells on the map
    for (unsigned int i = 0; i < valid_path.size(); ++i)
    {
        for (int x = valid_path[i].coords.x - valid_path[i].radius;
             x < valid_path[i].coords.x + valid_path[i].radius; ++x)
        {
            if ((x < attributes.wallSize + 2) || (x >= attributes.width - attributes.wallSize - 2))
                continue;

            for (int y = valid_path[i].coords.y - valid_path[i].radius;
                 y < valid_path[i].coords.y + valid_path[i].radius; ++y)
            {
                if ((y < attributes.wallSize + 2) || (y >= attributes.height - attributes.wallSize - 2))
                    continue;

                if (valid_path[i].coords.distance(Vector2(x, y)) > valid_path[i].radius)
                    continue;

                pMapBuilder->addSpot(x, y, 1);
            }
        }
    }

    pMapBuilder->setStartOrientation(Quaternion(Degree(-45.0f), Vector3::UNIT_Y));

    return pMapBuilder;
}


MapBuilder* createMap(const std::string& strName)
{
    if (strName == "SingleRoom")
        return createSingleRoom();
    else if (strName == "MediumRoom")
        return createMediumRoom();
    else if (strName == "TwoRooms")
        return createTwoRooms();
    else if (strName == "L-ShapedCorridor")
        return createLShapedCorridor();
    else if (strName == "T-ShapedCorridor")
        return createTShapedCorridor();
    else if (strName == "Secret")
        return createSecretMap();
    else if (strName == "LightRoom")
        return createLightRoom();
    else if (strName == "Line")
        return createSimpleLine();
    else if (strName == "HugeRoom")
        return createHugeRoom();
    else if (strName == "BlobsRoom")
        return createBlobsRoom();

    return 0;
}
