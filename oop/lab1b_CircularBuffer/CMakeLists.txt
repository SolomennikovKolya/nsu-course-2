# cmake_minimum_required(VERSION 3.5)

# project(CircularBuffer)

# set(SOURCES src/main.cpp src/circular_buffer.hpp)
# set(HEADERS src/circular_buffer.h)

# add_executable(circular_buffer ${SOURCES} ${HEADERS})

# target_compile_features(circular_buffer PUBLIC cxx_std_20)



# cmake_minimum_required(VERSION 3.14)
# project(CircularBuffer)

# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Автообновление библиотеки
# include(FetchContent)
# FetchContent_Declare(
#     googletest
#     URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# enable_testing()

# set(SOURCES src/main.cpp src/circular_buffer.hpp)
# set(HEADERS src/circular_buffer.h)
# add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# target_link_libraries(
#     ${PROJECT_NAME}
#     GTest::gtest_main)

# include(GoogleTest)
# gtest_discover_tests(hello_test)



cmake_minimum_required(VERSION 3.5)

project(CircularBuffer)

# Build googletest
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/googletest" "googletest")

# Main target
add_executable(circular_buffer src/main.cpp src/circular_buffer.hpp src/circular_buffer.h)
target_compile_features(circular_buffer PUBLIC cxx_std_20)

# Test target
add_executable(test_circular_buffer test/test_circular_buffer.cpp src/circular_buffer.hpp)

# Add src to the include directories for the test target (чтобы работал #include "circular_buffer.hpp")
target_include_directories(test_circular_buffer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Link test target against gtest libreries (подключение гтестов)
target_link_libraries(test_circular_buffer PRIVATE gtest gtest_main)

# Настраиваем уровень логирования варнингов для компиляторов
if (MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Обязательно чтобы работало на винде
    target_compile_options(circular_buffer PRIVATE /W4)
    target_compile_options(test_circular_buffer PRIVATE /W4)
else ()
    target_compile_options(circular_buffer PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(test_circular_buffer PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()
