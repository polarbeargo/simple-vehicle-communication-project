#include "SPI_HAL.h"
#include "SPI.h"
#include <cstring>

void SPI_HAL::init(uint32_t addr, uint32_t config) {
    spi_write_config(addr, config);
}

void SPI_HAL::erasePage(uint8_t page_number) {
    SPI_xmit_t erase_xmit;
    erase_xmit.data[0] = SPI_FLASH_CMD_ERASE;
    erase_xmit.data[1] = page_number;
    erase_xmit.len = 2;
    spi_write_data(&erase_xmit, SPI_CS_1);
}

void SPI_HAL::writePage(uint8_t page_number, const void* data, uint32_t data_len) {
    SPI_xmit_t write_xmit;
    write_xmit.data[0] = SPI_FLASH_CMD_WRITE;
    write_xmit.data[1] = page_number;
    memcpy(&write_xmit.data[2], data, data_len);
    write_xmit.len = data_len + 2;
    spi_write_data(&write_xmit, SPI_CS_1);
}