/*
 * Wrapper class for camera frames
 * frame.cpp
 *
 * Copyright (C) 2019-2020 Balazs Nagy,
 * Robotics and Perception Group, University of Zurich
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef ROS_SUPPORT
#include <sensor_msgs/image_encodings.h>
#endif /* ROS_SUPPORT */
#include "torch_frame.h"
#include "vilib/preprocess/image_preprocessing.h"
#include "vilib/preprocess/pyramid.h"
#include "vilib/config.h"

std::size_t TorchFrame::last_id_ = 0;
std::mutex TorchFrame::last_id_mutex_;

TorchFrame::TorchFrame(
        torch::Tensor img,
        const int64_t timestamp_nsec,
        const std::size_t n_pyr_levels/*,
        cudaStream_t stream*/) :
    TorchFrame(timestamp_nsec, img.size(img.dim() - 2), img.size(img.dim() - 3), n_pyr_levels) {
    //std::cout << "dim() is " << img.dim() << std::endl;
    //std::cout << "Sizes " << img.sizes() << std::endl;
    //std::cout << "Size 0 " << img.size(0) << std::endl;
    //img.print();
    cv::Size size(img.size(img.dim() - 2), img.size(img.dim() - 3));
    size_t channels = img.size(img.dim() - 1);
    int type = 0;
    switch (channels) {
    case 1: type = CV_8UC1; break;
    case 3: type = CV_8UC3; break;
    case 4: type = CV_8UC4; break;
    default: throw std::invalid_argument("Wrong number of channels");
    }

    preprocess_image(cv::Mat(size, type, img.data_ptr()), pyramid_, 0/*stream*/);
}

#ifdef ROS_SUPPORT
TorchFrame::TorchFrame(const sensor_msgs::ImageConstPtr & msg,
             const std::size_t n_pyr_levels,
             cudaStream_t stream) :
  TorchFrame(msg->header.stamp.sec*1e9 + msg->header.stamp.nsec,
        msg->width,
        msg->height,
        n_pyr_levels) {
  preprocess_image(msg,pyramid_,stream);
}
#endif /* ROS_SUPPORT */

TorchFrame::TorchFrame(const int64_t timestamp_nsec,
             const std::size_t image_width,
             const std::size_t image_height,
             const std::size_t n_pyr_levels) :
  id_(getNewId()),
  timestamp_nsec_(timestamp_nsec),
  pyramid_pool(vilib::PyramidPool::get_pool(image_width, image_height, 1, n_pyr_levels, vilib::IMAGE_PYRAMID_MEMORY_TYPE))
{
    pyramid_pool->get(pyramid_);
}

std::size_t TorchFrame::getNewId(void) {
  std::lock_guard<std::mutex> lock(last_id_mutex_);
  return last_id_++;
}

TorchFrame::~TorchFrame(void) {
    // return the pyramid buffers
    pyramid_pool->release(std::move(pyramid_));
}

/*image_pyramid_descriptor_t TorchFrame::getPyramidDescriptor(void) const {
  image_pyramid_descriptor_t i;
  i.desc = PyramidPool::get_descriptor();
  for(std::size_t l=0;l<pyramid_.size();++l) {
    i.data[l] = pyramid_[l]->data_;
  }
  return i;
}*/

void TorchFrame::resizeFeatureStorage(std::size_t new_size) {
  // Note: we dont want to lose features during this function call
  assert((int)new_size >= px_vec_.cols());
  // Don't do anything if the column size is the same
  if((int)new_size == px_vec_.cols()) {
    return;
  }
  std::size_t uninitialized_cols = new_size - num_features_;
  // do the resizing
  px_vec_.conservativeResize(Eigen::NoChange, new_size);
  level_vec_.conservativeResize(new_size, Eigen::NoChange);
  level_vec_.tail(uninitialized_cols).setZero();
  score_vec_.conservativeResize(new_size, Eigen::NoChange);
  track_id_vec_.conservativeResize(new_size);
  track_id_vec_.tail(uninitialized_cols).setConstant(-1);
}

std::vector<cv::Mat> TorchFrame::pyramid_cpu()
{
    std::vector<cv::Mat> pyramid;
    cv::Mat image;
    pyramid_[0].copy_to(image);
    vilib::pyramid_create_cpu(image, pyramid, pyramid_.size(), false);
    return pyramid;
}

std::vector<vilib::Subframe> TorchFrame::pyramid_gpu()
{
    return pyramid_;
}
