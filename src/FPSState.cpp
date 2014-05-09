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


#include <Declarations.h>
#include <FPSState.h>
#include <MapBuilder.h>
#include <maps.h>
#include <goals/goals.h>
#include <teachers/teachers.h>

#include <Athena/Engine.h>
#include <Athena/Tasks/TaskManager.h>
#include <Athena/GameStates/GameStateManager.h>
#include <Athena-Inputs/InputsUnit.h>
#include <Athena-Entities/Scene.h>
#include <Athena-Entities/Entity.h>
#include <Athena-Entities/Transforms.h>
#include <Athena-Graphics/Visual/Camera.h>
#include <Athena-Graphics/Visual/World.h>
#include <Athena-Physics/World.h>
#include <Athena-Physics/CollisionManager.h>
#include <Athena-Physics/Body.h>
#include <Athena-Physics/GhostObject.h>
#include <Athena-Physics/CompoundShape.h>
#include <Athena-Physics/Conversions.h>
#include <Athena-Core/Log/LogManager.h>
#include <Athena-Core/Utils/StringConverter.h>
#include <Athena-Math/Vector3.h>
#include <Ogre/OgreRenderWindow.h>
#include <Ogre/OgreSceneManager.h>
#include <Ogre/OgreOverlayManager.h>


using namespace Athena;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Physics;
using namespace Athena::Inputs;
using namespace Athena::Math;
using namespace Athena::Utils;
using namespace Athena::Log;


static const char* __CONTEXT__ = "FPS State";


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

FPSState::FPSState()
: m_pViewport(0), m_pTeacher(0), m_pAvatar(0), m_pAvatarBody(0), m_pAvatarGhost(0),
  m_pCameraAxis(0), m_pOverlay(0), m_pMap(0), m_pGoal(0), m_result(RESULT_NONE)
{
}


FPSState::~FPSState()
{
}


/************************************** METHODS ****************************************/

void FPSState::setup(const std::string& goal, const std::string& environment)
{
    assert(!goal.empty());
    assert(!environment.empty());
    assert(!m_pMap);
    assert(!m_pGoal);

    m_selectedMap = environment;
    m_selectedGoal = goal;
}


void FPSState::resetTask()
{
    assert(!m_selectedGoal.empty());
    assert(!m_selectedMap.empty());

    if (m_pOverlay)
    {
        m_pOverlay->hide();
        m_pOverlay = 0;
    }

    Engine::getSingletonPtr()->getMainWindow()->removeViewport(0);

    delete m_pMap;
    delete m_pGoal;
    delete m_pTeacher;


    MapBuilder* pMapBuilder = createMap(m_selectedMap);

    m_pGoal = createGoal(m_selectedGoal);
    m_pGoal->setup(pMapBuilder);

    pMapBuilder->finalize();
    m_pMap = pMapBuilder->getMap();
    m_pMap->pScene->show();


    // Create the avatar entity
    m_pAvatar = m_pMap->pScene->create("Avatar");

    Physics::World* pPhysicalWorld = Physics::World::cast(m_pMap->pScene->getMainComponent(COMP_PHYSICAL));

    m_pAvatarBody = new Body("Body", m_pAvatar->getComponentsList());
    m_pAvatarBody->setMass(75.0f / MathUtils::Abs(pPhysicalWorld->getGravity().length()));
    m_pAvatarBody->setAngularFactor(0.0f);
    m_pAvatarBody->enableDeactivation(false);
    m_pAvatarBody->setCollisionGroup(GROUP_AVATAR);

    m_pAvatarGhost = new GhostObject("Ghost", m_pAvatar->getComponentsList());
    m_pAvatarGhost->setCollisionGroup(GROUP_GHOST);

    CompoundShape* pAvatarShape = new CompoundShape("Shape", m_pAvatar->getComponentsList());
    pAvatarShape->addCapsule(0.25f, 1.25f, PrimitiveShape::AXIS_Y, Vector3(0.0f, 1.75f * 0.5f, 0.0f));
    m_pAvatarBody->setCollisionShape(pAvatarShape);
    m_pAvatarGhost->setCollisionShape(pAvatarShape);

    // Attach the camera to the avatar
    m_pCameraAxis = new Transforms("CameraTransforms", m_pAvatar->getComponentsList());
    m_pCameraAxis->translate(0.0f, 1.75f, 0.0f);

    Camera* pCamera = new Camera("Camera", m_pAvatar->getComponentsList());
    pCamera->setTransforms(m_pCameraAxis);
    pCamera->setNearClipDistance(0.1f);
    pCamera->setFarClipDistance(100.0f);
    pCamera->setFOVy(Degree(45.0f));
    pCamera->setAspectRatio(float(VIEW_WIDTH) / VIEW_HEIGHT);

    // Create one viewport, entire window
    m_pViewport = pCamera->createViewport(Engine::getSingletonPtr()->getMainWindow());
    m_pViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));


    m_pAvatarBody->getRigidBody()->proceedToTransform(
                            btTransform(toBullet(pMapBuilder->getStartOrientation()),
                                        toBullet(pMapBuilder->getStartPosition())));


    m_pGoal->finalize(m_pMap);


    m_pTeacher = createTeacher(m_selectedGoal, m_selectedMap, m_pMap, pCamera);
    if (m_pTeacher)
        m_pTeacher->update(pMapBuilder->getStartPosition(), pMapBuilder->getStartOrientation());


    unsigned int width, height;
    unsigned char* pImage = m_pMap->getImageOfGrid(width, height);

    std::ofstream stream;

    stream.open("map.ppm", std::ios::out | std::ios::binary);
    if (stream.is_open())
    {
        std::ostringstream str;
        str << "P6" << std::endl << width << " " << height << std::endl << 255 << std::endl;

        stream.write(str.str().c_str(), (std::streamsize) str.str().length());
        stream.write((char*) pImage, width * height * 3 * sizeof(unsigned char));

        stream.close();
    }

    delete[] pImage;


    delete pMapBuilder;

    m_result = RESULT_NONE;
}


/************************ METHODS TO BE OVERRIDEN BY EACH STATE ************************/

void FPSState::enter()
{
    resetTask();
}


void FPSState::exit()
{
    if (m_pOverlay)
    {
        m_pOverlay->hide();
        m_pOverlay = 0;
    }

    Engine::getSingletonPtr()->getMainWindow()->removeViewport(0);

    delete m_pGoal;
    delete m_pMap;
    delete m_pTeacher;

    m_pAvatar     = 0;
    m_pViewport   = 0;
    m_pMap        = 0;
    m_pGoal       = 0;
    m_pTeacher    = 0;
}


void FPSState::pause()
{
}


void FPSState::resume()
{
}


void FPSState::process()
{
    static float prevReward = 0.0f;
    static bool prevFalling = true;

    if (m_pTeacher)
        m_pTeacher->nextAction();

    // m_pPhysicalWorld->getRigidBodyWorld()->debugDrawWorld();

    // Retrieve the number of milliseconds elapsed since the last frame
    float elapsed = (float) Engine::getSingletonPtr()->getTaskManager()->getElapsedMicroseconds() * 1e-3f;

    if ((m_result == RESULT_NONE) && m_pGoal->isInitialized() && m_pGoal->updateTimeout(elapsed))
    {
        m_result = RESULT_FAILED;

        printf("----------------------------------------\n");
        printf("TIMEOUT\n");
    }

    if (m_result == RESULT_NONE)
    {
        float reward;
        tResult result = m_pGoal->process(reward, m_pTeacher);

        if (m_pGoal->isInitialized())
        {
            if ((reward != prevReward) || (result != m_result) ||
                (m_pGoal->isAvatarFalling() != prevFalling))
            {
                printf("----------------------------------------\n");
                printf("REWARD = %f\n", reward);
                printf("RESULT = %s\n", (result == RESULT_SUCCESS ? "Success" : (result == RESULT_FAILED ? "Failed" : "-")));
                printf("FALLING = %d\n", m_pGoal->isAvatarFalling());
            }

            prevReward  = reward;
            prevFalling = m_pGoal->isAvatarFalling();
            m_result    = result;
        }
    }


    if (m_pTeacher)
    {
        m_pTeacher->update(m_pAvatar->getTransforms()->getWorldPosition(),
                           m_pAvatar->getTransforms()->getWorldOrientation());

        unsigned int width, height;
        unsigned char* pImage = m_pTeacher->getImageOfGrid(width, height);

        std::ofstream stream;

        stream.open("map2.ppm", std::ios::out | std::ios::binary);
        if (stream.is_open())
        {
            std::ostringstream str;
            str << "P6" << std::endl << width << " " << height << std::endl << 255 << std::endl;

            stream.write(str.str().c_str(), (std::streamsize) str.str().length());
            stream.write((char*) pImage, width * height * 3 * sizeof(unsigned char));

            stream.close();
        }

        delete[] pImage;
    }


    // Retrieve the virtual controller
    VirtualController* pController = Engine::getSingletonPtr()->getInputsUnit()->getVirtualController("MainController");
    if (!pController)
        return;

    if ((m_result == RESULT_NONE) && m_pGoal->isInitialized() && !m_pGoal->isAvatarFalling())
    {
        // Change the orientation of the avatar if needed
        Quaternion orientation = m_pAvatar->getTransforms()->getWorldOrientation();

        if (pController->isKeyPressed(VKEY_LEFT))
        {
            m_pAvatarBody->setAngularVelocity(Vector3(0.0f, Degree(60.0f).valueRadians(), 0.0f));
            orientation = Quaternion(Degree(60.0f * 1e-6f * elapsed), Vector3::UNIT_Y) * orientation;
        }
        else if (pController->isKeyPressed(VKEY_RIGHT))
        {
            m_pAvatarBody->setAngularVelocity(Vector3(0.0f, Degree(-60.0f).valueRadians(), 0.0f));
            orientation = Quaternion(Degree(-60.0f * 1e-6f * elapsed), Vector3::UNIT_Y) * orientation;
        }
        else
        {
            m_pAvatarBody->setAngularVelocity(Vector3(0.0f, 0.0f, 0.0f));
        }

        // Change the velocity of the avatar if needed
        Vector3 currentVelocity = m_pAvatarBody->getLinearVelocity();

        if (pController->isKeyPressed(VKEY_UP))
            m_pAvatarBody->setLinearVelocity(Vector3(0.0f, currentVelocity.y, -3.0f));
        else if (pController->isKeyPressed(VKEY_DOWN))
            m_pAvatarBody->setLinearVelocity(Vector3(0.0f, currentVelocity.y, 3.0f));
        else
            m_pAvatarBody->setLinearVelocity(Vector3(0.0f, currentVelocity.y, 0.0f));
    }
    else if (m_result != RESULT_NONE)
    {
        m_pAvatarBody->setAngularVelocity(Vector3(0.0f, 0.0f, 0.0f));
        m_pAvatarBody->setLinearVelocity(Vector3(0.0f, 0.0f, 0.0f));
    }

    if ((m_result != RESULT_NONE) && !m_pOverlay)
    {
        m_pOverlay = Ogre::OverlayManager::getSingletonPtr()->getByName(
                (m_result == RESULT_SUCCESS) ? "Simulator/Success" : "Simulator/Failed");
        m_pOverlay->show();
    }

    // Detect if we must shutdown the application
    if (Engine::getSingletonPtr()->getMainWindow()->isClosed() || pController->wasKeyPressed(VKEY_EXIT))
    {
        Engine::getSingletonPtr()->getGameStateManager()->popState();
    }

    // Map switching
    else if (pController->wasKeyPressed(VKEY_RESET_TASK))
    {
        prevReward  = 0.0f;
        prevFalling = true;

        resetTask();
    }
}
