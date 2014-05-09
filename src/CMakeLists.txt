# List the header files
set(HEADERS ../include/FPSState.h
            ../include/SimulationServer.h
            ../include/Simulator.h
            ../include/ServerState.h
            ../include/DebugDrawer.h
            ../include/Declarations.h
            ../include/MapBuilder.h
            ../include/Map.h
            ../include/maps.h

            ../include/goals/Goal.h
            ../include/goals/GoalReachOneFlag.h
            ../include/goals/GoalReachTwoFlagsInOrder.h
            ../include/goals/GoalReachUniqueFlag.h
            ../include/goals/GoalAllYouCanEat.h
            ../include/goals/GoalEatBlackDisks.h
            ../include/goals/GoalFollowTheLight.h
            ../include/goals/GoalFollowTheArrow.h
            ../include/goals/GoalFollowTheLine.h
            ../include/goals/GoalFollowTheBlobs.h
            ../include/goals/GoalReachCorrectPillar.h
            ../include/goals/GoalReachCorrectObject.h
            ../include/goals/GoalSecret.h

            ../include/teachers/teachers.h
            ../include/teachers/Teacher.h
            ../include/teachers/TeacherReachOneFlagSingleRoom.h
            ../include/teachers/TeacherReachOneFlagTwoRooms.h
            ../include/teachers/TeacherReachCorrectTargetSingleRoom.h
            ../include/teachers/TeacherFollowTheLightLightRoom.h
            ../include/teachers/TeacherFollowTheArrowTShapedCorridor.h
            ../include/teachers/TeacherFollowTheLineLineRoom.h
            ../include/teachers/TeacherFollowTheBlobsInBlobsRoom.h
            ../include/teachers/TeacherEatAllTargets.h
)

# List the source files
set(SRCS main.cpp
         FPSState.cpp
         SimulationServer.cpp
         Simulator.cpp
         ServerState.cpp
         DebugDrawer.cpp
         Declarations.cpp
         MapBuilder.cpp
         Map.cpp
         maps.cpp

         goals/goals.cpp
         goals/Goal.cpp
         goals/GoalReachOneFlag.cpp
         goals/GoalReachTwoFlagsInOrder.cpp
         goals/GoalReachUniqueFlag.cpp
         goals/GoalAllYouCanEat.cpp
         goals/GoalEatBlackDisks.cpp
         goals/GoalFollowTheLight.cpp
         goals/GoalFollowTheArrow.cpp
         goals/GoalFollowTheLine.cpp
         goals/GoalFollowTheBlobs.cpp
         goals/GoalReachCorrectPillar.cpp
         goals/GoalReachCorrectObject.cpp
         goals/GoalSecret.cpp

         teachers/teachers.cpp
         teachers/Teacher.cpp
         teachers/TeacherReachOneFlagSingleRoom.cpp
         teachers/TeacherReachOneFlagTwoRooms.cpp
         teachers/TeacherReachCorrectTargetSingleRoom.cpp
         teachers/TeacherFollowTheLightLightRoom.cpp
         teachers/TeacherFollowTheArrowTShapedCorridor.cpp
         teachers/TeacherFollowTheLineLineRoom.cpp
         teachers/TeacherFollowTheBlobsInBlobsRoom.cpp
         teachers/TeacherEatAllTargets.cpp
)


# List the include paths
include_directories("${MASH_SIMULATOR_SOURCE_DIR}/include"
                    "${MASH_SIMULATOR_SOURCE_DIR}/dependencies"
                    "${MASH_SIMULATOR_SOURCE_DIR}/dependencies/include"
)

xmake_import_search_paths(ATHENA_FRAMEWORK)
xmake_import_search_paths(OGRE)


# Create and link the executable
xmake_create_executable(SIMULATOR simulator ${HEADERS} ${SRCS})
xmake_project_link(SIMULATOR ATHENA_FRAMEWORK OGRE)
target_link_libraries(simulator mash-utils mash-network mash-appserver)


# On OS X, create .app bundle
if (APPLE)
	set_property(TARGET simulator PROPERTY MACOSX_BUNDLE TRUE)

	set (CONTENTS_PATH "${XMAKE_BINARY_DIR}/bin/simulator.app/Contents")

    # Retrieve the version of each dynamic library needed by the application
    get_target_property(ATHENA_MATH_VERSION Athena-Math SOVERSION)
    get_target_property(ATHENA_CORE_VERSION Athena-Core SOVERSION)
    get_target_property(ATHENA_ENTITIES_VERSION Athena-Entities SOVERSION)
    get_target_property(ATHENA_GRAPHICS_VERSION Athena-Graphics SOVERSION)
    get_target_property(ATHENA_PHYSICS_VERSION Athena-Physics SOVERSION)
    get_target_property(ATHENA_INPUTS_VERSION Athena-Inputs SOVERSION)
    get_target_property(ATHENA_SCRIPTING_VERSION Athena-Scripting SOVERSION)
    get_target_property(ATHENA_FRAMEWORK_VERSION Athena-Framework SOVERSION)
    get_target_property(V8_VERSION v8 SOVERSION)

    # Symlink the Frameworks
	add_custom_command(TARGET simulator POST_BUILD
		COMMAND mkdir ARGS -p ${CONTENTS_PATH}/Frameworks
		COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/Ogre.framework ${CONTENTS_PATH}/Frameworks/
		COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/Cg.framework ${CONTENTS_PATH}/Frameworks/
	)

    # Symlink the configuration files
	add_custom_command(TARGET simulator POST_BUILD
		COMMAND mkdir ARGS -p ${CONTENTS_PATH}/Resources
		COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/athena.cfg ${CONTENTS_PATH}/Resources/
	)

	# Symlink the plugins
    add_custom_command(TARGET simulator POST_BUILD
        COMMAND mkdir ARGS -p ${CONTENTS_PATH}/Plugins
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/RenderSystem_GL.dylib ${CONTENTS_PATH}/Plugins/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/Plugin_CgProgramManager.dylib ${CONTENTS_PATH}/Plugins/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/Plugin_OctreeSceneManager.dylib ${CONTENTS_PATH}/Plugins/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/Plugin_ParticleFX.dylib ${CONTENTS_PATH}/Plugins/
    )

	# Symlink the dynamic libraries
    add_custom_command(TARGET simulator POST_BUILD
        COMMAND mkdir ARGS -p ${CONTENTS_PATH}/Libraries
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/libAthena-Math.${ATHENA_MATH_VERSION}.dylib ${CONTENTS_PATH}/Libraries/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/libAthena-Core.${ATHENA_CORE_VERSION}.dylib ${CONTENTS_PATH}/Libraries/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/libAthena-Entities.${ATHENA_ENTITIES_VERSION}.dylib ${CONTENTS_PATH}/Libraries/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/libAthena-Graphics.${ATHENA_GRAPHICS_VERSION}.dylib ${CONTENTS_PATH}/Libraries/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/libAthena-Physics.${ATHENA_PHYSICS_VERSION}.dylib ${CONTENTS_PATH}/Libraries/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/libAthena-Inputs.${ATHENA_INPUTS_VERSION}.dylib ${CONTENTS_PATH}/Libraries/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/libAthena-Framework.${ATHENA_FRAMEWORK_VERSION}.dylib ${CONTENTS_PATH}/Libraries/
        COMMAND ln ARGS -s -f ${XMAKE_BINARY_DIR}/bin/RenderSystem_GL.dylib ${CONTENTS_PATH}/Libraries/
    )
endif()


# Set the RPATH
if (APPLE)
    xmake_add_to_property(SIMULATOR LINK_FLAGS "-Wl,-rpath,@loader_path/../Frameworks,-rpath,@loader_path/../Libraries")
endif()
