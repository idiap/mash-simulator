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
#include <ServerState.h>
#include <maps.h>
#include <goals/goals.h>
#include <teachers/teachers.h>

#include <Athena/Engine.h>
#include <Athena/Tasks/TaskManager.h>
#include <Athena/GameStates/GameStateManager.h>
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
#include <Athena-Math/Vector3.h>
#include <Athena-Math/RandomNumberGenerator.h>
#include <Ogre/OgreRoot.h>
#include <Ogre/OgreRenderTexture.h>
#include <Ogre/OgreRenderWindow.h>
#include <Ogre/OgreSceneManager.h>
#include <Ogre/OgreHardwarePixelBuffer.h>
#include <Ogre/OgreOverlayManager.h>


using namespace Athena;
using namespace Athena::Entities;
using namespace Athena::Graphics;
using namespace Athena::Graphics::Visual;
using namespace Athena::Physics;
using namespace Athena::Inputs;
using namespace Athena::Math;
using namespace Athena::Log;

using Ogre::HardwarePixelBufferSharedPtr;
using Ogre::Image;
using Ogre::PixelBox;
using Ogre::OverlayManager;
using Ogre::ResourceGroupManager;
using Ogre::Root;
using Ogre::TextureManager;
using Ogre::Viewport;


static const char* __CONTEXT__ = "Server State";


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

ServerState::ServerState(bool bEnableSecrets)
: m_pRenderTexture(0), m_pAvatar(0), m_pAvatarBody(0), m_pAvatarGhost(0), m_pOverlay(0),
  m_pTeacher(0), m_pMap(0), m_pGoal(0), m_bEnableSecrets(bEnableSecrets),
  m_result(RESULT_NONE), m_fReward(0.0f), m_strEvent(""), m_pCurrentView(0)
{
    m_texture = TextureManager::getSingleton().createManual("RttTex", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                            Ogre::TEX_TYPE_2D, RTT_WIDTH, RTT_HEIGHT, 0, Ogre::PF_A8R8G8B8, Ogre::TU_RENDERTARGET);
    m_pRenderTexture = m_texture->getBuffer()->getRenderTarget();
    m_pRenderTexture->setAutoUpdated(true);
}


ServerState::~ServerState()
{
    reset();
}


/************************************** METHODS ****************************************/

void ServerState::setup(const std::string& goal, const std::string& environment,
                        unsigned int globalSeed)
{
    assert(!goal.empty());
    assert(!environment.empty());
    assert(!m_pMap);
    assert(!m_pGoal);

    m_selectedMap = environment;
    m_selectedGoal = goal;

    resetTask();
}


void ServerState::reset()
{
    if (m_pOverlay)
    {
        m_pOverlay->hide();
        m_pOverlay = 0;
    }

    if (m_pMap)
    {
        if (m_bEnableSecrets || ((m_selectedGoal != "secret") && (m_selectedMap != "Secret")))
            Engine::getSingletonPtr()->getMainWindow()->removeViewport(1);

        m_pRenderTexture->removeViewport(0);
        Engine::getSingletonPtr()->getMainWindow()->removeViewport(0);

        delete m_pGoal;
        delete m_pMap;
        delete m_pTeacher;

        m_pAvatar     = 0;
        m_pMap        = 0;
        m_pGoal       = 0;
        m_pTeacher    = 0;
    }

    m_result   = RESULT_NONE;
    m_fReward  = 0.0f;
    m_strEvent = "";

    if (m_pCurrentView)
    {
        delete[] m_pCurrentView;
        m_pCurrentView = 0;
    }
}


void ServerState::resetTask()
{
    assert(!m_selectedGoal.empty());
    assert(!m_selectedMap.empty());

    reset();


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
    m_pAvatarBody->enableRotation(false);
    m_pAvatarBody->setCollisionGroup(GROUP_AVATAR);

    m_pAvatarGhost = new GhostObject("Ghost", m_pAvatar->getComponentsList());
    m_pAvatarGhost->setCollisionGroup(GROUP_GHOST);

    CompoundShape* pAvatarShape = new CompoundShape("Shape", m_pAvatar->getComponentsList());
    pAvatarShape->addCapsule(0.25f, 1.25f, PrimitiveShape::AXIS_Y, Vector3(0.0f, 1.75f * 0.5f, 0.0f));
    m_pAvatarBody->setCollisionShape(pAvatarShape);
    m_pAvatarGhost->setCollisionShape(pAvatarShape);

    // Attach the camera to the avatar
    Transforms* pCameraAxis = new Transforms("CameraTransforms", m_pAvatar->getComponentsList());
    pCameraAxis->translate(0.0f, 1.75f, 0.0f);

    Camera* pCamera = new Camera("Camera", m_pAvatar->getComponentsList());
    pCamera->setTransforms(pCameraAxis);
    pCamera->setNearClipDistance(0.1f);
    pCamera->setFarClipDistance(100.0f);
    pCamera->setFOVy(Degree(45.0f));
    pCamera->setAspectRatio(float(VIEW_WIDTH) / VIEW_HEIGHT);

    // Create one viewport, entire render target
    Viewport* pViewport = pCamera->createViewport(m_pRenderTexture, 0, 0.0f, 0.0f, float(VIEW_WIDTH) / RTT_WIDTH, float(VIEW_HEIGHT) / RTT_HEIGHT);
    pViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));
    pViewport->setClearEveryFrame(true);

    // Create one viewport, entire window
    if (m_bEnableSecrets || ((m_selectedGoal != "secret") && (m_selectedMap != "Secret")))
    {
        pViewport = pCamera->createViewport(Engine::getSingletonPtr()->getMainWindow());
        pViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));
    }

    m_pAvatarBody->getRigidBody()->proceedToTransform(
                            btTransform(toBullet(pMapBuilder->getStartOrientation()),
                                        toBullet(pMapBuilder->getStartPosition())));

    m_pAvatar->getTransforms()->setPosition(pMapBuilder->getStartPosition());
    m_pAvatar->getTransforms()->setOrientation(pMapBuilder->getStartOrientation());

    m_pGoal->finalize(m_pMap);


    m_pTeacher = createTeacher(m_selectedGoal, m_selectedMap, m_pMap, pCamera);
    if (m_pTeacher)
        m_pTeacher->update(pMapBuilder->getStartPosition(), pMapBuilder->getStartOrientation());

    delete pMapBuilder;


    m_result   = RESULT_NONE;
    m_fReward  = 0.0f;
    m_strEvent = "";

    if (m_pCurrentView)
    {
        delete[] m_pCurrentView;
        m_pCurrentView = 0;
    }
}


bool ServerState::performAction(tAction action, float elapsedMilliseconds)
{
    assert(m_pGoal);
    assert(m_pMap);

    if (m_pCurrentView)
    {
        delete[] m_pCurrentView;
        m_pCurrentView = 0;
    }

    if (m_result != RESULT_NONE)
        return false;

    if (m_pGoal->isInitialized() && m_pGoal->updateTimeout(elapsedMilliseconds))
    {
        m_result = RESULT_FAILED;
        return false;
    }

    if (m_pGoal->isInitialized() && !m_pGoal->isAvatarFalling())
    {
        m_pAvatarBody->getRigidBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        m_pAvatarBody->getRigidBody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

        // Change the orientation of the avatar if needed
        Quaternion orientation = m_pAvatar->getTransforms()->getWorldOrientation();

        if (action == ACTION_TURN_LEFT)
            m_pAvatar->getTransforms()->setOrientation(Quaternion(Degree(30.0f * 1e-3f * elapsedMilliseconds), Vector3::UNIT_Y) * orientation);
        else if (action == ACTION_TURN_RIGHT)
            m_pAvatar->getTransforms()->setOrientation(Quaternion(Degree(-30.0f * 1e-3f * elapsedMilliseconds), Vector3::UNIT_Y) * orientation);

        // Change the velocity of the avatar if needed
        Vector3 currentVelocity = m_pAvatarBody->getLinearVelocity();

        if (action == ACTION_GO_FORWARD)
            m_pAvatarBody->setLinearVelocity(Vector3(0.0f, currentVelocity.y, -2.0f));
        else if (action == ACTION_GO_BACKWARD)
            m_pAvatarBody->setLinearVelocity(Vector3(0.0f, currentVelocity.y, 2.0f));
        else
            m_pAvatarBody->setLinearVelocity(Vector3(0.0f, currentVelocity.y, 0.0f));
    }

    return true;
}


unsigned char* ServerState::getAvatarView(size_t &nbBytes)
{
    // Retrieve the current view if necessary
    if (!m_pCurrentView)
        retrieveCurrentView();

    nbBytes = VIEW_WIDTH * VIEW_HEIGHT * 3;

    return m_pCurrentView;
}


tAction ServerState::getTeacherAction()
{
    if (m_pTeacher)
        return m_pTeacher->nextAction();

    return ACTIONS_COUNT;
}


Mash::tActionsList ServerState::getNotRecommendedActions()
{
    if (m_pTeacher)
        return m_pTeacher->notRecommendedActions();

    return Mash::tActionsList();
}


bool ServerState::retrieveCurrentView()
{
    assert(!m_pCurrentView);

    if (m_pRenderTexture->getNumViewports() == 0)
        return false;

    m_pCurrentView = new unsigned char[VIEW_WIDTH * VIEW_HEIGHT * 3];

    HardwarePixelBufferSharedPtr ogrePixelBuffer = m_texture->getBuffer();

    Image::Box srcBox(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

    PixelBox dstBox(VIEW_WIDTH, VIEW_HEIGHT, 1, Ogre::PF_B8G8R8, m_pCurrentView);

    ogrePixelBuffer->blitToMemory(srcBox, dstBox);

    return true;
}


/************************ METHODS TO BE OVERRIDEN BY EACH STATE ************************/

void ServerState::enter()
{
}


void ServerState::exit()
{
}


void ServerState::pause()
{
}


void ServerState::resume()
{
}


void ServerState::process()
{
    m_result = m_pGoal->process(m_fReward, m_pTeacher);

    if (m_pTeacher)
    {
        m_pTeacher->update(m_pAvatar->getTransforms()->getWorldPosition(),
                           m_pAvatar->getTransforms()->getWorldOrientation());
    }

    if ((m_result != RESULT_NONE) && !m_pOverlay)
    {
        m_pOverlay = Ogre::OverlayManager::getSingletonPtr()->getByName(
                (m_result == RESULT_SUCCESS) ? "Simulator/Success" : "Simulator/Failed");
        m_pOverlay->show();
    }
}
