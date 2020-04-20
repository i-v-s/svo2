#ifndef CAMERAS_H
#define CAMERAS_H
#include <pybind11/pybind11.h>
namespace py = pybind11;

void initCameras(py::module &m);
#endif // CAMERAS_H
