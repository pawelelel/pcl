add_library(pcl
        pcl.c
        pcl_ascii.c
        pcl_Unicode.c
        queue.c
)

target_include_directories(pcl
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(pcl::pcl ALIAS pcl)
