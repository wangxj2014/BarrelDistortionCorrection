cmake_minimum_required(VERSION 3.20)

project(pcl_test)
set(OpenCV_DIR /home/lab/wangxiujing/software/opencv-3.4.3/build)
find_package(OpenCV REQUIRED)
#add_executable(main undistort_img.cpp)
add_library(
        native-lib
        SHARED
        HelloWorld.cpp
)

add_executable(main undistort_img.cpp)
link_directories(${OpenCV_LIBRARY_DIRS})
target_link_libraries(main ${OpenCV_LIBS})
