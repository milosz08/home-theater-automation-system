#ifndef CYCLIC_TASK_H_
#define CYCLIC_TASK_H_

#include "esp_err.h"

#define CYCLIC_TASK_ENV_INTERVAL_MS 5000 /*!< How often (in ms) to read environmental sensors (5 seconds). */

/*! \brief Starts the background task for cyclic sensor readings.
 *
 * This task wakes up every `CYCLIC_TASK_ENV_INTERVAL_MS`, reads the AHT20/BMP280 sensors, and pushes the new data to
 * the WebSocket queue so clients get live updates.
 *
 * \retval ESP_OK         If the task started successfully.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       On other errors.
 */
esp_err_t cyclic_task_env_init(void);

#endif // CYCLIC_TASK_H_
