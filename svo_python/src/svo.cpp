#include <svo/initialization.h>
#include <pybind11/pybind11.h>
#include <vilib/feature_detection/fast/rosten/fast_cpu.h>
#include <vilib/feature_detection/fast/fast_gpu.h>
#include <torch_frame.h>

#include "cameras.h"
#include "handlers.h"
#include "visualize.h"

using namespace std;
using namespace svo;

PYBIND11_MODULE(torch_svo, m) {
    using namespace py;
    using namespace vk;
    auto m_data = m.def_submodule("data");
    py::class_<TorchFrame>(m_data, "Frame")
        .def(py::init<torch::Tensor, //const cv::Mat &,
             const int64_t,
             const std::size_t>())
        //.def("setName", &vilib::Subframe::setName)
        //.def("getName", &vilib::Subframe::getName)
        .def("pyramid_cpu", &TorchFrame::pyramid_cpu)
        .def("pyramid_gpu", &TorchFrame::pyramid_gpu)
        .def_readonly("pyramid", &TorchFrame::pyramid_);

    py::class_<vilib::Subframe, std::shared_ptr<vilib::Subframe>>(m_data, "Subframe");
    py::class_<cv::Mat>(m_data, "Mat");

// detect submodule
    auto m_detect = m.def_submodule("detect");
    py::enum_<vilib::fast_score>(m_detect, "FastScore")
        .value("SUM_OF_ABS_DIFF_ALL", vilib::SUM_OF_ABS_DIFF_ALL)
        .value("SUM_OF_ABS_DIFF_ON_ARC", vilib::SUM_OF_ABS_DIFF_ON_ARC)
        .value("MAX_THRESHOLD", vilib::MAX_THRESHOLD)
        .export_values();

    py::class_<vilib::DetectorBase::FeaturePoint>(m_detect, "FeaturePoint");
    py::class_<vilib::DetectorBaseGPU, shared_ptr<vilib::DetectorBaseGPU>>(m_detect, "DetectorGPU");
    py::class_<vilib::rosten::FASTCPU<true>>(m_detect, "FastDetectorCpuGrid")
        .def(py::init<
            const std::size_t/* image_width*/,
            const std::size_t/* image_height*/,
            const std::size_t/* cell_size_width*/,
            const std::size_t/* cell_size_height*/,
            const std::size_t/* min_level*/,
            const std::size_t/* max_level*/,
            const std::size_t/* horizontal_border*/,
            const std::size_t/* vertical_border*/,
            const float/* threshold*/,
            const int/* min_arc_length*/,
            const vilib::fast_score /*score*/>())
        .def("reset", &vilib::rosten::FASTCPU<true>::reset)
        .def("detect", &vilib::rosten::FASTCPU<true>::detect)
        .def("get_points", &vilib::rosten::FASTCPU<true>::getPoints)
        .def("display_features", &vilib::rosten::FASTCPU<true>::displayFeatures);

    py::class_<vilib::FASTGPU, vilib::DetectorBaseGPU, shared_ptr<vilib::FASTGPU>>(m_detect, "FastDetectorGpu")
        .def(py::init<
            const std::size_t /*image_width*/,
            const std::size_t /*image_height*/,
            const std::size_t /*cell_size_width*/,
            const std::size_t /*cell_size_height*/,
            const std::size_t /*min_level*/,
            const std::size_t /*max_level*/,
            const std::size_t /*horizontal_border*/,
            const std::size_t /*vertical_border*/,
            const float /*threshold*/,
            const int /*min_arc_length*/,
            vilib::fast_score /*score*/>())
        .def("reset", &vilib::FASTGPU::reset)
        .def("detect", py::overload_cast<const std::vector<vilib::Subframe> &>(&vilib::FASTGPU::detect))
        .def("get_points", &vilib::FASTGPU::getPoints)
        .def("display_features", &vilib::FASTGPU::displayFeatures);

    auto m_init = m.def_submodule("init");
    class_<svo::initialization::KltHomographyInit>(m_init, "KltHomography")
        .def(init<shared_ptr<vilib::DetectorBaseGPU>>(), py::arg("detector"));
        //.def("setName", &vilib::Subframe::setName)
        //.def("getName", &vilib::Subframe::getName)
        //.def("pyramid_cpu", &vilib::TorchFrame::pyramid_cpu)
        //.def("pyramid_gpu", &vilib::TorchFrame::pyramid_gpu)
        //.def_readonly("pyramid", &vilib::TorchFrame::pyramid_);

    initCameras(m);
    initHandlers(m);
    initVisualize(m);
}
