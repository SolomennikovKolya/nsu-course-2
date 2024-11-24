cmake_minimum_required(VERSION 3.5)

project(lab0a)

set(SOURCES src/main.cpp src/module1.cpp src/module2.cpp src/module3.cpp)
set(HEADERS includes/module1.h includes/module2.h includes/module3.h)

add_executable(projectEXE ${SOURCES} ${HEADERS})

# set_target_properties(
#     version PROPERTIES
#     CXX_STANDARD 11
#     CXX_STANDARD_REQUIRED ON
# )
