cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME module)
message("building ${module} module")

set(NHOLMANN_INCLUDE_INSTALL_DIR "${PROJECT_BINARY_DIR}/include/json")
set(NLOHMANN_JSON_HEADER_FILES "${PROJECT_BINARY_DIR}/include/json/json.hpp")

add_custom_target(create_json_include_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${NHOLMANN_INCLUDE_INSTALL_DIR})

add_custom_command(OUTPUT ${NLOHMANN_JSON_HEADER_FILES}
  COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/json_pkg.tar.gz"
  COMMAND ${CMAKE_COMMAND} -E touch ${NLOHMANN_JSON_HEADER_FILES}
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/include/json"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/json_pkg.tar.gz"
  DEPENDS create_json_include_dir
  COMMENT "Unpacking json_pkg.tar.gz"
  VERBATIM
)

add_custom_target(json_untar DEPENDS ${NLOHMANN_JSON_HEADER_FILES})

add_library(json INTERFACE)
add_dependencies(json json_untar)
target_include_directories(json INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/include/json")
target_link_libraries(json INTERFACE ${JSON_LIBRARIES})