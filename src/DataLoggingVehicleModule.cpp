#include "SPI_HAL.h"
#include "CAN_HAL.h"
#include "CAN.h"
#include <cstring>
#include <thread>
#include <cstdio>
#include <config.h>
#include <cstdlib>

// SPI Flash definitions
#define SPI_FLASH_DATA_PTS_PER_PAGE  (SPI_FLASH_PAGE_SIZE / sizeof(SPI_FLASH_data_pt_t))
#define SPI_FLASH_PAGES_TOTAL        (SPI_FLASH_SZ / SPI_FLASH_PAGE_SIZE)

static SPI_FLASH_data_pt_t page_buffer[SPI_FLASH_DATA_PTS_PER_PAGE];
static int page_buffer_index = 0;

static uint8_t latest_avg_temp = 0;
static uint8_t latest_current_temp = 0;
static uint32_t latest_time = 0;

// CAN ISR
void can_packet_isr(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len) {
    printf("[DEBUG] CAN ISR triggered: id=0x%X, type=%d, len=%d\n", id, type, len);
    if(type == CAN_DATA_FRAME) {
        if(id == CAN_AVG_TEMPERATURE_11_SENSOR_ID && len >= 1) {
            latest_avg_temp = data[0];
            printf("[LOG] Received AVG TEMP: %u\n", latest_avg_temp);
        } else if(id == CAN_CURRENT_TEMP_11_SENSOR_ID && len >= 1) {
            latest_current_temp = data[0];
            printf("[LOG] Received CURRENT TEMP: %u\n", latest_current_temp);
        } else if(id == CAN_TIME_11_SENSOR_ID && len >= 4) {
            memcpy(&latest_time, data, sizeof(uint32_t));
            printf("[LOG] Received TIME: %u\n", latest_time);
            page_buffer[page_buffer_index].avg_temp = latest_avg_temp;
            page_buffer[page_buffer_index].current_temp = latest_current_temp;
            page_buffer[page_buffer_index].time = latest_time;
            printf("[LOG] Logged Data Point: AVG=%u, CUR=%u, TIME=%u, IDX=%d\n",
                latest_avg_temp, latest_current_temp, latest_time, page_buffer_index);
            page_buffer_index++;
        }
    }
    SimCANBus::instance().clearRxInterrupt();
}

#define SLEEP_NOW_MS(ms)   std::this_thread::sleep_for(std::chrono::milliseconds(ms))

int main(int argc, char **argv) {
    int port_offset = 0;
    if (argc > 1) port_offset = atoi(argv[1]);
    can_set_port(CAN_PORT_START + port_offset);

    // Initialize SPI and CAN using HAL
    SPI_HAL::init(SPI_HARDWARE_REGISTER, SPI_CLK_1MHZ | SPI_CS_1);

    auto& canBus = SimCANBus::instance();
    canBus.init(CAN_HARDWARE_REGISTER, CAN_CONFIG);
    canBus.setRxISR(can_packet_isr);
    canBus.addFilter(0, SENSOR_MASK, CAN_AVG_TEMPERATURE_11_SENSOR_ID);
    canBus.addFilter(1, SENSOR_MASK, CAN_CURRENT_TEMP_11_SENSOR_ID);
    canBus.addFilter(2, SENSOR_MASK, CAN_TIME_11_SENSOR_ID);

    uint32_t page_number = 0;

    while(true) {
        printf("[DEBUG] Sending CAN RTR frames...\n");
        canBus.send(CAN_AVG_TEMPERATURE_11_SENSOR_ID, CAN_RTR_FRAME, nullptr, 0);
        canBus.send(CAN_CURRENT_TEMP_11_SENSOR_ID, CAN_RTR_FRAME, nullptr, 0);
        canBus.send(CAN_TIME_11_SENSOR_ID, CAN_RTR_FRAME, nullptr, 0);
        printf("[DEBUG] Sending CAN RTR frames End...\n");

        if(page_buffer_index >= SPI_FLASH_DATA_PTS_PER_PAGE) {
            printf("[DEBUG] Writing page to SPI flash, page_number=%u\n", page_number);

            SPI_HAL::erasePage((uint8_t)page_number);
            SPI_HAL::writePage((uint8_t)page_number, page_buffer, SPI_FLASH_PAGE_SIZE);

            page_buffer_index = 0;
            page_number = (page_number + 1) % SPI_FLASH_PAGES_TOTAL;
        }

        SLEEP_NOW_MS(500);
    }
    return 0;
}
