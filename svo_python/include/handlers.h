#ifndef HANDLERS_H
#define HANDLERS_H
#include <pybind11/pybind11.h>
#include <svo/frame_handler_mono.h>
#include <torch/extension.h>


namespace py = pybind11;

class FHMWrapper : public svo::FrameHandlerMono
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    FHMWrapper(std::shared_ptr<vk::AbstractCamera> cam, std::shared_ptr<vilib::DetectorBaseGPU> detector, size_t pyramidLevels);

    void addImage(torch::Tensor tensor, double timestamp);
private:
    size_t pyramidLevels;
    std::shared_ptr<vk::AbstractCamera> camera;
};


void initHandlers(py::module &m);
#endif // HANDLERS_H
