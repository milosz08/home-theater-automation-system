#include "env_sensor.h"
#include "i2c_bus.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "ENV_SENSORS";

// calibrating data for BMP280
typedef struct {
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;
  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;
} bmp280_calib_t;

static bmp280_calib_t bmp_calib;
static int32_t t_fine;

static esp_err_t bmp280_read_calib()
{
  uint8_t reg = 0x88;
  uint8_t data[24];
  esp_err_t err = i2c_master_write_read_device(I2C_MASTER_NUM, BMP280_ADDR, &reg, 1, data, 24, pdMS_TO_TICKS(100));
  if (err != ESP_OK) return err;

  bmp_calib.dig_T1 = (data[1] << 8) | data[0];
  bmp_calib.dig_T2 = (int16_t)((data[3] << 8) | data[2]);
  bmp_calib.dig_T3 = (int16_t)((data[5] << 8) | data[4]);
  bmp_calib.dig_P1 = (data[7] << 8) | data[6];
  bmp_calib.dig_P2 = (int16_t)((data[9] << 8) | data[8]);
  bmp_calib.dig_P3 = (int16_t)((data[11] << 8) | data[10]);
  bmp_calib.dig_P4 = (int16_t)((data[13] << 8) | data[12]);
  bmp_calib.dig_P5 = (int16_t)((data[15] << 8) | data[14]);
  bmp_calib.dig_P6 = (int16_t)((data[17] << 8) | data[16]);
  bmp_calib.dig_P7 = (int16_t)((data[19] << 8) | data[18]);
  bmp_calib.dig_P8 = (int16_t)((data[21] << 8) | data[20]);
  bmp_calib.dig_P9 = (int16_t)((data[23] << 8) | data[22]);
  return ESP_OK;
}

static int32_t bmp280_comp_T(int32_t adc_T)
{
  int32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((int32_t)bmp_calib.dig_T1 << 1))) * ((int32_t)bmp_calib.dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)bmp_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)bmp_calib.dig_T1))) >> 12)
          * ((int32_t)bmp_calib.dig_T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}

static uint32_t bmp280_comp_P(int32_t adc_P)
{
  int64_t var1, var2, p;
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)bmp_calib.dig_P6;
  var2 = var2 + ((var1 * (int64_t)bmp_calib.dig_P5) << 17);
  var2 = var2 + (((int64_t)bmp_calib.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)bmp_calib.dig_P3) >> 8) + ((var1 * (int64_t)bmp_calib.dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bmp_calib.dig_P1) >> 33;
  if (var1 == 0) return 0;

  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)bmp_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)bmp_calib.dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)bmp_calib.dig_P7) << 4);

  return (uint32_t)p;
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t env_sensors_init(void)
{
  esp_err_t err;

  // AHT20 init
  uint8_t aht_init[] = {0xBE, 0x08, 0x00};
  err = i2c_master_write_to_device(I2C_MASTER_NUM, AHT20_ADDR, aht_init, 3, pdMS_TO_TICKS(100));
  if (err != ESP_OK) return err;
  vTaskDelay(pdMS_TO_TICKS(100));

  // BMP280 init
  err = bmp280_read_calib();
  if (err != ESP_OK) return err;

  // config: normal mode, osrs_t x4, osrs_p x4, standby 500ms, filter x16
  uint8_t bmp_cfg1[] = {0xF4, 0x57}; 
  uint8_t bmp_cfg2[] = {0xF5, 0x10};
  i2c_master_write_to_device(I2C_MASTER_NUM, BMP280_ADDR, bmp_cfg1, 2, pdMS_TO_TICKS(100));
  i2c_master_write_to_device(I2C_MASTER_NUM, BMP280_ADDR, bmp_cfg2, 2, pdMS_TO_TICKS(100));

  ESP_LOGI(TAG, "env sensors initialized on port %d", I2C_MASTER_NUM);
  return ESP_OK;
}

esp_err_t env_sensors_read_all(env_data_t *data)
{
  if (!data) return ESP_ERR_INVALID_ARG;
  esp_err_t err;

  // AHT20
  uint8_t aht_cmd[] = {0xAC, 0x33, 0x00};
  i2c_master_write_to_device(I2C_MASTER_NUM, AHT20_ADDR, aht_cmd, 3, pdMS_TO_TICKS(100));
  vTaskDelay(pdMS_TO_TICKS(80)); // wait for measurement

  uint8_t aht_buf[6];
  err = i2c_master_read_from_device(I2C_MASTER_NUM, AHT20_ADDR, aht_buf, 6, pdMS_TO_TICKS(100));
  if (err == ESP_OK && !(aht_buf[0] & 0x80))
  {
    uint32_t hum_raw = ((uint32_t)aht_buf[1] << 12) | ((uint32_t)aht_buf[2] << 4) | (aht_buf[3] >> 4);
    uint32_t temp_raw = ((uint32_t)(aht_buf[3] & 0x0F) << 16) | ((uint32_t)aht_buf[4] << 8) | aht_buf[5];
    data->hum_aht = ((float)hum_raw / 1048576.0f) * 100.0f;
    data->temp_aht = ((float)temp_raw / 1048576.0f) * 200.0f - 50.0f;
  }
  else
  {
    data->hum_aht = 0.0f;
    data->temp_aht = 0.0f;
  }
  // BMP280
  uint8_t bmp_reg = 0xF7;
  uint8_t bmp_buf[6];
  err = i2c_master_write_read_device(I2C_MASTER_NUM, BMP280_ADDR, &bmp_reg, 1, bmp_buf, 6, pdMS_TO_TICKS(100));
  if (err == ESP_OK)
  {
    int32_t adc_P = (bmp_buf[0] << 12) | (bmp_buf[1] << 4) | (bmp_buf[2] >> 4);
    int32_t adc_T = (bmp_buf[3] << 12) | (bmp_buf[4] << 4) | (bmp_buf[5] >> 4);

    int32_t t_int = bmp280_comp_T(adc_T);
    uint32_t p_int = bmp280_comp_P(adc_P);

    data->temp_bmp = t_int / 100.0f;
    data->pres_bmp = p_int / 256.0f / 100.0f; // hpa
  }
  return ESP_OK;
}
