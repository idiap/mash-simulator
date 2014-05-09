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


#include <Map.h>
#include <Athena-Entities/Scene.h>
#include <Athena-Entities/Entity.h>
#include <Athena-Entities/Transforms.h>
#include <Athena-Math/Vector3.h>

using namespace Athena;
using namespace Athena::Math;
using namespace Athena::Entities;


#define TO_METERS(dim)  0.001f * ((dim) * cell_size)
#define FROM_METERS(dim)  (int) (1000 * ((dim) / cell_size))


Map::tColor Map::COLORS[] = {
    { 0, 0, 0},
    { 127, 127, 127},
    { 255, 255, 255},
    { 255, 0, 0},
    { 0, 0, 255},
    { 0, 255, 0},
    { 255, 0, 255},
    { 0, 255, 255},
};


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

Map::Map(unsigned int cell_size, unsigned int map_width, unsigned int map_height)
: cell_size(cell_size), width(map_width), height(map_height), grid(0), pScene(0), pEntity(0)
{
    // Create the grid
    grid = new tCell[width * height];

    for (unsigned int i = 0; i < width * height; ++i)
        grid[i].type = CELL_UNREACHEABLE;
}


Map::~Map()
{
    delete[] grid;
    delete pScene;
}


/************************************** METHODS ****************************************/

unsigned char* Map::getImageOfGrid(unsigned int &width, unsigned int &height)
{
    width = this->width;
    height= this->height;

    unsigned char* pImage = new unsigned char[width * height * 3];
    unsigned char* pDst = pImage;

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            tColor color = COLORS[ grid[y * width + x].type ];
            pDst[0] = color.r;
            pDst[1] = color.g;
            pDst[2] = color.b;

            pDst += 3;
        }
    }

    return pImage;
}


void Map::moveTarget(tTarget* pTarget, Athena::Entities::Entity* pAvatar)
{
    assert(pTarget);
    assert(pAvatar);
    assert(!targets.empty());

    // Retrieve the positions of the avatar and the other targets
    Vector3* positions = new Vector3[targets.size()];
    unsigned int target_info_index = 0;

    for (unsigned int i = 0, j = 0; i < targets.size(); ++i)
    {
        if (targets[i].pEntity != pTarget->pEntity)
        {
            positions[j] = targets[i].pEntity->getTransforms()->getPosition();
            ++j;
        }
        else
        {
            target_info_index = i;
        }
    }

    positions[targets.size() - 1] = pAvatar->getTransforms()->getPosition();

    unsigned int new_x, new_y;

    float threshold = properties.get("min_target_squared_distance")->toFloat();
    while (true)
    {
        float min_squared_dist = 0.0f;
        unsigned int nb_tries = 0;

        Vector3 position;

        while ((min_squared_dist <= threshold) && (nb_tries < 1000))
        {
            unsigned int zone_index = generator.randomize(0, target_zones.size() - 1);
            tZone zone = target_zones[zone_index];

            new_x = generator.randomize(zone.left, zone.left + zone.width - 1);
            new_y = generator.randomize(zone.top, zone.top + zone.height - 1);

            position.x = TO_METERS(new_x);
            position.y = (pTarget->type == TARGET_FLAG ? 0.0f : 0.1f);
            position.z = TO_METERS(new_y);

            min_squared_dist = 100000.0f;
            for (unsigned int i = 0; i < targets.size(); ++i)
            {
                float squared_dist = positions[i].squaredDistance(position);

                if (squared_dist < min_squared_dist)
                    min_squared_dist = squared_dist;
            }

            ++nb_tries;
        }

        if (min_squared_dist > threshold)
        {
            pTarget->pEntity->getTransforms()->setPosition(position);
            pTarget->pEntity->getTransforms()->setOrientation(Quaternion(Degree(generator.randomize(0.0, 360.0f)), Vector3::UNIT_Y));
            break;
        }

        nb_tries = 0;
        threshold -= 1.0f;
    }

    delete[] positions;

    for (unsigned int i = 0; i < width * height; ++i)
    {
        if (grid[i].infos_index == target_info_index)
        {
            grid[i].type = CELL_FLOOR;
            grid[i].infos_index = 0;
        }
    }

    putDisk(new_x, new_y, true, target_info_index);
}


void Map::putDisk(unsigned int center_x, unsigned int center_y, bool present, unsigned int infos_index)
{
    unsigned int radius = FROM_METERS(1.5f);

    for (int y = center_y - radius; y <= center_y + radius; ++y)
    {
        if ((y < 0) || (y >= height))
            continue;

        for (int x = center_x - radius; x <= center_x + radius; ++x)
        {
            if ((x < 0) || (x >= width))
                continue;

            if ((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y) >= radius * radius)
                continue;

            tCell* pCell       = &grid[y * width + x];
            pCell->type        = (present ? CELL_TARGET : CELL_FLOOR);
            pCell->infos_index = infos_index;
            pCell->main_x      = (present ? center_x : -1);
            pCell->main_y      = (present ? center_y : -1);
        }
    }
}
