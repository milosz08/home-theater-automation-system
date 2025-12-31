#pragma once

#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

// w5500 pinouts setup
#define ETH_SPI_HOST        SPI3_HOST
#define ETH_SPI_SCLK_GPIO   GPIO_NUM_18   // SPI clock
#define ETH_SPI_MOSI_GPIO   GPIO_NUM_23   // SPI data input
#define ETH_SPI_MISO_GPIO   GPIO_NUM_19   // SPI data output
#define ETH_SPI_CS_GPIO     GPIO_NUM_5    // chip select
#define ETH_SPI_INT_GPIO    GPIO_NUM_4    // interrupt
#define ETH_SPI_CLOCK_MHZ   16            // clock speed
#define ETH_SPI_RST_GPIO    GPIO_NUM_32   // reset

esp_err_t eth_init_w5500(void);
