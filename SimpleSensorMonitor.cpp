// Simple Sensor Monitor

// 1. Include the necessary header files
#include <cstdint>
#include <thread>
#include <chrono>
#include <cstdio>
#include "I2C.h"
#include "LIN.h"


// 1. Initialize I2C controller
//   - 1.1    I2C register address 0xFF000030
//   - 1.2    Set I2C clock speed to 100kHz
//   - 1.3    Set I2C control register to host mode
//   - 1.4    Set I2C control register with bus idle low
//   - 1.5    Set I2C control register with clock on rising edge
//   - 1.6    0xFF000030: | clk speed: 2 | host: 1 |  bus_idle: 1 | clk_edge: 2 |
// 2. Initialize LIN slave
//  - 2.1    LIN register address 0xFF000040
//  - 2.2    Set LIN control register to 9600 baud rate
//  - 2.3    Set LIN control register to 1 start bit
//  - 2.4    Set LIN control register to 1 stop bit
//  - 2.5    Set LIN control register to 8 data bits
//  - 2.6    Set LIN control register to no flow control
//  - 2.7    0xFF000040: | baud rate: 8 | parity: 2 | stop bits: 2 | data bits: 2 | flow control: 2 |
// 3. Setup LIN Interrupt cb
// 4. Infinite loop reading temperature sensor data
//  - 4.1    Read 8 bit temperature sensor data at i2c address 0x20
//  - 4.2    Cache latest temperature sensor data to send over LIN
//  - 4.3    Send temperature sensor data over LIN when requested

#define ADC_ADDR 0x20
#define ADC_REG  0x00
#define SLEEP_DURATION_MS 1000

// Reading the contents of the LIN Descriptor File is too much for this project
static uint8_t g_current_val = 0;

void lin_rx_isr(uint8_t id) {

    // If the LIN master is looking for node temp id, send the avg temp over LIN
    if(id == LIN_AVG_TEMP_SENSOR_ID) {
        // calculate the avg;


    } else if(id == LIN_CURRENT_TEMP_SENSOR_ID) {
        // If the LIN master is looking for node temp id, send the last sample over LIN

    }

    // Clear the lin interrupt before exit isr:

}

int main(int argc, char **argv) {
    // Configure the I2C controller and LIN controller here:


    // Add the LIN RX ISR
    lin_add_rx_frame_header_interrupt(lin_rx_isr);
    while(true) {

        // Read the ADC value via I2C here:


        printf("ADC Value: %d\n", g_current_val);

        std::this_thread::sleep_for(std::chrono::milliseconds (SLEEP_DURATION_MS));
    }
}
