#include <cstdint>
#include <thread>
#include <chrono>
#include <cstdio>
#include "config.h"
#include "I2C_HAL.h"
#include "LIN_HAL.h"

static uint8_t g_current_val = 0;
static uint32_t g_sum = 0;
static uint32_t g_count = 0;

void lin_rx_isr(uint8_t id) {
    if (id == LIN_AVG_TEMP_SENSOR_ID) {
        uint8_t avg = g_count ? (uint8_t)(g_sum / g_count) : 0;
        LIN_HAL::sendResponse(LIN_AVG_TEMP_SENSOR_ID, &avg, 1);
    } else if (id == LIN_CURRENT_TEMP_SENSOR_ID) {
        LIN_HAL::sendResponse(LIN_CURRENT_TEMP_SENSOR_ID, &g_current_val, 1);
    }
    LIN_HAL::clearInterrupt();
}

int main(int argc, char **argv) {
    I2C_HAL::init(I2C_HW_ADDR, I2C_CONFIG);
    LIN_HAL::init(LIN_HW_ADDR, LIN_CONFIG);

    LIN_HAL::setRxISR(lin_rx_isr);

    while (true) {
        // Write register address to ADC before reading
        uint8_t reg = ADC_REG;
        I2C_HAL::write(ADC_ADDR, &reg, 1);

        // Read ADC value via I2C HAL
        uint8_t temp = 0;
        if (I2C_HAL::read(ADC_ADDR, &temp, 1) == 1) {
            g_current_val = temp;
            g_sum += temp;
            g_count++;
        }

        printf("ADC Value: %d\n", g_current_val);

        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_DURATION_MS));
    }
}
