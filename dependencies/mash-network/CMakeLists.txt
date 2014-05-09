# Setup the search paths
include_directories(${MASH_SIMULATOR_SOURCE_DIR}/dependencies)

# List the source files of mash-network
set(SRCS busy_listener.cpp
         client.cpp
         networkutils.cpp
         server.cpp
         server_listener.cpp
)

add_library(mash-network SHARED ${SRCS})
target_link_libraries(mash-network mash-utils)
set_target_properties(mash-network PROPERTIES COMPILE_FLAGS "-fPIC")
