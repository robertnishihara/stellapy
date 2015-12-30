cmake_minimum_required(VERSION 2.6)

add_subdirectory(3rdparty/Boost.NumPy)
add_subdirectory(3rdparty/ale_0_4)
include(${CMAKE_SOURCE_DIR}/cmake/boost-python.cmake)
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty/Boost.NumPy")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty/ale_0_4/src")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Boost COMPONENTS python REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(Numpy)

boost_python_module(alepy alepy/alepy.cpp)
target_link_libraries(alepy ale boost_numpy)
