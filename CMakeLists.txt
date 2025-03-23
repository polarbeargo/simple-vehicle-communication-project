cmake_minimum_required(VERSION 3.22)
project(SimpleSensorMonitor)
project(BatteryTemperatureVehicleModule)
project(DataLoggingVehicleModule)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/hardware_simulation/include)

link_directories(${PROJECT_SOURCE_DIR}/hardware_simulation/build)

add_executable(SimpleSensorMonitor
        SimpleSensorMonitor.cpp)
add_executable(BatteryTemperatureVehicleModule
        BatteryTemperatureVehicleModule.cpp)
add_executable(DataLoggingVehicleModule
        DataLoggingVehicleModule.cpp)

target_link_libraries(SimpleSensorMonitor PRIVATE hardware)
target_link_libraries(BatteryTemperatureVehicleModule PRIVATE hardware)
target_link_libraries(DataLoggingVehicleModule PRIVATE hardware)
