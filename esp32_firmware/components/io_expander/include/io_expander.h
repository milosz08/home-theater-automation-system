#ifndef IO_EXPANDER_H_
#define IO_EXPANDER_H_

#include <stdbool.h>

#include "esp_err.h"

#define EXPANDER_ADDR             0x20  // I2C expander address (3xGND)
#define EXPANDER_INIT_PINS_MASK   0xE0  // 11100000 (in: 7,6,5; out: 4,3,2,1,0)
#define EXPANDER_PORT_SHADOW      0xFF  // 1111111

esp_err_t io_expander_init();
esp_err_t io_expander_set_level(int pin, int level);        // thread-safe
esp_err_t io_expander_get_level(int pin, uint8_t *level);   // thread-safe

#endif // IO_EXPANDER_H_
