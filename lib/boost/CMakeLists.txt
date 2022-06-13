
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME module)
message("building ${module} module")


set(BOOST_INCLUDE_INSTALL_DIR "${PROJECT_BINARY_DIR}/include/boost")
set(BOOST_HEADER_FILES "${PROJECT_BINARY_DIR}/include/boost/interprocess/message_queue.hpp")

add_custom_target(create_boost_include_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BOOST_INCLUDE_INSTALL_DIR})

add_custom_command(OUTPUT ${BOOST_HEADER_FILES}
  COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/boost_pkg.tar.gz"
  COMMAND ${CMAKE_COMMAND} -E touch ${BOOST_HEADER_FILES}
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/include/boost"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/boost_pkg.tar.gz"
  DEPENDS create_boost_include_dir
  COMMENT "Unpacking boost_pkg.tar.gz"
  VERBATIM
)

add_custom_target(boost_untar DEPENDS ${BOOST_HEADER_FILES})

add_library(boost INTERFACE)
add_dependencies(boost boost_untar)
target_include_directories(boost INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/include/boost")
target_link_libraries(boost INTERFACE ${BOOST_LIBRARIES})