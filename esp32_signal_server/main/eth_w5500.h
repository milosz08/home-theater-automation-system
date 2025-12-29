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

// static/dynamic IP setup
#if defined(ESP_IP_ADDRESS) && defined(ESP_IP_DEFAULT_GATEWAY) && defined(ESP_IP_SUBNET_MASK)
  #define ENABLE_STATIC_IP 1
  #ifndef ESP_IP_DNS_ADDRESS
    #define ESP_IP_DNS_ADDRESS "1.1.1.1" // fallback DNS (cloudflare)
  #endif
#endif

void eth_init_w5500(void);
