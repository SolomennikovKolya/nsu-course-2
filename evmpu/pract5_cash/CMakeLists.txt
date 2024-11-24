
cmake_minimum_required(VERSION 3.10)

project(CASH)

# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/SFML" "SFML_BIN")

add_executable(CashTest src/main.cpp)
target_compile_features(CashTest PUBLIC cxx_std_20)
# target_include_directories(CashTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/SFML/include")
# target_link_libraries(CashTest PRIVATE SFML SFML_BIN)

add_compile_options(-O1)
