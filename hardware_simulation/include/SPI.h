#ifndef PROJECT_SPI_H
#define PROJECT_SPI_H
#include <cstdint>

extern "C" {

#define SPI_HARDWARE_REGISTER 0xFF000070
//CLK EDGE bits 1 - 2
#define SPI_CLK_RISING_EDGE 0x01
#define SPI_CLK_FALLING_EDGE 0x00
// IDLE State bits 3 - 4
#define SPI_IDLE_LOW 0b0100
#define SPI_IDLE_HIGH 0b1000
// Master/Slave bits 5
#define SPI_HOST  0b10000
#define SPI_SLAVE 0b00000
// Clock Speed bits 6 - 8
#define SPI_CLK_1MHZ   0b10000000
#define SPI_CLK_400KHZ 0b01000000
#define SPI_CLK_200KHZ 0b00100000

#define SPI_CS_0  0x01
#define SPI_CS_1  0x02

#define SPI_FLASH_PAGE_SIZE 64
#define SPI_FLASH_SZ 4096

#define SPI_FLASH_CMD_WRITE 0x01
#define SPI_FLASH_CMD_READ 0x02
#define SPI_FLASH_CMD_ERASE 0x03

// SPI_FLASH_PAGE_SIZE / sizeof(SPI_FLASH_data_pt_t) = 64 / 8 = 8
typedef struct {
    uint8_t avg_temp;
    uint8_t current_temp;
    unsigned int time;
} SPI_FLASH_data_pt_t;

typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t *data;
} SPI_xmit_t;

void spi_write_config(uint32_t reg, uint32_t value);

int spi_write_data(SPI_xmit_t *xmit, uint8_t cs);

int spi_read_data(SPI_xmit_t *xmit, uint8_t cs);

};
#endif //PROJECT_SPI_H
