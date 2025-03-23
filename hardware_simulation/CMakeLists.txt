cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)

project(hardware VERSION 1.0.0 DESCRIPTION "Hardware communication library")

set(SOURCE_FILES source/I2C.cpp
                 source/LIN.cpp
                 source/CAN.cpp
                 source/SPI.cpp)

set(API_HEADERS include/I2C.h
                include/LIN.h
                include/CAN.h
                include/SPI.h)

add_library(hardware SHARED ${SOURCE_FILES})

target_include_directories(hardware PRIVATE .)
target_include_directories(hardware PRIVATE source)
target_include_directories(hardware PRIVATE include)


#set_target_properties(hardware PROPERTIES PUBLIC_HEADER ${API_HEADERS})

include(GNUInstallDirs)
install(TARGETS hardware
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

