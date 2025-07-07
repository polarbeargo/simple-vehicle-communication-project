#pragma once
#include <cstdint>

class I2C_HAL {
public:
    static void init(uint32_t addr, uint32_t config);
    static void write(uint8_t addr, uint8_t* data, uint8_t len);
    static int read(uint8_t addr, uint8_t* data, uint8_t len);
};