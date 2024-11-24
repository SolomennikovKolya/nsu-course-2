cmake_minimum_required(VERSION 3.5)

project(gameOfLife)

# Main target
set(SOURCES src/main.cpp src/initial_parameters.cpp src/game_controller.cpp src/universe.cpp src/field.cpp)
set(HEADERS src/initial_parameters.h src/game_controller.h src/universe.h src/field.h)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# # Build googletest
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/googletest" "googletest")
# # Test target
# set(TEST_TARGET_NAME testGameOfLife)
# add_executable(${TEST_TARGET_NAME} test/test_game_of_life.cpp src/initial_parameters.cpp src/game_controller.cpp)
# target_include_directories(${TEST_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
# target_link_libraries(${TEST_TARGET_NAME} PRIVATE gtest gtest_main)

# # Настраиваем уровень логирования варнингов для компиляторов
# if (MSVC)
#     set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Обязательно чтобы работало на винде
#     target_compile_options(${PROJECT_NAME} PRIVATE /W4)
#     target_compile_options(${TEST_TARGET_NAME} PRIVATE /W4)
# else ()
#     target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
#     target_compile_options(${TEST_TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
# endif ()
