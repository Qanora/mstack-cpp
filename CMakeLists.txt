cmake_minimum_required(VERSION 3.10)

project(mstack LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# find_package (glog REQUIRED)
add_compile_options(-g)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC)

add_executable(mstack main.cpp)

target_link_libraries (mstack glog gflags)