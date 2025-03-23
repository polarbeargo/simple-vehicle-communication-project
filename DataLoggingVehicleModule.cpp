#include "SPI.h"
#include "CAN.h"
#include <cstring>
#include <thread>
#include <cstdio>

// 1. Configure the SPI controller
// --- 1.1 SPI clock 1MHz, CS1
// 2. Configure the CAN controller
// --- 2.1 Baud 100K, 11 bit format
// 3. Add the CAN RX ISR
// 4. Every 500ms send the CAN RTR frames to the CAN Bus
// --- 4.5 CAN_AVG_TEMPERATURE_11_SENSOR_ID
// --- 4.6 CAN_CURRENT_TEMP_11_SENSOR_ID
// --- 4.7 CAN_TIME_11_SENSOR_ID
// 5. Save the temperature results to the SPI Flash (See SPI Flash section below)

// SPI Flash Brief
// * The SPI Flash is on CS1, not CS0
// * The SPI Flash has a Page size of of 64 bytes defined as SPI_FLASH_PAGE_SIZE in SPI.h
// * The SPI Flash has a total size of 4096 bytes defined as SPI_FLASH_SZ in SPI.h
// * A SPI Flash must be written to in pages, not bytes
// * A SPI Flash will return an error if you attemp to write to a page that is not erased
// * The SPI Flash expects the MOSI data to look like the following
//               | Flash Command | Page Number | Data 1 | Data 2 | ... | Data 64 |
// * The available flash commands area (SPI.h):
// ** SPI_FLASH_CMD_WRITE
// ** SPI_FLASH_CMD_READ
// ** SPI_FLASH_CMD_ERASE



#define SPI_FLASH_DATA_PTS_PER_PAGE  (SPI_FLASH_PAGE_SIZE / sizeof(SPI_FLASH_data_pt_t))
#define SPI_FLASH_PAGES_TOTAL        (SPI_FLASH_SZ / SPI_FLASH_PAGE_SIZE)



void can_packet_isr(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len) {
    // we can not save the page in the isr, too slow
    // so you need to accomodate for this in the main loop

    // Do something with the CAN packet
    if(id == CAN_AVG_TEMPERATURE_11_SENSOR_ID && type == CAN_DATA_FRAME) {

        printf("Average temperature data: %d\n", data[0]);

    } //else if() ...
    else {
        printf("Unknown CAN packet received\n");
    }


    // Clear the can interrupt before exit isr:
}


#define SLEEP_NOW_MS(ms)   std::this_thread::sleep_for(std::chrono::milliseconds(ms))

int main(int argc, char **argv) {
    // Configure the SPI and CAN controllers;


    // Add the CAN RX ISR
    can_add_rx_packet_interrupt(can_packet_isr);

    while(true) {
        // Send the CAN RTR frames to the BatteryTemperatureVehicleModule every 500ms
        // Once a full SPI Flash page size worth of data is received, save it to the SPI Flash




    }
    return 0;
}
