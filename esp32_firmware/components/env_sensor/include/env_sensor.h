#ifndef ENV_SENSOR_H_
#define ENV_SENSOR_H_

#include "esp_err.h"
#include "driver/i2c.h"

#define AHT20_ADDR    0x38  // temp + humidity
#define BMP280_ADDR   0x77  // temp + pressure

typedef struct {
  float temp_aht;
  float hum_aht;
  float temp_bmp;
  float pres_bmp;
} env_data_t;

esp_err_t env_sensors_init(void);
esp_err_t env_sensors_read_all(env_data_t *data);

#endif // ENV_SENSOR_H_
