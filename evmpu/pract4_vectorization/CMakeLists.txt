
cmake_minimum_required(VERSION 3.10)

project(matrix)

set(SOURCES src/main.cpp src/usual_matrix.cpp src/blas_matrix.cpp src/intrinsic_matrix.cpp)
set(HEADERS src/usual_matrix.h src/blas_matrix.h src/intrinsic_matrix.h)

add_executable(${PROJECT_NAME} ${SOURCES})

# OpenBLAS-0.3.25
# add_subdirectory(OpenBLAS)
# target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/OpenBLAS")

# find_package(BLAS)

# include(FindBLAS)
# target_compile_options(${PROJECT_NAME} PRIVATE ${BLAS_LINKER_FLAGS})
# target_link_libraries(${PROJECT_NAME} ${BLAS_LIBRARIES})
