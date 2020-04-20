#ifndef HANDLERS_H
#define HANDLERS_H
#include <pybind11/pybind11.h>
namespace py = pybind11;

void initHandlers(py::module &m);
#endif // HANDLERS_H
