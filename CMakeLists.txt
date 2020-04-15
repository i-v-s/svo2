cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(svo_root)

include(CMakeHelpers.txt)

option(WITH_PYTHON "Build Python bindings" ON)


#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/torch_svo)

if(WITH_PYTHON)
    set(pybind11_SUBDIR ${CMAKE_CURRENT_LIST_DIR}/modules/pybind11)
endif()

add_subdirectory(modules/vikit/vikit_common ${CMAKE_BINARY_DIR}/vikit)
set(vikit_common_DIR ${CMAKE_BINARY_DIR}/vikit)
add_subdirectory(modules/vilib/visual_lib ${CMAKE_BINARY_DIR}/vilib)
set(vilib_DIR ${CMAKE_BINARY_DIR}/vilib)
add_subdirectory(svo ${CMAKE_BINARY_DIR}/svo)

if(WITH_PYTHON)
    add_subdirectory(svo_python)
    set_target_properties(torch_svo torch_vilib
        PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/torch_svo
    )
    configure_file(svo_python/root.py ${CMAKE_BINARY_DIR}/torch_svo/__init__.py COPYONLY)
endif()

set_target_properties(svo vikit_common vilib
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/torch_svo
)

#message(STATUS "Targets: ${BUILDSYSTEM_TARGETS}")

#print_target_properties(vilib::vilib)

#include(CMakePrintHelpers)
#cmake_print_properties(
#    TARGETS pybind11::module vikit::common vilib::vilib vilib::python
#    PROPERTIES INTERFACE_LINK_LIBRARIES BINARY_DIR
#)
#get_property(fancy_lib_location TARGET vikit::common PROPERTY LOCATION)
#message (STATUS "fancy_lib_location == $<TARGET_FILE:vikit_common>")
