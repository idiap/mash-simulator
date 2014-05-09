# Setup the search paths
include_directories(${MASH_SIMULATOR_SOURCE_DIR}/dependencies)

# List the source files of mash-utils
set(SRCS errors.cpp
         arguments_list.cpp
         commands_serializer.cpp
         data_buffer.cpp
         outstream.cpp
         data_reader.cpp
         data_writer.cpp
         random_number_generator.cpp
         stringutils.cpp
)

add_library(mash-utils SHARED ${SRCS})
set_target_properties(mash-utils PROPERTIES COMPILE_FLAGS "-fPIC")
