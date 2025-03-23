#ifndef PROJECT_I2C_H
#define PROJECT_I2C_H
#include <cstdint>

extern "C" {

#define I2C_CLK_RISING_EDGE 0x01
#define I2C_CLK_FALLING_EDGE 0x00

#define I2C_IDLE_LOW 0b0100
#define I2C_IDLE_HIGH 0b000

#define I2C_HOST      0b0000
#define I2C_SLAVE     0b1000

#define I2C_CLK_400KHZ 0b000000
#define I2C_CLK_200KHZ 0b010000
#define I2C_CLK_100KHZ 0b100000

void i2c_write_config(uint32_t mem_mapped_addr, uint32_t value);

int i2c_write_data(uint8_t bus_addr, uint8_t *data, uint8_t len);

int i2c_read_data(uint8_t bus_addr, uint8_t *data, uint8_t len);

//extern "C" {
//    void i2c_write_(uint32_t mem_mapped_addr, uint32_t value) {
//        i2c_write_config(mem_mapped_addr, value);
//    }
//    int i2c_write(uint8_t bus_addr, uint8_t *data, uint8_t len) {
//        return i2c_write_data(bus_addr, data, len);
//    }
//    int i2c_read(uint8_t bus_addr, uint8_t *data, uint8_t len) {
//        return i2c_read_data(bus_addr, data, len);
//    }
//};
};
#endif //PROJECT_I2C_H
