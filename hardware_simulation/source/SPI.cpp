#include "CAN.h"
#include "SPI.h"
#include <cstring>

#define SPI_NUM_OF_PAGES  (SPI_FLASH_SZ / SPI_FLASH_PAGE_SIZE)
uint8_t g_spi_flash_data[SPI_NUM_OF_PAGES][SPI_FLASH_PAGE_SIZE] = {0};

typedef struct {
    uint32_t reg;
    uint8_t data[8];
} SPI_t;

static SPI_t g_spi;

void spi_write_config(uint32_t reg, uint32_t value) {
    // Write the configuration value to the SPI bus
    if(reg == SPI_HARDWARE_REGISTER) {
        g_spi.reg = value;
    } else {
        g_spi.reg = 0;
    }

    // intentionally not erased
    memset(g_spi_flash_data, 0, sizeof(g_spi_flash_data));
}

static bool is_page_erased(uint16_t pg_num) {
    for(int i = 0; i < SPI_FLASH_PAGE_SIZE; i++) {
        if(g_spi_flash_data[pg_num][i] != 0xFF) {
            return false;
        }
    }
    return true;
}

// returns -1 if invalid operation
// returns -2 if invalid page number
// returns -3 if the flash operation is invalid
// returns pg num if successful
int spi_write_data(SPI_xmit_t *xmit, uint8_t cs) {
    uint32_t pg_num = xmit->data[0];
    uint8_t *data = xmit->data + sizeof(uint32_t);

    if(!(g_spi.reg & SPI_CLK_1MHZ) || cs != SPI_CS_1) {
        return -1;
    }

    if(pg_num >= SPI_NUM_OF_PAGES) {
        return -2;
    }

    if(xmit->cmd == SPI_FLASH_CMD_WRITE && is_page_erased(pg_num)) {
        memcpy(g_spi_flash_data[pg_num], data, SPI_FLASH_PAGE_SIZE);
        return pg_num;
    } else if(xmit->cmd == SPI_FLASH_CMD_ERASE) {
        memset(g_spi_flash_data[pg_num], 0xFF, SPI_FLASH_PAGE_SIZE);
        return pg_num;
    } else if(xmit->cmd == SPI_FLASH_CMD_READ) {
        // Intentionally do not do a memcpy here, that is a SPI read operation
        return pg_num;
    }
    return -3;
}

int spi_read_data(SPI_xmit_t *xmit, uint8_t cs) {
    uint32_t pg_num = xmit->data[0];
    uint8_t *data = xmit->data + sizeof(uint32_t);

    if(!(g_spi.reg & SPI_CLK_1MHZ) || cs != SPI_CS_1) {
        return -1;
    }

    if(pg_num >= SPI_NUM_OF_PAGES) {
        return -2;
    }

    if(xmit->cmd == SPI_FLASH_CMD_READ) {
        memcpy(data, g_spi_flash_data[pg_num], SPI_FLASH_PAGE_SIZE);
        return pg_num;
    }
    return -3;
}