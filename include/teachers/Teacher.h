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


#ifndef _TEACHER_H_
#define _TEACHER_H_

#include <Declarations.h>
#include <Map.h>
#include <Athena-Graphics/Visual/Camera.h>
#include <mash-utils/declarations.h>


#define TO_METERS(dim)    0.001f * ((dim) * m_pMap->cell_size)
#define TO_CELL_POS(dim)  (int(1000.0f * dim) / m_pMap->cell_size)


class Teacher
{
    //_____ Construction / Destruction __________
public:
    Teacher(Map* pMap, Athena::Graphics::Visual::Camera* pCamera);
    virtual ~Teacher();


    //_____ Methods __________
public:
    void update(const Athena::Math::Vector3& position,
                const Athena::Math::Quaternion& orientation);

    tAction nextAction();

    virtual Mash::tActionsList notRecommendedActions() = 0;

    unsigned char* getImageOfGrid(unsigned int &width, unsigned int &height);

    virtual void onTargetReached(tTarget* pTarget) {}

protected:
    virtual tAction computeNextAction() = 0;

    bool isCellVisible(unsigned int x, unsigned int y);
    float getDistanceToTarget(const tPoint& target);
    Athena::Math::Degree getAngleToTarget(const tPoint& target);


    //_____ Attributes __________
protected:
    Athena::Graphics::Visual::Camera*   m_pCamera;

    Map*                                m_pMap;
    tCellType*                          m_grid;

    tPoint                              m_robot_position;
    tPointF                             m_robot_position_f;

    std::vector<tPoint>                 m_new_waypoints;
    std::vector<tPoint>                 m_known_waypoints;
    std::vector<tPoint>                 m_detected_targets;

    tAction                             m_nextAction;
};

#endif
