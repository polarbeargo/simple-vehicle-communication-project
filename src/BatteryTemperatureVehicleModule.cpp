#include "config.h"
#include "LIN_HAL.h"
#include "CAN_HAL.h"
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <thread>

#define SLEEP_MS(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#define TIME_NOW_S() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()

static uint8_t latest_avg_temp = 0;
static uint8_t latest_current_temp = 0;

void can_new_packet_isr(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len) {
    auto& canBus = SimCANBus::instance();
    printf("[DEBUG] CAN ISR triggered: id=0x%X, type=%d, len=%d\n", id, type, len);
    if(type == CAN_RTR_FRAME) {
        switch(id) {
            case CAN_AVG_TEMPERATURE_11_SENSOR_ID:
                canBus.send(CAN_AVG_TEMPERATURE_11_SENSOR_ID, CAN_DATA_FRAME, &latest_avg_temp, 1);
                break;
            case CAN_CURRENT_TEMP_11_SENSOR_ID:
                canBus.send(CAN_CURRENT_TEMP_11_SENSOR_ID, CAN_DATA_FRAME, &latest_current_temp, 1);
                break;
            case CAN_TIME_11_SENSOR_ID: {
                uint32_t now = (uint32_t)TIME_NOW_S();
                canBus.send(CAN_TIME_11_SENSOR_ID, CAN_DATA_FRAME, (uint8_t*)&now, sizeof(now));
                break;
            }
            default:
                break;
        }
    }
    canBus.clearRxInterrupt();
}

void lin_rx_isr(uint8_t id, uint8_t *data, uint8_t len) {
    if(id == LIN_AVG_TEMP_SENSOR_ID && len >= 1) {
        printf("Average Temperature data: %d\n", data[0]);
    } else if(id == LIN_CURRENT_TEMP_SENSOR_ID && len >= 1) {
        printf("Current Temperature data: %d\n", data[0]);
    }
    LIN_HAL::clearFrameRespInterrupt();
}

void poll_and_request() {
    uint32_t last_avg_temp_request_time = 0;
    uint32_t last_current_temp_request_time = 0;
    while(true) {
        LIN_HAL::writeFrameHeader(LIN_AVG_TEMP_SENSOR_ID);
        LIN_HAL::writeFrameHeader(LIN_CURRENT_TEMP_SENSOR_ID);
        SLEEP_MS(POLL_INTERVAL_MS);
    }
}

int main(int argc, char **argv) {
    LIN_HAL::init(LIN_HW_ADDR, LIN_MASTER_CONFIG);
    LIN_HAL::setFrameRespISR(lin_rx_isr);

    auto& canBus = SimCANBus::instance();
    canBus.init(CAN_HARDWARE_REGISTER, CAN_CONFIG);
    canBus.setRxISR(can_new_packet_isr);

    poll_and_request();
    return 0;
}