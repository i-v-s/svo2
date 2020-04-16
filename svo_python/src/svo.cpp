#include <svo/initialization.h>
#include <vikit/abstract_camera.h>
#include <vikit/pinhole_camera.h>
#include <svo/frame_handler_base.h>
#include <svo/frame_handler_mono.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;
PYBIND11_MODULE(torch_svo, m) {
    using namespace std;
    using namespace py;
    using namespace vk;
    auto m_init = m.def_submodule("init");
    class_<svo::initialization::KltHomographyInit>(m_init, "KltHomography")
        .def(init<shared_ptr<vilib::DetectorBaseGPU>>(), py::arg("detector"));
        //.def("setName", &vilib::Subframe::setName)
        //.def("getName", &vilib::Subframe::getName)
        //.def("pyramid_cpu", &vilib::TorchFrame::pyramid_cpu)
        //.def("pyramid_gpu", &vilib::TorchFrame::pyramid_gpu)
        //.def_readonly("pyramid", &vilib::TorchFrame::pyramid_);
    auto m_camera = m.def_submodule("camera");
    class_<AbstractCamera, shared_ptr<AbstractCamera>>(m_camera, "Camera");
    class_<PinholeCamera, shared_ptr<PinholeCamera>>(m_camera, "CameraPinhole")
         .def(init<double /*width*/, double /*height*/,
                       double /*fx*/, double /*fy*/, double /*cx*/, double /*cy*/,
                       double /*d0=0.0*/, double /*d1=0.0*/, double /*d2=0.0*/, double /*d3=0.0*/, double /*d4=0.0*/>(),
              py::arg("width"), py::arg("height"), py::arg("fx"), py::arg("fy"), py::arg("cx"), py::arg("cy"),
              py::arg("d0") = 0.0, py::arg("d1") = 0.0, py::arg("d2") = 0.0, py::arg("d3") = 0.0, py::arg("d4") = 0.0);

    auto m_handler = m.def_submodule("handler");
    class_<svo::FrameHandlerBase>(m_handler, "Handler");
    class_<svo::FrameHandlerMono>(m_handler, "HandlerMono")
         .def(init<shared_ptr<AbstractCamera> /*cam*/, shared_ptr<vilib::DetectorBaseGPU> /*detector*/>(),
              py::arg("camera"), py::arg("detector"));
}
