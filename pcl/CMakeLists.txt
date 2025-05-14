cmake_minimum_required(VERSION 3.30)
project(pcl C)

set(CMAKE_C_STANDARD 23)

add_library(pcl SHARED pcl.c)

target_include_directories(pcl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})