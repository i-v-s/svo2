

find_package(Python3 REQUIRED Interpreter Development)

add_library(torch_svo MODULE
    src/svo.cpp
)
target_include_directories(torch_svo PRIVATE
    ${TORCH_INCLUDE_DIR}
    "${TORCH_INCLUDE_DIR}/torch/csrc/api/include"
    include
)
target_link_libraries(torch_svo PRIVATE
    Python3::Python
    pybind11::module
    svo
    vilib::vilib
    "/home/igor/src/cpp/build/svo-Debug/vilib/libvilib_python.so"
    #Eigen3::Eigen
    #CUDA::cudart
    ${TORCH_LIBRARIES}
)
set_target_properties(torch_svo PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                           SUFFIX "${PYTHON_MODULE_EXTENSION}")
configure_file(test.py test.py COPYONLY)
