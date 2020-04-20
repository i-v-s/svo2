#include <vikit/abstract_camera.h>
#include <vikit/pinhole_camera.h>

#include "cameras.h"

void initCameras(py::module &m)
{
    using namespace vk;
    auto m_camera = m.def_submodule("camera");
    py::class_<AbstractCamera, shared_ptr<AbstractCamera>>(m_camera, "Camera");
    py::class_<PinholeCamera, shared_ptr<PinholeCamera>, AbstractCamera>(m_camera, "CameraPinhole")
         .def(py::init<double /*width*/, double /*height*/,
                       double /*fx*/, double /*fy*/, double /*cx*/, double /*cy*/,
                       double /*d0=0.0*/, double /*d1=0.0*/, double /*d2=0.0*/, double /*d3=0.0*/, double /*d4=0.0*/>(),
              py::arg("width"), py::arg("height"), py::arg("fx"), py::arg("fy"), py::arg("cx"), py::arg("cy"),
              py::arg("d0") = 0.0, py::arg("d1") = 0.0, py::arg("d2") = 0.0, py::arg("d3") = 0.0, py::arg("d4") = 0.0);
}
