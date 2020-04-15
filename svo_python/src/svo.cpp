#include <svo/initialization.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(torch_svo, m) {
    auto m_init = m.def_submodule("init");
    py::class_<svo::initialization::KltHomographyInit>(m_init, "KltHomographyInit")
        .def(py::init<>());
        //.def("setName", &vilib::Subframe::setName)
        //.def("getName", &vilib::Subframe::getName)
        //.def("pyramid_cpu", &vilib::TorchFrame::pyramid_cpu)
        //.def("pyramid_gpu", &vilib::TorchFrame::pyramid_gpu)
        //.def_readonly("pyramid", &vilib::TorchFrame::pyramid_);



}
