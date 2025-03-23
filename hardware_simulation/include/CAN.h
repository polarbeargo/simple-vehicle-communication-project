#ifndef PROJECT_CAN_H
#define PROJECT_CAN_H

#include <cstdint>

extern "C" {

// | CAN FORMAT: 2 | CAN SPEED: 4 |

#define CAN_BAUD_RATE_100K 0x00
#define CAN_BAUD_RATE_125K 0x01
#define CAN_BAUD_RATE_250K 0x02
#define CAN_BAUD_RATE_500K 0x03
#define CAN_BAUD_RATE_1M   0x04

#define CAN_FORMAT_11BIT 0x08
#define CAN_FORMAT_29BIT 0x09

#define CAN_HARDWARE_REGISTER 0xFF000050

#define CAN_AVG_TEMPERATURE_11_SENSOR_ID 0x14f
#define CAN_CURRENT_TEMP_11_SENSOR_ID 0x15f
#define CAN_TIME_11_SENSOR_ID 0x18f

enum CAN_FRAME_TYPES {
    CAN_DATA_FRAME = 0,
    // remote transmission request
    CAN_RTR_FRAME = 1
};

void can_write_config(uint32_t reg, uint32_t value);
int can_add_filter(uint8_t idx, uint16_t mask, uint16_t filter);

int can_send_new_packet(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len);

void can_add_rx_packet_interrupt(void (*callback)(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len));
void can_clear_rx_packet_interrupt();

};
#endif //PROJECT_CAN_H
