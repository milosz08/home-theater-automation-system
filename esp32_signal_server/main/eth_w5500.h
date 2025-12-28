#pragma once

// w5500 pinouts setup
#define ETH_SPI_HOST        SPI3_HOST
#define ETH_SPI_SCLK_GPIO   18          // SPI clock
#define ETH_SPI_MOSI_GPIO   23          // SPI data input
#define ETH_SPI_MISO_GPIO   19          // SPI data output
#define ETH_SPI_CS_GPIO     5           // chip select
#define ETH_SPI_INT_GPIO    4           // interrupt
#define ETH_SPI_CLOCK_MHZ   16          // clock speed
#define ETH_SPI_RST_GPIO    32          // reset

// static IP setup
#define STATIC_IP_ADDR      "192.168.0.125"
#define STATIC_IP_GW        "192.168.0.1"
#define STATIC_IP_NETMASK   "255.255.255.0"
#define STATIC_DNS_SERVER   "192.168.0.121"

void eth_init_w5500(void);
