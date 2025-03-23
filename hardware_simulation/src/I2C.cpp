#include "I2C.h"
#include <random>
#include <cstring>

typedef union {
    struct {
        uint8_t host: 1;
        uint8_t start: 2;
        uint8_t stop: 2;
        uint8_t reserved: 8;
        uint8_t clk_pol: 2;
    } bits;
    uint32_t reg;
} I2C_CR;

typedef struct {
    uint8_t bus_addr_to_read;
    uint8_t tx_data[8];
    uint16_t previous_temp;
    std::random_device rd;
    std::mt19937 gen;
    I2C_CR reg;
    uint32_t desired_reg;
} I2C_t;


// Control Reg, intentionally initialized to 0
static I2C_t g_i2c;

static void init_random_temps() {
    g_i2c.gen = std::mt19937(g_i2c.rd());
    g_i2c.previous_temp = 6000;
}

static uint8_t get_random_temp() {
    std::uniform_int_distribution<> distrub((g_i2c.previous_temp - 24),
                                            (g_i2c.previous_temp + 50) > 12000 ? 12000 : g_i2c.previous_temp + 25);
    g_i2c.previous_temp = distrub(g_i2c.gen);
    return g_i2c.previous_temp / 100;
}

void i2c_write_config(uint32_t addr, uint32_t value) {
    // Write the configuration value to the I2C bus
    if(addr == 0xFF000030) {
        g_i2c.reg.reg = value;
    } else {
        g_i2c.reg.reg = 0;
    }
    g_i2c.desired_reg = I2C_CLK_100KHZ |
                        I2C_HOST |
                        I2C_IDLE_LOW |
                        I2C_CLK_RISING_EDGE;

//    g_i2c.clk_speed = I2C_CLK_100KHZ;
    g_i2c.bus_addr_to_read = 0;
    g_i2c.previous_temp = 0;
}

int i2c_write_data(uint8_t bus_addr, uint8_t *data, uint8_t len) {
    if(g_i2c.desired_reg == g_i2c.reg.reg) {
        g_i2c.bus_addr_to_read = bus_addr;
        memcpy(g_i2c.tx_data, data, len);
        return len;
    }
    return 0;
}

int i2c_read_data(uint8_t bus_addr, uint8_t *data, uint8_t len) {
    len = 0;
    if(g_i2c.previous_temp == 0) {
        init_random_temps();
    }

    if(bus_addr == g_i2c.bus_addr_to_read &&
       g_i2c.tx_data[0] == 0x00 &&
       g_i2c.desired_reg == g_i2c.reg.reg) {
        // This will intentionally allow the random temp to get hotter over time
        data[0] = get_random_temp();
        len = 1;
    }

    return len;
}
