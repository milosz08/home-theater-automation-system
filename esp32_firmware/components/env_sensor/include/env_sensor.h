#ifndef ENV_SENSOR_H_
#define ENV_SENSOR_H_

#include "esp_err.h"
#include "driver/i2c.h"

/*! \brief I2C address for the AHT20 (temp & humidity) sensor. */
#define AHT20_ADDR    0x38
/*! \brief I2C address for the BMP280 (temp & pressure) sensor. */
#define BMP280_ADDR   0x77

/*! \brief Simple struct to hold all our environmental readings. */
typedef struct
{
  float temp_aht; /*!< Temperature from AHT20 in °C. */
  float hum_aht;  /*!< Humidity from AHT20 in %. */
  float temp_bmp; /*!< Temperature from BMP280 in °C. */
  float pres_bmp; /*!< Pressure from BMP280 in hPa. */
} env_data_t;

/*! \brief Wakes up and configures the sensors.
 *
 * Sends the initialization commands to the AHT20 and grabs the factory calibration data from the BMP280. Also sets the
 * BMP280 to normal mode with some filtering enabled so the readings aren't too jumpy.
 *
 * \retval ESP_OK   Sensors are ready to roll.
 * \retval ESP_FAIL Couldn't talk to one of the sensors.
 */
esp_err_t env_sensors_init(void);

/*! \brief Reads the latest data from both sensors.
 *
 * Triggers a measurement, waits a bit for the sensors to do their work. Reads the raw bytes, and does all the math to
 * convert them into real numbers (Celsius, %, hPa).
 *
 * \param[out] data Pointer to the `env_data_t` struct to fill.
 * 
 * \retval ESP_OK               Readings looks good.
 * \retval ESP_ERR_INVALID_ARG  You passed a NULL pointer.
 * \retval ESP_FAIL             I2C communication failed.
 */
esp_err_t env_sensors_read_all(env_data_t *data);

#endif // ENV_SENSOR_H_
