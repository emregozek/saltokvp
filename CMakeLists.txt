cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(
  saltokvp
  VERSION 0.0.1
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/emregozek/saltokvp"
  LANGUAGES CXX)


set(ENABLE_SANITIZERS ON CACHE BOOL "Enable Sanitizers")
if (ENABLE_SANITIZERS)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address  -fno-omit-frame-pointer")
  set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
endif()

set(ENABLE_CLANG_TIDY ON CACHE BOOL "Add clang-tidy automatically to builds")
if (ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY NAMES "clang-tidy")
    if (CLANG_TIDY)
        message(STATUS "clang-tidy found: ${CLANG_TIDY}")
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
    else()
        message(AUTHOR_WARNING "clang-tidy not found!")
        set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE) # delete it
    endif()
endif()

option(BUILD_DOC "Build documentation" OFF)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

add_subdirectory(src)
add_subdirectory(lib)


option(ENABLE_TESTING "Enable the tests" ON)
if(ENABLE_TESTING)
  enable_testing()
  add_custom_target(build_and_test ${CMAKE_CTEST_COMMAND} -V)
  add_subdirectory(test)
endif()
