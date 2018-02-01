// This file is part of SVO - Semi-direct Visual Odometry.
//
// Copyright (C) 2014 Christian Forster <forster at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// SVO is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or any later version.
//
// SVO is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <svo/config.h>
#include <opencv2/opencv.hpp>

namespace svo {

Config::Config() :
    trace_name("svo"),
    trace_dir("/tmp"),
    n_pyr_levels(3),
    use_imu(false),
    core_n_kfs(3),
    map_scale(1.0),
    grid_size(20),
    init_min_disparity(50.0),
    init_min_tracked(50),
    init_min_inliers(40),
    klt_max_level(4),
    klt_min_level(2),
    reproj_thresh(2.0),
    poseoptim_thresh(2.0),
    poseoptim_num_iter(10),
    structureoptim_max_pts(20),
    structureoptim_num_iter(5),
    loba_thresh(2.0),
    loba_robust_huber_width(1.0),
    loba_num_iter(0),
    kfselect_mindist(0.12),
    triang_min_corner_score(20.0),
    triang_half_patch_size(4),
    subpix_n_iter(10),
    max_n_kfs(10),
    img_imu_delay(0.0),
    max_fts(120),
    quality_min_fts(50),
    quality_max_drop_fts(80)
{}

template <typename T>
void readParamIfExist(T& val, const cv:: FileStorage& fs, const char* name)
{
    auto n = fs[name];
    if(!n.isNone())
        n>>val;
}

template <>
void readParamIfExist(size_t& val, const cv:: FileStorage& fs, const char* name)
{
    auto n = fs[name];
    int v;
    if(!n.isNone())
    {
        n>>v;
        val = v;
    }
}

bool Config::loadConfigYml(const char* file)
{
    cv:: FileStorage fs(file, cv::FileStorage::READ);
    if(!fs.isOpened()) return false;
    readParamIfExist(useImu(), fs, "use_imu");
    readParamIfExist(coreNKfs(), fs, "core_n_kfs");
    readParamIfExist(mapScale(), fs, "map_scale");
    readParamIfExist(gridSize(), fs, "grid_size");
    readParamIfExist(initMinDisparity(), fs, "init_min_disparity");
    readParamIfExist(initMinTracked(), fs, "init_min_tracked");
    readParamIfExist(initMinInliers(), fs, "init_min_inliers");
    readParamIfExist(kltMaxLevel(), fs, "klt_max_level");
    readParamIfExist(kltMinLevel(), fs, "klt_min_level");
    readParamIfExist(reprojThresh(), fs, "reproj_thresh");
    readParamIfExist(poseOptimThresh(), fs, "poseoptim_thresh");
    readParamIfExist(poseOptimNumIter(), fs, "poseoptim_num_iter");
    readParamIfExist(structureOptimMaxPts(), fs, "structureoptim_max_pts");
    readParamIfExist(structureOptimNumIter(), fs, "structureoptim_num_iter");
    readParamIfExist(lobaThresh(), fs, "loba_thresh");
    readParamIfExist(lobaRobustHuberWidth(), fs, "loba_robust_huber_width");
    readParamIfExist(lobaNumIter(), fs, "loba_num_iter");
    readParamIfExist(kfSelectMinDist(), fs, "kfselect_mindist");
    readParamIfExist(triangMinCornerScore(), fs, "triang_min_corner_score");
    readParamIfExist(subpixNIter(), fs, "subpix_n_iter");
    readParamIfExist(maxNKfs(), fs, "max_n_kfs");
    readParamIfExist(imgImuDelay(), fs, "img_imu_delay");
    readParamIfExist(maxFts(), fs, "max_fts");
    readParamIfExist(qualityMinFts(), fs, "quality_min_fts");
    readParamIfExist(qualityMaxFtsDrop(), fs, "quality_max_drop_fts");
    return true;
}

Config& Config::getInstance()
{
    static Config instance; // Instantiated on first use and guaranteed to be destroyed
    return instance;
}

} // namespace svo

