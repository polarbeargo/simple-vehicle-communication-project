cmake_minimum_required(VERSION 3.22)
project(VehicleProject)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/hal)
include_directories(${PROJECT_SOURCE_DIR}/hardware_simulation/include)

link_directories(${PROJECT_SOURCE_DIR}/hardware_simulation/build)

set(HAL_SOURCES
    hal/CAN_HAL.cpp
    hal/LIN_HAL.cpp
    hal/I2C_HAL.cpp
    hal/SPI_HAL.cpp
)

add_executable(SimpleSensorMonitor src/SimpleSensorMonitor.cpp ${HAL_SOURCES})
add_executable(BatteryTemperatureVehicleModule src/BatteryTemperatureVehicleModule.cpp ${HAL_SOURCES})
add_executable(DataLoggingVehicleModule src/DataLoggingVehicleModule.cpp ${HAL_SOURCES})

target_link_libraries(SimpleSensorMonitor PRIVATE hardware)
target_link_libraries(BatteryTemperatureVehicleModule PRIVATE hardware)
target_link_libraries(DataLoggingVehicleModule PRIVATE hardware)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_HAL_gtest test/test_HAL_gtest.cpp ${HAL_SOURCES})
target_link_libraries(test_HAL_gtest PRIVATE hardware gtest gtest_main pthread)

add_test(NAME HAL_GTest COMMAND test_HAL_gtest)
