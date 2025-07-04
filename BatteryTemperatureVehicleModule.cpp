#include "LIN.h"
#include "CAN.h"
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <thread>

// === Configuration ===
constexpr uint32_t LIN_HW_ADDR = 0xFF000040;
constexpr uint32_t CAN_HW_ADDR = 0xFF000020;
constexpr uint32_t LIN_CONFIG = LIN_BAUD_RATE_9600 | LIN_START_BITS_1 | LIN_STOP_BITS_1 | LIN_DATA_BITS_8 |
                                LIN_PARITY_NONE | LIN_NO_FLOW_CONTROL | LIN_MODE_LEADER;
constexpr uint32_t CAN_CONFIG = CAN_BAUD_RATE_100K | CAN_FORMAT_11BIT;
constexpr int POLL_INTERVAL_MS = 500;

// === Utility Macros ===
#define SLEEP_MS(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#define TIME_NOW_S() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()

// === CAN ISR ===
void can_new_packet_isr(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len) {
    if(type == CAN_RTR_FRAME) {
        switch(id) {
            case CAN_AVG_TEMPERATURE_11_SENSOR_ID: {
                uint8_t avg_temp = data[0];
                can_send_new_packet(CAN_AVG_TEMPERATURE_11_SENSOR_ID, CAN_DATA_FRAME, &avg_temp, 1);
                break;
            }
            case CAN_CURRENT_TEMP_11_SENSOR_ID: {
                uint8_t current_temp = data[0];
                can_send_new_packet(CAN_CURRENT_TEMP_11_SENSOR_ID, CAN_DATA_FRAME, &current_temp, 1);
                break;
            }
            case CAN_TIME_11_SENSOR_ID: {
                uint32_t now = (uint32_t)TIME_NOW_S();
                can_send_new_packet(CAN_TIME_11_SENSOR_ID, CAN_DATA_FRAME, (uint8_t*)&now, sizeof(now));
                break;
            }
            default:
                break;
        }
    }
    can_clear_rx_packet_interrupt();
}

// === LIN ISR ===
void lin_rx_isr(uint8_t id, uint8_t *data, uint8_t len) {
    if(id == LIN_AVG_TEMP_SENSOR_ID) {
        printf("Average Temperature data: %d\n", data[0]);
    } else if(id == LIN_CURRENT_TEMP_SENSOR_ID) {
        printf("Current Temperature data: %d\n", data[0]);
    }
    lin_clear_frame_resp_interrupt();
}

// === Initialization ===
void init_lin() {
    lin_write_config(LIN_HW_ADDR, LIN_CONFIG);
    lin_add_frame_resp_interrupt(lin_rx_isr);
}

void init_can() {
    can_write_config(CAN_HW_ADDR, CAN_CONFIG);
    can_add_rx_packet_interrupt(can_new_packet_isr);
}

// === Main Logic ===
void poll_and_request() {
    uint32_t last_avg_temp_request_time = 0;
    uint32_t last_current_temp_request_time = 0;
    while(true) {
        lin_write_frame_header(LIN_AVG_TEMP_SENSOR_ID);
        last_avg_temp_request_time = (uint32_t)TIME_NOW_S();

        lin_write_frame_header(LIN_CURRENT_TEMP_SENSOR_ID);
        last_current_temp_request_time = (uint32_t)TIME_NOW_S();

        SLEEP_MS(POLL_INTERVAL_MS);
    }
}

int main(int argc, char **argv) {
    init_lin();
    init_can();
    poll_and_request();
    return 0;
}
