
project(svo_root)
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

add_subdirectory(modules/pybind11)
add_subdirectory(modules/vikit/vikit_common)
add_subdirectory(modules/vilib/visual_lib)
set(vilib_DIR ${CMAKE_BINARY_DIR}/modules/vilib/visual_lib/)
add_subdirectory(svo)
