#pragma once
#include <cstdint>

class LIN_HAL {
public:
    static void init(uint32_t addr, uint32_t config);
    static void setRxISR(void (*cb)(uint8_t)); 
    static void setFrameRespISR(void (*cb)(uint8_t, uint8_t*, uint8_t)); 
    static void sendResponse(uint8_t id, uint8_t* data, uint8_t len);
    static void clearInterrupt();
    static void writeFrameHeader(uint8_t id);
    static void clearFrameRespInterrupt();
};