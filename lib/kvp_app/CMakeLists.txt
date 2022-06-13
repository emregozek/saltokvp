
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME module)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/include)

file(GLOB source_files_lib_${module} "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(lib_${module} SHARED ${source_files_lib_${module}})

