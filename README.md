# Simple Vehicle Communication Project 

Create 3 different vehicle modules to represent communication in vehicle
1. SimpleTemperatureMonitor
1.1 Read the Temperature value over i2c
1.2 Responds to LIN frames with the Avg temperature or current temperature
2. BatteryTemperatureVehicleModule
2.1 Sends Temperature requests to the SimpleTemperatureMonitor over LIN
2.2 Responds to CAN RTR frames the for the current temp, avg temp, and current time in seconds
3. DataLoggingVehicleModue
3.1 Sends CAN RTR frames to the BatteryTemperatureVehicleModule
3.2 Reads the CAN Data frames with the current temp, avg temp, and time stamp
3.3 Saves the data to the SPI Flash on chip select 1


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

## Project Instructions

This section should contain all the student deliverables for this project.

## Built With

* [Item1](www.item1.com) - Description of item
* [Item2](www.item2.com) - Description of item
* [Item3](www.item3.com) - Description of item

Include all items used to build project.

## License
[License](../LICENSE.md)
