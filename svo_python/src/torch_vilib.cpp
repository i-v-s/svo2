#include <torch/extension.h>
//#include <cuda.h>
//#include <cuda_runtime.h>
#include <iostream>
#include <torch_frame.h>
#include <vilib/feature_detection/fast/rosten/fast_cpu.h>
#include <vilib/feature_detection/fast/fast_gpu.h>


PYBIND11_MODULE(torch_vilib, m)
{
// data submodule
    auto m_data = m.def_submodule("data");
    py::class_<vilib::TorchFrame>(m_data, "Frame")
        .def(py::init<torch::Tensor, //const cv::Mat &,
             const int64_t,
             const std::size_t>())
        //.def("setName", &vilib::Subframe::setName)
        //.def("getName", &vilib::Subframe::getName)
        .def("pyramid_cpu", &vilib::TorchFrame::pyramid_cpu)
        .def("pyramid_gpu", &vilib::TorchFrame::pyramid_gpu)
        .def_readonly("pyramid", &vilib::TorchFrame::pyramid_);


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

    py::class_<vilib::FASTGPU>(m_detect, "FastDetectorGpu")
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
        .def("detect", py::overload_cast<const std::vector<std::shared_ptr<vilib::Subframe>> &>(&vilib::FASTGPU::detect))
        .def("get_points", &vilib::FASTGPU::getPoints)
        .def("display_features", &vilib::FASTGPU::displayFeatures);
}
