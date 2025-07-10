#pragma once
#include <cstdint>
#include "SPI.h"

class SPI_HAL {
public:
    static void init(uint32_t addr, uint32_t config);
    static void erasePage(uint8_t page_number);
    static void writePage(uint8_t page_number, const void* data, uint32_t data_len);
};