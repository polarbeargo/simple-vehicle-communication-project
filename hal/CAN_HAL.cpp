#include "CAN_HAL.h"
#include "CAN.h"

void SimCANBus::init(uint32_t addr, uint32_t config) {
    can_write_config(addr, config);
}

void SimCANBus::setRxISR(void (*callback)(uint32_t, CAN_FRAME_TYPES, uint8_t*, uint8_t)) {
    can_add_rx_packet_interrupt(callback);
}

void SimCANBus::send(uint32_t id, CAN_FRAME_TYPES type, uint8_t* data, uint8_t len) {
    can_send_new_packet(id, type, data, len);
}

void SimCANBus::clearRxInterrupt() {
    can_clear_rx_packet_interrupt();
}