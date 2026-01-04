cmake_minimum_required(VERSION 3.30)
project(pcl C)

set(CMAKE_C_STANDARD 99)

add_library(pcl SHARED pcl.c
        pcl_Unicode.h
        pcl_ascii.h
        pcl_ascii.c
        pcl_Unicode.c
        queue.h
        queue.c)

target_include_directories(pcl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})