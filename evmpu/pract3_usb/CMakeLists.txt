cmake_minimum_required(VERSION 3.10)

project(libusbTest)

# set(libusb_DIR C:/Software/libusb-1.0.26-binaries/libusb-MinGW-x64)
# find_package(libusb CONFIG REQUIRED)
# target_include_directories(main PRIVATE ${LIBUSB_INCLUDE_DIRS})
# target_link_libraries(main PRIVATE ${LIBUSB_LIBRARIES})

add_executable(${PROJECT_NAME} main.cpp)
