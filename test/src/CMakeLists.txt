include(${PROJECT_SOURCE_DIR}/cmake.macros)
subdirlist(children_dirs ${CMAKE_CURRENT_SOURCE_DIR})

foreach(child ${children_dirs})
    get_filename_component(module ${child} NAME)
    file(GLOB source_files_${module} "${CMAKE_SOURCE_DIR}/src/${module}/${module}.cpp")
    if (NOT "${source_files_${module}}" STREQUAL "")
      include_directories("${PROJECT_SOURCE_DIR}/include")
      include_directories("${PROJECT_BINARY_DIR}/include")
      add_library(lib_test_${module} SHARED ${source_files_${module}})
      target_link_libraries(lib_test_${module} pthread lib_kvp_store lib_salto_ipc lib_kvp_app rt)
      add_dependencies(lib_test_${module} json boost)
      file(GLOB test_files_${module} "${child}/*.cpp")
      add_executable(gtest_${module} ${test_files_${module}})
      add_dependencies(build_and_test gtest_${module})
      target_link_libraries(
        gtest_${module}
        lib_test_${module}
        gtest_main)
        gtest_discover_tests(gtest_${module} DISCOVERY_MODE PRE_TEST)
    else()
        message("no source files discovered for ${module} module")
    endif()
endforeach()