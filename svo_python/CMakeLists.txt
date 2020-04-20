

find_package(Python3 REQUIRED Interpreter Development)

if(DEFINED pybind11_SUBDIR)
    add_subdirectory(${pybind11_SUBDIR} pybind11)
else()
    find_package(pybind11 REQUIRED)
endif()


exec_program(${Python3_EXECUTABLE}
             ARGS "-c \"from os.path import join, split; import torch; print(split(torch.__file__)[0])\""
             OUTPUT_VARIABLE TORCH_DIR
             RETURN_VALUE TORCH_NOT_FOUND
            )
set(TORCH_INCLUDE_DIR ${TORCH_DIR}/include)
set(TORCH_LIB_DIR ${TORCH_DIR}/lib)
set(TORCH_LIBRARIES
    ${TORCH_LIB_DIR}/libtorch.so
    ${TORCH_LIB_DIR}/libc10.so
    ${TORCH_LIB_DIR}/libc10_cuda.so
    ${TORCH_LIB_DIR}/libtorch_python.so
)


add_library(torch_svo MODULE
    src/svo.cpp
    src/cameras.cpp     include/cameras.h
    src/handlers.cpp    include/handlers.h
    src/torch_frame.cpp include/torch_frame.h
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
    ${TORCH_LIBRARIES}
)
set_target_properties(torch_svo PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                           SUFFIX "${PYTHON_MODULE_EXTENSION}")
