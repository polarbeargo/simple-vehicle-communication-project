#pragma once
#include <cstdint>
#include "I2C.h"
#include "LIN.h"
#include "CAN.h"

// I2C Configuration
constexpr uint8_t  ADC_ADDR      = 0x20;
constexpr uint8_t  ADC_REG       = 0x00;
constexpr uint32_t I2C_HW_ADDR   = 0xFF000030;
constexpr uint32_t I2C_CONFIG    = I2C_CLK_100KHZ | I2C_HOST | I2C_IDLE_LOW | I2C_CLK_RISING_EDGE;

// LIN Configuration
constexpr uint32_t LIN_HW_ADDR   = 0xFF000040;
constexpr uint32_t LIN_CONFIG = LIN_BAUD_RATE_9600 | LIN_START_BITS_1 | LIN_STOP_BITS_1 | LIN_DATA_BITS_8 |
                          LIN_PARITY_NONE | LIN_NO_FLOW_CONTROL | LIN_MODE_FOLLOWER;

constexpr uint32_t LIN_MASTER_CONFIG = LIN_BAUD_RATE_9600 | LIN_START_BITS_1 | LIN_STOP_BITS_1 | LIN_DATA_BITS_8 |
                                LIN_PARITY_NONE | LIN_NO_FLOW_CONTROL | LIN_MODE_LEADER;
// CAN Configuration
constexpr uint32_t CAN_HW_ADDR   = 0xFF000020;
constexpr uint32_t CAN_CONFIG = CAN_BAUD_RATE_100K | CAN_FORMAT_11BIT;

// General
constexpr int SLEEP_DURATION_MS  = 1000;
constexpr int POLL_INTERVAL_MS = 500;

#define CAN_PORT_START 9000