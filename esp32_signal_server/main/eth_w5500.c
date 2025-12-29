#include "eth_w5500.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

static const char *TAG = "ETH_W5500";

static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
  if (event_base == ETH_EVENT)
  {
    switch (event_id)
    {
      case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ethernet Link UP");
        break;
      case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link DOWN");
        break;
    }
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    ESP_LOGI(TAG, "Assigned IP: " IPSTR, IP2STR(&event->ip_info.ip));
  }
}

#ifdef ENABLE_STATIC_IP
static void set_static_ip(esp_netif_t *netif)
{
  ESP_ERROR_CHECK(esp_netif_dhcpc_stop(netif)); // disable DHCP client

  esp_netif_ip_info_t ip_info;
  esp_netif_str_to_ip4(ESP_IP_ADDRESS, &ip_info.ip);
  esp_netif_str_to_ip4(ESP_IP_DEFAULT_GATEWAY, &ip_info.gw);
  esp_netif_str_to_ip4(ESP_IP_SUBNET_MASK, &ip_info.netmask);
  ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));

  // DNS config
  esp_netif_dns_info_t dns_info = {};
  dns_info.ip.type = ESP_IPADDR_TYPE_V4;
  esp_netif_str_to_ip4(ESP_IP_DNS_ADDRESS, &dns_info.ip.u_addr.ip4);

  ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info));
  ESP_LOGI(TAG, "IP: %s, DNS: %s", ESP_IP_ADDRESS, ESP_IP_DNS_ADDRESS);
}
#endif

void eth_init_w5500(void)
{
  esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
  esp_netif_t *eth_netif = esp_netif_new(&cfg);

  #ifdef ENABLE_STATIC_IP
    set_static_ip(eth_netif);
  #else
    ESP_LOGW(TAG, "Missing static IP config - switching to DHCP");
  #endif

  spi_bus_config_t buscfg = {
    .miso_io_num = ETH_SPI_MISO_GPIO,
    .mosi_io_num = ETH_SPI_MOSI_GPIO,
    .sclk_io_num = ETH_SPI_SCLK_GPIO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
  };
  ESP_ERROR_CHECK(spi_bus_initialize(ETH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

  // init w5500 and mac
  spi_device_interface_config_t spi_devcfg = {
    .mode = 0,
    .clock_speed_hz = ETH_SPI_CLOCK_MHZ * 1000 * 1000,
    .spics_io_num = ETH_SPI_CS_GPIO,
    .queue_size = 20
  };
  eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(ETH_SPI_HOST, &spi_devcfg);
  w5500_config.int_gpio_num = ETH_SPI_INT_GPIO;
  eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
  esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);

  // init physical layer
  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
  phy_config.phy_addr = -1; 
  phy_config.reset_gpio_num = ETH_SPI_RST_GPIO;
  esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);

  // install driver
  esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
  esp_eth_handle_t eth_handle = NULL;
  ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));

  // get unique MAC address
  uint8_t base_mac_addr[ETH_ADDR_LEN];
  ESP_ERROR_CHECK(esp_read_mac(base_mac_addr, ESP_MAC_ETH));
  ESP_LOGI(TAG, "MAC: %02x:%02x:%02x:%02x:%02x:%02x",
           base_mac_addr[0], base_mac_addr[1], base_mac_addr[2],
           base_mac_addr[3], base_mac_addr[4], base_mac_addr[5]);

  ESP_ERROR_CHECK(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, base_mac_addr));

  // connect driver with netif
  ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

  // reigster handlers
  ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_event_handler, NULL));

  ESP_ERROR_CHECK(esp_eth_start(eth_handle));
}
