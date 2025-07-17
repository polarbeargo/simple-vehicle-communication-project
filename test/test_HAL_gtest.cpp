#include <gtest/gtest.h>
#include <cstdint>
#include "config.h"
#include "I2C_HAL.h"
#include "LIN_HAL.h"
#include "CAN_HAL.h"

static bool i2c_init_called = false;
static uint8_t last_i2c_write_addr = 0;
static uint8_t last_i2c_write_data = 0;
static uint8_t last_i2c_read_addr = 0;
static uint8_t last_i2c_read_val = 0;

static bool lin_init_called = false;
static uint8_t last_lin_response_id = 0;
static uint8_t last_lin_response_data = 0;

static bool can_init_called = false;
static uint32_t last_can_init_addr = 0;
static uint32_t last_can_init_config = 0;
static uint32_t last_can_send_id = 0;
static uint8_t last_can_send_data = 0;

extern "C" {
void i2c_write_config(uint32_t, uint32_t) { i2c_init_called = true; }
int i2c_write_data(uint8_t addr, uint8_t* data, uint8_t len) {
    last_i2c_write_addr = addr;
    last_i2c_write_data = data[0];
    return 0;
}
int i2c_read_data(uint8_t addr, uint8_t* data, uint8_t len) {
    last_i2c_read_addr = addr;
    data[0] = 123;
    last_i2c_read_val = data[0];
    return 1;
}
int lin_write_config(uint32_t, uint32_t) { lin_init_called = true; return 0; }
int lin_write_response_data(uint8_t id, uint8_t* data, uint8_t len) {
    last_lin_response_id = id;
    last_lin_response_data = data[0];
    return 0;
}
void can_write_config(uint32_t addr, uint32_t config) {
    can_init_called = true;
    last_can_init_addr = addr;
    last_can_init_config = config;
}
int can_send_new_packet(uint32_t id, CAN_FRAME_TYPES, uint8_t* data, uint8_t len) {
    last_can_send_id = id;
    last_can_send_data = data ? data[0] : 0;
    return 0;
}
void can_add_rx_packet_interrupt(void (*)(uint32_t, CAN_FRAME_TYPES, uint8_t*, uint8_t)) {}
void can_clear_rx_packet_interrupt() {}
}

TEST(HALTest, I2C_HAL_InitWriteRead) {
    i2c_init_called = false;
    I2C_HAL::init(0x10, 0x20);
    EXPECT_TRUE(i2c_init_called);

    uint8_t data = 42;
    I2C_HAL::write(0x55, &data, 1);
    EXPECT_EQ(last_i2c_write_addr, 0x55);
    EXPECT_EQ(last_i2c_write_data, 42);

    uint8_t read_val = 0;
    int ret = I2C_HAL::read(0x55, &read_val, 1);
    EXPECT_EQ(ret, 1);
    EXPECT_EQ(read_val, 123);
    EXPECT_EQ(last_i2c_read_addr, 0x55);
}

TEST(HALTest, LIN_HAL_InitSendResponse) {
    lin_init_called = false;
    LIN_HAL::init(0x11, 0x22);
    EXPECT_TRUE(lin_init_called);

    uint8_t data = 77;
    LIN_HAL::sendResponse(0x33, &data, 1);
    EXPECT_EQ(last_lin_response_id, 0x33);
    EXPECT_EQ(last_lin_response_data, 77);
}

TEST(HALTest, CAN_HAL_InitSend) {
    can_init_called = false;
    SimCANBus canBus;
    canBus.init(0x123, 0x456);
    EXPECT_TRUE(can_init_called);
    EXPECT_EQ(last_can_init_addr, 0x123);
    EXPECT_EQ(last_can_init_config, 0x456);

    uint8_t data = 88;
    canBus.send(0x789, CAN_DATA_FRAME, &data, 1);
    EXPECT_EQ(last_can_send_id, 0x789);
    EXPECT_EQ(last_can_send_data, 88);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}