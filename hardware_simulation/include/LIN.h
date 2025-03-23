#ifndef PROJECT_LIN_H
#define PROJECT_LIN_H
#include <cstdint>

extern "C" {

// LIN msg is
// | BREAK FIELD | SYNC FIELD | ID FIELD | DATA FIELD | CHECKSUM FIELD |
// |------------- HEADER ----------------|----------- DATA ------------|
// | 13 bits     | 8 bits     | 6 bits   | 8 bits    | 8 bits
// Since the header break and sync field would be handled by hardware,
// we only need to handle the ID, DATA and CHECKSUM field in software

// bits 0 - 1: Flow control
#define LIN_NO_FLOW_CONTROL              0b00
#define LIN_1_RTS_CTS                    0b01
// bits 2 - 3: Data bits
#define LIN_DATA_BITS_7                0b0100
#define LIN_DATA_BITS_8                0b1000
#define LIN_DATA_BITS_9                0b1100
// bits 4 - 5: Stop bits
#define LIN_STOP_BITS_1              0b000000
#define LIN_STOP_BITS_2              0b010000
// bits 6 - 7: Start bits
#define LIN_START_BITS_1           0b01000000
#define LIN_START_BITS_2           0b10000000
// bits 8 - 10: Parity
#define LIN_PARITY_NONE          0b1100000000
#define LIN_PARITY_EVEN          0b0100000000
#define LIN_PARITY_ODD           0b1000000000
// bits 10 - 13: Baud rate
#define LIN_BAUD_RATE_9600     0b010000000000
#define LIN_BAUD_RATE_19200    0b100000000000
#define LIN_BAUD_RATE_57600    0b110000000000
#define LIN_BAUD_RATE_115200  0b1000000000000
// bits 14 - 16: Mode
#define LIN_MODE_LEADER     0b010000000000000
#define LIN_MODE_FOLLOWER   0b110000000000000

#define LIN_AVG_TEMP_SENSOR_ID 0x01
#define LIN_CURRENT_TEMP_SENSOR_ID 0x02
#define LIN_CURRENT_TIME_ID        0x04
#define LIN_FW_VERSION_ID          0x06

int lin_write_config(uint32_t mem_mapped_addr, uint32_t value);

int lin_add_rx_frame_header_interrupt(void (*callback)(uint8_t id));
int lin_add_frame_resp_interrupt(void (*callback)(uint8_t id, uint8_t *data, uint8_t len));

void lin_clear_rx_frame_interrupt();
void lin_clear_frame_resp_interrupt();

int lin_write_response_data(uint8_t id, uint8_t *data, uint8_t len);

void lin_write_frame_header(uint8_t id);

};

#endif //PROJECT_LIN_H
