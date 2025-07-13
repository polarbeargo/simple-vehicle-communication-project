# Simple Vehicle Communication Project 

Create 3 different vehicle modules to represent communication in vehicle
1. SimpleTemperatureMonitor
    - Read the Temperature value over i2c.
    - Responds to LIN frames with the Avg temperature or current temperature.
2. BatteryTemperatureVehicleModule
    - Sends Temperature requests to the SimpleTemperatureMonitor over LIN.
    - Responds to CAN RTR frames the for the current temp, avg temp, and current time in seconds.
3. DataLoggingVehicleModule
    - Sends CAN RTR frames to the BatteryTemperatureVehicleModule.
    - Reads the CAN Data frames with the current temp, avg temp, and time stamp.
    - Saves the data to the SPI Flash on chip select 1.


## Getting Started

To build this within the workspace
``` 
mkdir build && cd build
cmake ..
make
```

### Dependencies

CMake

## Testing

Explain the steps needed to run any automated tests

## Software Design Patterns

In this project, we utilize the `Hardware Abstraction Layer (HAL)` design pattern to separate hardware-specific implementations (registers, drivers, protocols) from the application logic which enables to run on a different device or adding new hardware or features, we only need to update the HAL implementation not the core logic. This allows for easier reading, testing, maintain and simulation of the vehicle modules without needing actual hardware with the following steps:

- Define abstract interfaces for each communication protocol (CAN, I2C, LIN, SPI).
- Implement these interfaces for each supported platform/devices.

- Use the interfaces in the application logic, not direct hardware calls.  

## Project Structure

- `hal/` for hardware abstraction implementations
- `include/` for interfaces and config
- `src/` for application logic
```
project-root/
│
├── include/
│   ├── CAN_HAL.h
│   ├── I2C_HAL.h
│   ├── LIN_HAL.h
│   ├── SPI_HAL.h
│   └── config.h
│
├── hal/
│   ├── CAN_HAL.cpp
│   ├── I2C_HAL.cpp
│   ├── SPI_HAL.cpp
│   └── LIN_HAL.cpp
│
├── src/
│   ├── BatteryTemperatureVehicleModule.cpp
│   ├── DataLoggingVehicleModule.cpp
│   └── SimpleSensorMonitor.cpp
│
├── hardware_simulation/
│   └── ... (existing simulation code)
│
└── CMakeLists.txt
```

## License
[License](../LICENSE.md)
