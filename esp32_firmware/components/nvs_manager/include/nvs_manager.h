#ifndef NVS_MANAGER_H_
#define NVS_MANAGER_H_

#include <stdint.h>
#include <stddef.h>

#include "esp_err.h"

typedef struct
{
  const char *key;
  const char *value;
} nvs_entry_t;

esp_err_t nvs_manager_init(void);
esp_err_t nvs_manager_save_strings_batch(const char *ns, const nvs_entry_t *entries, size_t count);
esp_err_t nvs_manager_save_str(const char *ns, const char *key, const char *value);
esp_err_t nvs_manager_load_str(const char *ns, const char *key, char *out_buf, size_t max_len);

#endif // NVS_MANAGER_H_
