include(${PROJECT_SOURCE_DIR}/cmake.macros)
subdirlist(children_dirs ${CMAKE_CURRENT_SOURCE_DIR})

foreach(child ${children_dirs})
    get_filename_component(module ${child} NAME)
    add_subdirectory(${module})
endforeach()