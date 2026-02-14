cmake_minimum_required(VERSION 3.19.0)

file(GLOB_RECURSE DRIVER_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

set(DRIVER_INCLUDE_DIRS 
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP/Components"
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP/B-L475E-IOT01"
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP/Components/Common"
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP/Components/hts221"
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP/Components/lis3mdl"
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP/Components/lps22hb"
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP/Components/lsm6dsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP/Components/mx25r6435f"
)

include_directories(${DRIVER_INCLUDE_DIRS})

add_library(bsp_drivers STATIC ${DRIVER_SOURCES})

target_link_libraries(bsp_drivers PUBLIC mbed-os)

target_include_directories(bsp_drivers PUBLIC ${DRIVER_INCLUDE_DIRS})
