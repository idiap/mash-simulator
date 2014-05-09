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


#include <teachers/Teacher.h>
#include <MapBuilder.h>
#include <Athena-Entities/tComponentID.h>
#include <Athena-Entities/Transforms.h>
#include <Athena-Graphics/Visual/World.h>
#include <Athena-Graphics/Conversions.h>
#include <Ogre/OgreSceneManager.h>
#include <Ogre/OgreSubMesh.h>
#include <Ogre/OgreSubEntity.h>
#include <vector>

using namespace Athena::Math;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Utils;
using namespace std;

using Ogre::SceneManager;


static tPoint gReferencePoint;

bool sortByDistanceWithReferencePoint(tPoint i, tPoint j)
{
    tPoint p;

    p.x = i.x - gReferencePoint.x;
    p.y = i.y - gReferencePoint.y;

    float dist1 = p.x * p.x + p.y + p.y;

    p.x = j.x - gReferencePoint.x;
    p.y = j.y - gReferencePoint.y;

    float dist2 = p.x * p.x + p.y + p.y;

    return (dist1 < dist2);
}


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

Teacher::Teacher(Map* pMap, Athena::Graphics::Visual::Camera* pCamera)
: m_pMap(pMap), m_grid(0), m_pCamera(pCamera), m_nextAction(ACTIONS_COUNT)
{
    assert(pMap);

    m_robot_position.x = pMap->width + 1;
    m_robot_position.y = pMap->height + 1;

    m_robot_position_f.x = 0.001f *  (pMap->width + 1) * m_pMap->cell_size;
    m_robot_position_f.y = 0.001f *  (pMap->height + 1) * m_pMap->cell_size;

    // Create the grid
    m_grid = new tCellType[pMap->width * pMap->height];

    for (unsigned int y = 0; y < pMap->height; ++y)
    {
        for (unsigned int x = 0; x < pMap->width; ++x)
            m_grid[y * pMap->width + x] = (pMap->grid[y * pMap->width + x].type != CELL_UNREACHEABLE ? CELL_UNKNOWN : CELL_UNREACHEABLE);
    }
}


Teacher::~Teacher()
{
    delete[] m_grid;
}


/************************************** METHODS ****************************************/

void Teacher::update(const Vector3& position, const Quaternion& orientation)
{
    // Robot
    if (m_robot_position.x < m_pMap->width)
        m_grid[m_robot_position.y * m_pMap->width + m_robot_position.x] = m_pMap->grid[m_robot_position.y * m_pMap->width + m_robot_position.x].type;

    m_robot_position.x = ((int) ((position.x + m_pMap->cell_size * 0.0005f) * 1000.0f)) / m_pMap->cell_size;
    m_robot_position.y = ((int) ((position.z + m_pMap->cell_size * 0.0005f) * 1000.0f)) / m_pMap->cell_size;

    m_robot_position_f.x = position.x;
    m_robot_position_f.y = position.z;

    m_grid[m_robot_position.y * m_pMap->width + m_robot_position.x] = CELL_ROBOT;

    // View area
    std::vector<tPoint> new_visible_cells;

    for (unsigned int j = 0; j < m_pMap->height; ++j)
    {
        for (unsigned int i = 0; i < m_pMap->width; ++i)
        {
            int index = j * m_pMap->width + i;

            if ((m_grid[index] == CELL_UNKNOWN) && isCellVisible(i, j))
            {
                if (m_pMap->grid[index].type == CELL_FLOOR)
                {
                    Vector3 cameraPos = m_pCamera->getTransforms()->getWorldPosition();
                    Vector3 targetPos(TO_METERS(i) + m_pMap->cell_size * 0.0005f, 0.0f,
                                      TO_METERS(j) + m_pMap->cell_size * 0.0005f);

                    if (targetPos.squaredDistance(position) <= 4.1f * 4.1f)
                        continue;
                }

                tPoint point;
                point.x = i;
                point.y = j;

                new_visible_cells.push_back(point);
            }
        }
    }

    gReferencePoint = m_robot_position;
    std::sort(new_visible_cells.begin(), new_visible_cells.end(), sortByDistanceWithReferencePoint);

    VectorIterator< vector<tPoint> > iter(new_visible_cells.begin(), new_visible_cells.end());
    while (iter.hasMoreElements())
    {
        tPoint cell = iter.getNext();

        int dx = cell.x - m_robot_position.x;
        int dy = cell.y - m_robot_position.y;

        int abs_dx = MathUtils::IAbs(dx);
        int abs_dy = MathUtils::IAbs(dy);

        int sign_dx = MathUtils::ISign(dx);
        int sign_dy = MathUtils::ISign(dy);

        bool bVisible = true;

        if (abs_dx >= abs_dy)
        {
            float a = 0.0f;

            if (abs_dy > 0)
                a = float(dy) / dx;

            for (int x = 0; x < abs_dx; ++x)
            {
                tPoint point;
                point.x = m_robot_position.x + sign_dx * x;
                point.y = m_robot_position.y + round(a * sign_dx * x);

                unsigned int index = point.y * m_pMap->width + point.x;

                if ((m_pMap->grid[index].type == CELL_WALL) ||
                    ((m_pMap->grid[index].type == CELL_TARGET) && (m_pMap->targets[m_pMap->grid[index].infos_index].type == TARGET_FLAG)))
                {
                    bVisible = false;
                    break;
                }
            }
        }
        else
        {
            float a = 0.0f;

            if (abs_dx > 0)
                a = float(dx) / dy;

            for (int y = 0; y < abs_dy; ++y)
            {
                tPoint point;
                point.x = m_robot_position.x + round(a * sign_dy * y);
                point.y = m_robot_position.y + sign_dy * y;

                unsigned int index = point.y * m_pMap->width + point.x;

                if ((m_pMap->grid[index].type == CELL_WALL) ||
                    ((m_pMap->grid[index].type == CELL_TARGET) && (m_pMap->targets[m_pMap->grid[index].infos_index].type == TARGET_FLAG)))
                {
                    bVisible = false;
                    break;
                }
            }
        }

        if (bVisible)
        {
            unsigned int index = cell.y * m_pMap->width + cell.x;
            m_grid[index] = m_pMap->grid[index].type;

            if (m_pMap->grid[index].type == CELL_WAYPOINT)
            {
                m_new_waypoints.push_back(cell);
            }
            else if (m_pMap->grid[index].type == CELL_TARGET)
            {
                tPoint point;
                point.x = m_pMap->grid[index].main_x;
                point.y = m_pMap->grid[index].main_y;

                bool found = false;

                VectorIterator< vector<tPoint> > iter2(m_detected_targets.begin(), m_detected_targets.end());
                while (iter2.hasMoreElements())
                {
                    tPoint cell2 = iter2.getNext();
                    if ((cell2.x == point.x) && (cell2.y == point.y))
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    m_detected_targets.push_back(point);
            }
        }
    }

    m_nextAction = ACTIONS_COUNT;
}


tAction Teacher::nextAction()
{
    // Assertions
    assert(m_pCamera);

    if (m_nextAction != ACTIONS_COUNT)
        return m_nextAction;

    m_nextAction = computeNextAction();

    return m_nextAction;
}


unsigned char* Teacher::getImageOfGrid(unsigned int &width, unsigned int &height)
{
    width = m_pMap->width;
    height= m_pMap->height;

    unsigned char* pImage = new unsigned char[width * height * 3];
    unsigned char* pDst = pImage;

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            Map::tColor color = Map::COLORS[ m_grid[y * m_pMap->width + x] ];
            pDst[0] = color.r;
            pDst[1] = color.g;
            pDst[2] = color.b;

            pDst += 3;
        }
    }

    return pImage;
}


bool Teacher::isCellVisible(unsigned int x, unsigned int y)
{
    float cell_size = 0.001f * m_pMap->cell_size;
    float cell_left = x * cell_size;
    float cell_top  = y * cell_size;

    AxisAlignedBox bound(cell_left, 0.0f, cell_top, cell_left + cell_size, 3.0f,
                         cell_top + cell_size);

    return m_pCamera->isVisible(bound);
}


float Teacher::getDistanceToTarget(const tPoint& target)
{
    Vector3 dir1(TO_METERS(target.x) + m_pMap->cell_size * 0.0005f - m_robot_position_f.x, 0.0f,
                 TO_METERS(target.y) + m_pMap->cell_size * 0.0005f - m_robot_position_f.y);

    return dir1.squaredLength();
}


Degree Teacher::getAngleToTarget(const tPoint& target)
{
    Vector3 dir1(TO_METERS(target.x) + m_pMap->cell_size * 0.0005f - m_robot_position_f.x, 0.0f,
                 TO_METERS(target.y) + m_pMap->cell_size * 0.0005f - m_robot_position_f.y);
    Vector3 dir2(m_pCamera->getTransforms()->getWorldOrientation() * Vector3::NEGATIVE_UNIT_Z);

    Degree angle1(dir1.angleBetween(Vector3::UNIT_X));
    Degree angle2(dir2.angleBetween(Vector3::UNIT_X));

    if (dir1.z < 0.0)
        angle1 = -angle1;

    if (dir2.z < 0.0)
        angle2 = -angle2;

    Degree angle = angle1 - angle2;

    if (angle.valueDegrees() > 180.0f)
        angle -= Degree(360.0f);
    else if (angle.valueDegrees() <= -180.0f)
        angle += Degree(360.0f);

    return angle;
}
