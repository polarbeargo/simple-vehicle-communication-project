#include "LIN.h"
#include "CAN.h"
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <thread>

// 1. Configure the LIN controller with Baud 9600, 1 Start bit, 1 Stop bit, 8 Data bits, No flow control, Master mode
// 2. Configure the CAN controller with Baud 100K, 11 bit format
// 3. Every 500ms get the latest avg temperature from the temperature sensor module on the LIN bus
// 4. Every 500ms get the latest current temperature from the temperature sensor module on the LIN bus
// 5. Save the current timestamp for each request on the LIN bus
// -- 5.5 Use TIME_NOW_S() to get the current time in seconds
// 6. Respond the CAN RTR frames with DATA frames


#define SLEEP_MS(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#define TIME_NOW_S() std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()

// REMEMBER:
// 1. CAN RTR frames are request for data from another CAN device
// 2. CAN DATA frames are responses to a CAN RTR frames
// 3. The CAN IDs we are supporting are:
//    3.1 CAN_AVG_TEMPERATURE_11_SENSOR_ID
//    3.2 CAN_CURRENT_TEMP_11_SENSOR_ID
//    3.3 CAN_TIME_11_SENSOR_ID
void can_new_packet_isr(uint32_t id, CAN_FRAME_TYPES type, uint8_t *data, uint8_t len) {

    if(id == CAN_AVG_TEMPERATURE_11_SENSOR_ID && type == CAN_RTR_FRAME) {

        printf("Average temperature data: %d\n", data[0]);
        // Setup CAN DATA frame to send the avg temperature data

    } // else if() ...


    // Clear the can interrupt before exit isr:

}

void lin_rx_isr(uint8_t id, uint8_t *data, uint8_t len) {
    if(id == LIN_AVG_TEMP_SENSOR_ID) {


        printf("Average Temperature data: %d\n", data[0]);
    } else if(id == LIN_CURRENT_TEMP_SENSOR_ID) {


        printf("Current Temperature data: %d\n", data[0]);
    }

    // Clear the lin interrupt before exit isr:
}


int main(int argc, char **argv) {
    // Configure the LIN controller and CAN controller here:


    //Add the LIN frame response ISR
    // This ISR fires when a slave node responds to a master node request, In this case the temperature sensor module
    lin_add_frame_resp_interrupt(lin_rx_isr);

    // Add the CAN RX ISR
    can_add_rx_packet_interrupt(can_new_packet_isr);

    while(true) {
        // Every 500 ms get the avg temperature from the temperature module
        // Every 500 ms get the current temperature the temperature module

    }
    return 0;
}
