#ifndef IO_EXPANDER_H_
#define IO_EXPANDER_H_

#include <stdbool.h>

#include "esp_err.h"

#define EXPANDER_ADDR   0x20  // I2C expander address (3xGND)

esp_err_t io_expander_init(uint8_t input_mask);
esp_err_t io_expander_set_level(int pin, int level);        // thread-safe
esp_err_t io_expander_get_level(int pin, uint8_t *level);   // thread-safe

#endif // IO_EXPANDER_H_
