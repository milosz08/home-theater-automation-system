#include "eth_w5500.h"

#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "ETH_W5500";

typedef struct
{
  eth_callbacks_t callbacks;
  esp_netif_t *netif;
} eth_context_t;

static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  eth_context_t *ctx = (eth_context_t *)arg;
  if (event_base == ETH_EVENT)
  {
    switch (event_id)
    {
      case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "ethernet link up");
        if (ctx && ctx->callbacks.on_link_state_changed) ctx->callbacks.on_link_state_changed(true);
        break;
      case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "ethernet link down");
        if (ctx && ctx->callbacks.on_link_state_changed) ctx->callbacks.on_link_state_changed(false);
        break;
    }
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "assigned ip: " IPSTR, IP2STR(&event->ip_info.ip));
  }
}

static esp_err_t pkt_eth_input_proxy(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv)
{
  eth_context_t *ctx = (eth_context_t *)priv;
  if (ctx)
  {
    if (ctx->callbacks.on_packet_received) ctx->callbacks.on_packet_received();
    return esp_netif_receive(ctx->netif, buffer, length, NULL);
  }
  return ESP_FAIL;
}

static esp_err_t set_static_ip(esp_netif_t *netif, const eth_config_t *cfg)
{
  if (cfg == NULL || cfg->ip == NULL || strlen(cfg->ip) == 0)
  {
    ESP_LOGW(TAG, "static ip not configured/empty, skipping (dhcp default)...");
    return ESP_OK;
  }

  esp_err_t err = esp_netif_dhcpc_stop(netif);
  if (err != ESP_OK && err != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED) return err;

  esp_netif_ip_info_t ip_info;
  esp_netif_str_to_ip4(cfg->ip, &ip_info.ip);
  esp_netif_str_to_ip4(cfg->gateway, &ip_info.gw);
  esp_netif_str_to_ip4(cfg->netmask, &ip_info.netmask);

  err = esp_netif_set_ip_info(netif, &ip_info);
  if (err != ESP_OK) return err;

  // DNS config
  esp_netif_dns_info_t dns_info = {};
  dns_info.ip.type = ESP_IPADDR_TYPE_V4;

  const char *dns_addr = (cfg->dns && strlen(cfg->dns) > 0) ? cfg->dns : "1.1.1.1";
  esp_netif_str_to_ip4(dns_addr, &dns_info.ip.u_addr.ip4);

  err = esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
  if (err != ESP_OK) return err;

  ESP_LOGI(TAG, "static ip applied: %s, gw: %s, dns: %s", cfg->ip, cfg->gateway, dns_addr);
  return ESP_OK;
}

static void eth_startup_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  EventGroupHandle_t eth_group = (EventGroupHandle_t)arg;
  if (event_base == ETH_EVENT && event_id == ETHERNET_EVENT_CONNECTED) xEventGroupSetBits(eth_group, BIT0);
}

static esp_err_t eth_w5500_wait_for_link(uint32_t timeout_ms)
{
  EventGroupHandle_t eth_boot_group = xEventGroupCreate();
  esp_event_handler_instance_t instance;

  esp_err_t err = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, eth_startup_event_handler,
                                                      eth_boot_group, &instance);
  if (err != ESP_OK)
  {
    vEventGroupDelete(eth_boot_group);
    return err;
  }
  ESP_LOGI(TAG, "waiting for link up (timeout: %lu ms)...", timeout_ms);

  EventBits_t bits = xEventGroupWaitBits(eth_boot_group, BIT0, pdTRUE, pdTRUE, pdMS_TO_TICKS(timeout_ms));

  esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, instance);
  vEventGroupDelete(eth_boot_group);

  if (bits & BIT0)
  {
    ESP_LOGI(TAG, "link detected");
    return ESP_OK;
  }
  ESP_LOGE(TAG, "link timeout (cable unplugged?)");
  return ESP_ERR_TIMEOUT;
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t eth_w5500_init(const eth_config_t *config, const eth_callbacks_t *callbacks)
{
  eth_context_t *ctx = malloc(sizeof(eth_context_t));
  if (ctx == NULL) return ESP_ERR_NO_MEM;

  if (callbacks) ctx->callbacks = *callbacks;
  else memset(&ctx->callbacks, 0, sizeof(eth_callbacks_t));

  esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
  esp_netif_t *eth_netif = esp_netif_new(&cfg);

  ctx->netif = eth_netif;

  esp_err_t err = set_static_ip(eth_netif, config);
  if (err != ESP_OK)
  {
    free(ctx);
    return err;
  }

  spi_bus_config_t buscfg = {
    .miso_io_num    = ETH_SPI_MISO_GPIO,
    .mosi_io_num    = ETH_SPI_MOSI_GPIO,
    .sclk_io_num    = ETH_SPI_SCLK_GPIO,
    .quadwp_io_num  = -1,
    .quadhd_io_num  = -1,
  };
  err = spi_bus_initialize(ETH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
  if (err != ESP_OK)
  {
    free(ctx);
    return err;
  }

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
  if (mac == NULL)
  {
    ESP_LOGE(TAG, "failed to create mac instance");
    free(ctx);
    return ESP_FAIL;
  }

  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
  phy_config.phy_addr = -1;
  phy_config.reset_gpio_num = ETH_SPI_RST_GPIO;

  esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);
  if (phy == NULL)
  {
    ESP_LOGE(TAG, "failed to create phy instance");
    free(ctx);
    return ESP_FAIL;
  }

  esp_eth_config_t eth_config_driver = ETH_DEFAULT_CONFIG(mac, phy);
  esp_eth_handle_t eth_handle = NULL;
  err = esp_eth_driver_install(&eth_config_driver, &eth_handle);
  if (err != ESP_OK)
  {
    free(ctx);
    return err;
  }
  uint8_t base_mac_addr[ETH_ADDR_LEN];
  err = esp_read_mac(base_mac_addr, ESP_MAC_ETH);
  if (err != ESP_OK) return err;

  ESP_LOGI(TAG, "mac: %02x:%02x:%02x:%02x:%02x:%02x",
            base_mac_addr[0], base_mac_addr[1], base_mac_addr[2],
            base_mac_addr[3], base_mac_addr[4], base_mac_addr[5]);

  err = esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, base_mac_addr);
  if (err != ESP_OK) return err;

  void *glue = esp_eth_new_netif_glue(eth_handle);
  err = esp_netif_attach(eth_netif, glue);
  if (err != ESP_OK) return err;

  // register handlers
  err = esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, ctx);
  if (err != ESP_OK) return err;

  err = esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_event_handler, ctx);
  if (err != ESP_OK) return err;

  // proxy input for packets activity callback function
  err = esp_eth_update_input_path(eth_handle, pkt_eth_input_proxy, ctx);
  if (err != ESP_OK) return err;

  err = esp_eth_start(eth_handle);
  if (err != ESP_OK) return err;

  return ESP_OK;
}

void eth_w5500_force_link_blocking(eth_link_wait_cb_t wait_cb)
{
  if (eth_w5500_wait_for_link(ETH_INIT_WAIT_FOR_LINK_MILLIS) == ESP_OK)
  {
    ESP_LOGI(TAG, "ethernet link detected immediately");
    if (wait_cb) wait_cb(true);
    return;
  }
  ESP_LOGW(TAG, "ethernet link not detected after grace period, starting alerts");
  
  while (eth_w5500_wait_for_link(ETH_INIT_LINK_CHECK_INTERVAL_MILLIS) != ESP_OK)
  {
    if (wait_cb) wait_cb(false);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  ESP_LOGI(TAG, "ethernet link restored");
  if (wait_cb) wait_cb(true);
}

void eth_w5500_network_info(char *out_ip, uint8_t *out_mac)
{
  if (out_ip == NULL || out_mac == NULL) return;

  esp_read_mac(out_mac, ESP_MAC_ETH);

  esp_netif_t *netif = esp_netif_next_unsafe(NULL);
  esp_netif_ip_info_t ip_info = {0};

  if (netif != NULL) esp_netif_get_ip_info(netif, &ip_info);

  if (ip_info.ip.addr != 0) sprintf(out_ip, IPSTR, IP2STR(&ip_info.ip));
  else strcpy(out_ip, "0.0.0.0"); // no connection/connection during initialization
}
