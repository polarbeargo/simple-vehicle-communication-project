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




typedef struct {
    uint8_t avg_temp;
    uint8_t current_temp;
    uint32_t timestamp;
} SPI_FLASH_data_pt_t;

static SPI_FLASH_data_pt_t page_buffer[SPI_FLASH_DATA_PTS_PER_PAGE];
static int page_buffer_index = 0;

static uint8_t latest_avg_temp = 0;
static uint8_t latest_current_temp = 0;
static uint32_t latest_time = 0;

void can_packet_isr(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len) {
    if(type == CAN_DATA_FRAME) {
        if(id == CAN_AVG_TEMPERATURE_11_SENSOR_ID && len >= 1) {
            latest_avg_temp = data[0];
        } else if(id == CAN_CURRENT_TEMP_11_SENSOR_ID && len >= 1) {
            latest_current_temp = data[0];
        } else if(id == CAN_TIME_11_SENSOR_ID && len >= 4) {
            memcpy(&latest_time, data, sizeof(uint32_t));
            page_buffer[page_buffer_index].avg_temp = latest_avg_temp;
            page_buffer[page_buffer_index].current_temp = latest_current_temp;
            page_buffer[page_buffer_index].timestamp = latest_time;
            page_buffer_index++;
        }
    }

    // Clear the can interrupt before exit isr:
    can_clear_rx_packet_interrupt();
}


#define SLEEP_NOW_MS(ms)   std::this_thread::sleep_for(std::chrono::milliseconds(ms))

int main(int argc, char **argv) {
    // Configure the SPI and CAN controllers;
    uint32_t spi_config = SPI_CLK_1MHZ | SPI_CS_1;
    spi_write_config(0xFF000010, spi_config);

    uint32_t can_config = CAN_BAUD_RATE_100K | CAN_FORMAT_11BIT;
    can_write_config(0xFF000020, can_config);

    // Add the CAN RX ISR
    can_add_rx_packet_interrupt(can_packet_isr);

    while(true) {
        // Send the CAN RTR frames to the BatteryTemperatureVehicleModule every 500ms
        // Once a full SPI Flash page size worth of data is received, save it to the SPI Flash




    }
    return 0;
}
