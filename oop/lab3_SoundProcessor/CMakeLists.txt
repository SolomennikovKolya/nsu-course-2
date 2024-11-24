cmake_minimum_required(VERSION 3.10)

project(SoundProcessor)

set(SOURCES src/main.cpp)
set(HEADERS)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
