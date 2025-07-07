#include "LIN_HAL.h"
#include "LIN.h"

void LIN_HAL::init(uint32_t addr, uint32_t config) {
    lin_write_config(addr, config);
}

void LIN_HAL::setRxISR(void (*cb)(uint8_t)) {
    lin_add_rx_frame_header_interrupt(cb);
}

void LIN_HAL::setFrameRespISR(void (*cb)(uint8_t, uint8_t*, uint8_t)) {
    lin_add_frame_resp_interrupt(cb);
}

void LIN_HAL::sendResponse(uint8_t id, uint8_t* data, uint8_t len) {
    lin_write_response_data(id, data, len);
}

void LIN_HAL::clearInterrupt() {
    lin_clear_rx_frame_interrupt();
}

void LIN_HAL::writeFrameHeader(uint8_t id) {
    lin_write_frame_header(id);
}

void LIN_HAL::clearFrameRespInterrupt() {
    lin_clear_frame_resp_interrupt();
}