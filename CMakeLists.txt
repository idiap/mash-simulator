cmake_minimum_required(VERSION 2.6)
project(MASH_SIMULATOR)


##########################################################################################
# Setup the dependencies

set(ATHENA_FRAMEWORK_USE_DEFAULT_CONFIGURATION OFF)
set(OGRE_BUILD_PLUGIN_BSP FALSE)
set(OGRE_BUILD_PLUGIN_OCTREE TRUE)
set(OGRE_BUILD_PLUGIN_PCZ FALSE)
set(OGRE_BUILD_PLUGIN_PFX TRUE)
set(OGRE_BUILD_PLUGIN_CG TRUE)
set(CONFIG_RENDERSYSTEM_GL "\"RenderSystem_GL\",")


##########################################################################################
# XMake-related settings

# Set the output paths
if (NOT DEFINED XMAKE_BINARY_DIR)
    set(XMAKE_BINARY_DIR "${MASH_SIMULATOR_BINARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${XMAKE_BINARY_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${XMAKE_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${XMAKE_BINARY_DIR}/bin")
endif()

# Set the dependencies path
if (NOT DEFINED XMAKE_DEPENDENCIES_DIR)
    set(XMAKE_DEPENDENCIES_DIR "${MASH_SIMULATOR_SOURCE_DIR}/dependencies")
endif()


##########################################################################################
# XMake importation

if (NOT EXISTS "${XMAKE_DEPENDENCIES_DIR}/XMake/XMake.cmake")
    message(FATAL_ERROR
"Missing dependency: XMake
MASH Simulator requires the following dependencies:
   - XMake
   - Athena-Math
   - Athena-Core
   - Athena-Entities
   - Athena-Graphics
   - Athena-Physics
   - Athena-Inputs
   - Athena-Framework
   - UnitTest++
   - Ogre
   - Bullet
   - rapidjson
   - OIS
   - FreeImage
   - FreeType
   - ZLib
   - zziplib
They are provided as GIT submodules of this repository.
Did you forgot to execute the following commands?
   git submodule init
   git submodule update")
endif()

include("${XMAKE_DEPENDENCIES_DIR}/XMake/XMake.cmake")


##########################################################################################
# Process subdirectories

add_subdirectory(dependencies)
add_subdirectory(src)
add_subdirectory(config)
