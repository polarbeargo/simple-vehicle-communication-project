#include "I2C_HAL.h"
#include "I2C.h"

void I2C_HAL::init(uint32_t addr, uint32_t config) {
    i2c_write_config(addr, config);
}

void I2C_HAL::write(uint8_t addr, uint8_t* data, uint8_t len) {
    i2c_write_data(addr, data, len);
}

int I2C_HAL::read(uint8_t addr, uint8_t* data, uint8_t len) {
    return i2c_read_data(addr, data, len);
}