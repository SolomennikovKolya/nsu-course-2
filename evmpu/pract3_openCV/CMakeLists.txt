cmake_minimum_required(VERSION 3.10)

project(DisplayImage)

set(OpenCV_DIR C:/Software/opencv/build)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} capt_camera.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} )
