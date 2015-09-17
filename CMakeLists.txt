cmake_minimum_required(VERSION 3.3)
project(sdr_power)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
message(STATUS "RTLSDR_DIR: " ${RTLSDR_DIR})
find_package(RTLSDR)
if (NOT RTLSDR_FOUND)
    message(FATAL_ERROR "RTLSDR not found")
else ()
    message(STATUS "Using RTLSDR from " ${RTLSDR_INCLUDE_DIR})
    include_directories(${RTLSDR_INCLUDE_DIR})
endif ()

find_package(PkgConfig)
find_package(LibUSB)
if(NOT LIBUSB_FOUND)
    message(FATAL_ERROR "LibUSB 1.0 required to compile rtl-sdr")
else()
    message(STATUS "Using LibUSB from " ${LIBUSB_INCLUDE_DIR})
    include_directories(${LIBUSB_INCLUDE_DIR})
endif()

find_library(M_LIB m)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES sdr-power.c convenience.c backend.c backend.h backends/rtlsdr.c backends/rtlsdr.h)
add_executable(sdr_power ${SOURCE_FILES})
target_link_libraries(sdr_power ${RTLSDR_LIBRARY} ${LIBUSB_LIBRARIES} ${M_LIB})