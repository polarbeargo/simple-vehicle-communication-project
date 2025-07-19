#pragma once
#include <cstdint>
#include "CAN.h"

// Abstract interface for CAN HAL
class ICANBus {
public:
    virtual void init(uint32_t addr, uint32_t config) = 0;
    virtual void setRxISR(void (*callback)(uint32_t, CAN_FRAME_TYPES, uint8_t*, uint8_t)) = 0;
    virtual void send(uint32_t id, CAN_FRAME_TYPES type, uint8_t* data, uint8_t len) = 0;
    virtual void clearRxInterrupt() = 0;
    virtual int addFilter(uint8_t idx, uint16_t mask, uint16_t filter) = 0;
    virtual ~ICANBus() {}
};

// Concrete implementation for simulation/hardware
class SimCANBus : public ICANBus {
public:
    static SimCANBus& instance() {
        static SimCANBus instance;
        return instance;
    }
    void init(uint32_t addr, uint32_t config) override;
    void setRxISR(void (*callback)(uint32_t, CAN_FRAME_TYPES, uint8_t*, uint8_t)) override;
    void send(uint32_t id, CAN_FRAME_TYPES type, uint8_t* data, uint8_t len) override;
    void clearRxInterrupt() override;
    int addFilter(uint8_t idx, uint16_t mask, uint16_t filter) override;
private:
    SimCANBus() = default;
    SimCANBus(const SimCANBus&) = delete;
    SimCANBus& operator=(const SimCANBus&) = delete;
};