#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN 2

void app_main(void)
{
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  printf("init...\n");

  while (1) {
    gpio_set_level(LED_PIN, 1);
    printf("LED ON\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    gpio_set_level(LED_PIN, 0);
    printf("LED OFF\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
