#ifndef NVS_MANAGER_H_
#define NVS_MANAGER_H_

#include <stdint.h>
#include <stddef.h>

#include "esp_err.h"

// Keys can have max 15 characters!
#define AUTH_NVS_NS           "auth_storage"
#define AUTH_NVS_KEY          "server_pass"
#define AUTH_NVS_DEFAULT_KEY  "def_server_pass"

/*! \brief Simple key-value pair helper struct. */
typedef struct
{
  const char *key;    /*!< The name of the key in NVS. */
  const char *value;  /*!< The string value to save. */
} nvs_entry_t;

/*! \brief Initializes the non-volatile storage (NVS).
 * 
 * Tries to init the partition. If it runs out of pages or finds a new version (which can happen after firmware
 * updates), it erases the partition and tries again.
 *
 * \retval ESP_OK   If storage is ready.
 * \retval ESP_FAIL If error occured.
 */
esp_err_t nvs_manager_init(void);

/*! \brief Saves multiple strings at once.
 *
 * Opens the namespace, writes all valid entries from the array, commits changes, and closes the handle. Efficient for
 * saving config dumps.
 *
 * \param ns      The NVS namespace (group name).
 * \param entries Array of `nvs_entry_t` structures.
 * \param count   How many items are in the array.
 *
 * \retval ESP_OK   If all strings saved.
 * \retval ESP_FAIL If some NVS error occured.
 */
esp_err_t nvs_manager_save_strings_batch(const char *ns, const nvs_entry_t *entries, size_t count);

/*! \brief Saves a single string to flash.
 *
 * Opens, writes, commits, closes.
 *
 * \param ns    The NVS namespace.
 * \param key   The key name.
 * \param value The string to store.
 *
 * \retval ESP_OK   If string saved.
 * \retval ESP_FAIL If some NVS error occured.
 */
esp_err_t nvs_manager_save_str(const char *ns, const char *key, const char *value);

/*! \brief Loads a string from flash.
 *
 * \param ns            The NVS namespace.
 * \param key           The key to look for.
 * \param[out] out_buf  Buffer where the string will be copied.
 * \param max_len       Size of the buffer (to prevent overflow).
 *
 * \retval ESP_OK                 Found and loaded.
 * \retval ESP_ERR_NVS_NOT_FOUND  Key doesn't exist.
 * \retval ESP_FAIL               If some NVS error occured.
 */
esp_err_t nvs_manager_load_str(const char *ns, const char *key, char *out_buf, size_t max_len);

#endif // NVS_MANAGER_H_
