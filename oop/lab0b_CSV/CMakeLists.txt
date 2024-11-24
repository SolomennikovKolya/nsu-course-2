cmake_minimum_required(VERSION 3.5)

project(lab0a)

set(SOURCES main.cpp text_data.cpp csv_writer.cpp)
set(HEADERS text_data.h csv_writer.h)

add_executable(word_count ${SOURCES} ${HEADERS})

target_compile_features(word_count PUBLIC cxx_std_20)
