#ifndef STORAGE_H_
#define STORAGE_H_

#include "esp_err.h"

/*! \brief Main system configuration structure.
 *
 * Holds network settings and security credentials.
 */
typedef struct
{
  char ip[16];                /*!< Static IP. */
  char gateway[16];           /*!< Gateway IP. */
  char netmask[16];           /*!< Subnet mask. */
  char dns[16];               /*!< DNS IP. */
  int https_port;             /*!< Port for the web server (default 443). */
  char default_password[64];  /*!< Admin password for the API. */
} system_config_t;

/*! \brief Mounts the storage system (NVS/SPIFFS).
 *
 * Should be called very early in the boot process.
 *
 * \retval ESP_OK   On success.
 * \retval ESP_FAIL On init failure.
 */
esp_err_t storage_init(void);

/*! \brief Loads the system configuration from NVS.
 *
 * If keys are missing, it might load defaults or return empty strings.
 *
 * @param[out] config Pointer to the struct to fill.
 *
 * \retval ESP_OK   On success.
 * \retval ESP_FAIL If error occured during loading.
 */
esp_err_t storage_load_system_config(system_config_t *config);

#endif // STORAGE_H_
