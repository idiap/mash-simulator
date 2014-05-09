# Setup the search paths
include_directories(${MASH_SIMULATOR_SOURCE_DIR}/dependencies)

# List the source files of mash-appserver
set(SRCS interactive_application_server.cpp
         interactive_listener.cpp
)

add_library(mash-appserver SHARED ${SRCS})
target_link_libraries(mash-appserver mash-utils mash-network)
set_target_properties(mash-appserver PROPERTIES COMPILE_FLAGS "-fPIC")
