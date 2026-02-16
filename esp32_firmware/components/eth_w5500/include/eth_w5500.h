#ifndef EHT_W5500_H_
#define EHT_W5500_H_

#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

/*! \brief SPI Host controller instance (VSPI/HSPI). */
#define ETH_SPI_HOST        SPI3_HOST
/*! \brief GPIO pin for SPI clock (SCK). */
#define ETH_SPI_SCLK_GPIO   GPIO_NUM_18
/*! \brief GPIO pin for master out slave in (MOSI) - sending data to W5500. */
#define ETH_SPI_MOSI_GPIO   GPIO_NUM_23
/*! \brief GPIO pin for master in slave out (MISO) - receiving data from W5500. */
#define ETH_SPI_MISO_GPIO   GPIO_NUM_19
/*! \brief GPIO pin for chip select (CS) - active low. */
#define ETH_SPI_CS_GPIO     GPIO_NUM_5
/*! \brief GPIO pin for interrupt (INT) - signals incoming packets. */
#define ETH_SPI_INT_GPIO    GPIO_NUM_4
/*! \brief SPI Clock speed in MHz. */
#define ETH_SPI_CLOCK_MHZ   16
/*! \brief GPIO pin for hardware reset (RST) of the W5500 chip. */
#define ETH_SPI_RST_GPIO    GPIO_NUM_32

/*! \brief Initial silent wait time for auto-negotiation before alerting the user. */
#define ETH_INIT_WAIT_FOR_LINK_MILLIS 2500
/*! \brief Interval between consecutive link status checks during blocking wait. */
#define ETH_INIT_LINK_CHECK_INTERVAL_MILLIS 1000

/*! \brief Static network configuration parameters.
 *
 * If `ip` is NULL or empty, DHCP will be used.
 */
typedef struct
{
  const char *ip;       /*!< Static IP address. */
  const char *gateway;  /*!< Gateway address. */
  const char *netmask;  /*!< Subnet mask. */
  const char *dns;      /*!< DNS server address. */
} eth_config_t;

/*! \brief Callbacks for Ethernet events. */
typedef struct
{
  void (*on_link_state_changed)(bool is_up);  /*!< Called when cable is plugged/unplugged. */
  void (*on_packet_received)(void);           /*!< Called on RX activity (useful for blinking LEDs). */
} eth_callbacks_t;

/*! \brief Callback type for boot-time link waiting process.
 *
 * \param state true if link is detected (success), false if still waiting (alert).
 */
typedef void (*eth_link_wait_cb_t)(bool state);

/*! \brief Initializes the W5500 Ethernet module.
 *
 * Sets up SPI bus, MAC, PHY, and attaches the network interface glue. Configures static IP if provided, otherwise
 * defaults to DHCP.
 *
 * \param config    Network settings (IP/GW/Mask/DNS).
 * \param callbacks Event handlers for link status and packet activity.
 *
 * \retval ESP_OK         If hardware initialized successfully.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       On hardware initialization failure.
 */
esp_err_t eth_w5500_init(const eth_config_t *config, const eth_callbacks_t *callbacks);

/*! \brief Blocks system execution until a physical Ethernet link is established.
 *
 * First, it waits silently for `ETH_INIT_WAIT_FOR_LINK_MILLIS` to allow auto-negotiation. If no link is found, it
 * enters an alert loop calling `wait_cb` with `false` every `ETH_INIT_LINK_CHECK_INTERVAL_MILLIS` until a cable is
 * connected.
 *
 * \param wait_cb Callback to update UI or indicators during the waiting process.
 */
void eth_w5500_force_link_blocking(eth_link_wait_cb_t wait_cb);

/*! \brief Gets the current network status.
 *
 * Reads the actual IP address assigned to the interface (from static or DHCP).
 *
 * \param[out] out_ip   Buffer to store the IP string (e.g., "192.168.1.5").
 * \param[out] out_mac  Buffer (6 bytes) to store the MAC address.
 */
void eth_w5500_network_info(char *out_ip, uint8_t *out_mac);

#endif // EHT_W5500_H_
