#include <svo/feature.h>
#include <svo/frame_handler_base.h>
#include <svo/frame_handler_mono.h>
#include <vikit/abstract_camera.h>
#include <torch/extension.h>
#include <vilib/cuda_common.h>
#include "handlers.h"

using namespace std;
using namespace svo;

class FHMWrapper : public FrameHandlerMono
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    FHMWrapper(std::shared_ptr<vk::AbstractCamera> cam, std::shared_ptr<vilib::DetectorBaseGPU> detector, size_t pyramidLevels):
        FrameHandlerMono(cam, detector), pyramidLevels(pyramidLevels), camera(cam) {}

    void addImage(torch::Tensor tensor, double timestamp) {
        unique_ptr<Frame> frame;
        {
            size_t dims = tensor.dim();
            assert(dims == 3);
            int64_t height = tensor.size(dims - 3),
                   width = tensor.size(dims - 2),
                   channels = tensor.size(dims - 1);
            auto strides = tensor.strides();
            //cout << "strides: " << strides[0] << ", " << strides[1] << ", " << strides[2] << endl;
            if (strides[2] != 1 || strides[1] != channels) {
                tensor = tensor.contiguous();
                strides = tensor.strides();
                //cout << "strides: " << strides[0] << ", " << strides[1] << ", " << strides[2] << endl;
            }
            vilib::Subframe::MemoryType mt = vilib::Subframe::MemoryType::PAGED_HOST_MEMORY;
            auto device = tensor.device();
            if (device.is_cuda()) {
                CUDA_API_CALL(cudaSetDevice(device.index()));
                mt = vilib::Subframe::MemoryType::PITCHED_DEVICE_MEMORY;
            }
            vilib::Subframe sf(width, height, channels, mt, tensor.data_ptr(), strides[0]);
            frame = make_unique<Frame>(camera.get(), sf, timestamp, pyramidLevels);
        }
        FrameHandlerMono::addImage(move(frame), timestamp);
    }
private:
    size_t pyramidLevels;
    std::shared_ptr<vk::AbstractCamera> camera;
};

void initHandlers(py::module &m)
{
    auto m_handler = m.def_submodule("handler");
    py::class_<svo::FrameHandlerBase>(m_handler, "Handler");
    py::class_<FHMWrapper>(m_handler, "HandlerMono")
         .def(py::init<shared_ptr<vk::AbstractCamera> /*cam*/, shared_ptr<vilib::DetectorBaseGPU> /*detector*/, size_t>(),
              py::arg("camera"), py::arg("detector"), py::arg("levels") = 4)
         .def("add_image", &FHMWrapper::addImage,
              py::arg("tensor"), py::arg("timestamp") = 0)
         .def("start", &FHMWrapper::start);
}
