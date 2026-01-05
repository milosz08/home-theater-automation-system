#ifndef IO_INPUT_H_
#define IO_INPUT_H_

#include "esp_err.h"

#define BTN_TIME_SHORT    50    // minimal time to press (debounce)
#define BTN_TIME_LONG     2000  // threshold for long click
#define BTN_TIME_FACTORY  5000  // threshold for fabric-reset

typedef enum
{
  BTN_CLICK_SHORT,
  BTN_CLICK_LONG,
  BTN_CLICK_FACTORY
} io_input_action_t;

typedef void (*io_input_callback_t)(io_input_action_t action);

typedef struct
{
  int pin;
  io_input_callback_t callback;
  const char *name;
} io_input_config_t;

esp_err_t io_input_init(const io_input_config_t *config, size_t num_buttons);

#endif // IO_INPUT_H_
