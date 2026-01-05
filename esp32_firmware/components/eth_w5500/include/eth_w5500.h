#ifndef EHT_W5500_H_
#define EHT_W5500_H_

#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define ETH_SPI_HOST        SPI3_HOST
#define ETH_SPI_SCLK_GPIO   GPIO_NUM_18   // SPI clock
#define ETH_SPI_MOSI_GPIO   GPIO_NUM_23   // SPI data input
#define ETH_SPI_MISO_GPIO   GPIO_NUM_19   // SPI data output
#define ETH_SPI_CS_GPIO     GPIO_NUM_5    // chip select
#define ETH_SPI_INT_GPIO    GPIO_NUM_4    // interrupt
#define ETH_SPI_CLOCK_MHZ   16            // clock speed
#define ETH_SPI_RST_GPIO    GPIO_NUM_32   // reset

typedef struct
{
  const char *ip;
  const char *gateway;
  const char *netmask;
  const char *dns;
} eth_config_t;

typedef struct
{
  void (*on_link_state_changed)(bool is_up);
  void (*on_packet_received)(void);
} eth_callbacks_t;

esp_err_t eth_w5500_init(const eth_config_t *config, const eth_callbacks_t *callbacks);
void eth_w5500_network_info(char *out_ip, uint8_t *out_mac);

#endif // EHT_W5500_H_
