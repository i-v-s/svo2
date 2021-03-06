################################################################################
# user build settings

set(TRACE TRUE)
set(HAVE_G2O FALSE)
option(USE_ROS "" FALSE) # Set FALSE if you want to build this package without Catkin
set(DEBUG_OUTPUT FALSE) # Only relevant if build without ROS

################################################################################

project(svo_lib)
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

SET(CMAKE_VERBOSE_MAKEFILE OFF)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMakeModules/")

# Set definitions
IF(TRACE)
  ADD_DEFINITIONS(-DSVO_TRACE)
ENDIF()
IF(HAVE_G2O)
  ADD_DEFINITIONS(-DUSE_BUNDLE_ADJUSTMENT)
ENDIF()
IF(USE_ROS)
  ADD_DEFINITIONS(-DSVO_USE_ROS)
ENDIF()
IF(DEBUG_OUTPUT)
  ADD_DEFINITIONS(-DSVO_DEBUG_OUTPUT)
ENDIF()

# Add plain cmake packages 
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)
FIND_PACKAGE(Sophus REQUIRED)
FIND_PACKAGE(vilib REQUIRED)

if(USE_ROS)
  FIND_PACKAGE(catkin REQUIRED COMPONENTS roscpp roslib cmake_modules vikit_common vikit_ros)
  catkin_package(
      DEPENDS Eigen OpenCV Sophus Boost fast
      CATKIN_DEPENDS roscpp roslib vikit_common vikit_ros
      INCLUDE_DIRS include
      LIBRARIES svo
  )
else()
  find_package(vikit_common REQUIRED)
endif()

# Set link libraries
list(APPEND LINK_LIBS
    ${OpenCV_LIBS}
    Sophus::Sophus
    vilib::vilib
)

IF(USE_ROS)
  INCLUDE_DIRECTORIES(${catkin_INCLUDE_DIRS})
  LIST(APPEND LINK_LIBS ${catkin_LIBRARIES})
ELSE()
  LIST(APPEND LINK_LIBS vikit::common)
ENDIF()

# Set sourcefiles
LIST(APPEND SOURCEFILES
  #src/frame_handler_stereo.cpp
  src/frame_handler_mono.cpp
  src/frame_handler_base.cpp
  src/frame.cpp
  src/point.cpp
  src/map.cpp
  src/pose_optimizer.cpp
  src/initialization.cpp
  src/matcher.cpp
  src/reprojector.cpp
  src/feature_alignment.cpp
  #src/feature_detection.cpp
  src/depth_filter.cpp
  src/config.cpp
  src/sparse_img_align.cpp)

# Add g2o if available
IF(HAVE_G2O)
  FIND_PACKAGE(G2O REQUIRED)
  INCLUDE_DIRECTORIES(
    ${G2O_INCLUDE_DIR}
    /usr/include/suitesparse # for cholmod
  )
  LIST(APPEND LINK_LIBS
    ${G2O_CORE_LIBRARY} ${G2O_STUFF_LIBRARY} ${G2O_SOLVER_CHOLMOD} ${G2O_SOLVER_CSPARSE} ${G2O_SOLVER_DENSE}
    ${G2O_SOLVER_PCG} ${G2O_TYPES_SBA} cholmod cxsparse
  )
  LIST(APPEND SOURCEFILES src/bundle_adjustment.cpp)
ENDIF(HAVE_G2O)

# Create svo library
add_library(svo ${SOURCEFILES})

set_target_properties(svo PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
)
# Set build flags, set ARM_ARCHITECTURE environment variable on Odroid
# Set build flags. Set IS_ARM on odroid board as environment variable
SET(CMAKE_CXX_FLAGS "-Wall -D_LINUX -D_REENTRANT -march=native -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unknown-pragmas")
IF(DEFINED ENV{ARM_ARCHITECTURE})
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -march=armv7-a")
ELSE()
  target_compile_options(svo PUBLIC -mmmx -msse -msse -msse2 -msse3 -mssse3 -msse4.1 -mavx2)
  # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -msse -msse -msse2 -msse3 -mssse3")
ENDIF()

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops")


target_link_libraries(svo
    ${LINK_LIBS}
)

target_include_directories(svo
    PUBLIC include
)

################################################################################
# TESTS
SET(WITH_TESTS OFF)

if(WITH_TESTS)
    ADD_EXECUTABLE(test_feature_align test/test_feature_alignment.cpp)
    TARGET_LINK_LIBRARIES(test_feature_align svo)

    ADD_EXECUTABLE(test_pipeline test/test_pipeline.cpp)
    TARGET_LINK_LIBRARIES(test_pipeline svo)

    ADD_EXECUTABLE(test_matcher test/test_matcher.cpp)
    TARGET_LINK_LIBRARIES(test_matcher svo)

    ADD_EXECUTABLE(test_feature_detection test/test_feature_detection.cpp)
    TARGET_LINK_LIBRARIES(test_feature_detection svo)

    ADD_EXECUTABLE(test_depth_filter test/test_depth_filter.cpp)
    TARGET_LINK_LIBRARIES(test_depth_filter svo)

    ADD_EXECUTABLE(test_sparse_img_align test/test_sparse_img_align.cpp)
    TARGET_LINK_LIBRARIES(test_sparse_img_align svo)

    ADD_EXECUTABLE(test_pose_optimizer test/test_pose_optimizer.cpp)
    TARGET_LINK_LIBRARIES(test_pose_optimizer svo)
endif()
