#ifndef CYCLIC_TASK_H_
#define CYCLIC_TASK_H_

#include "esp_err.h"

#define CYCLIC_TASK_ENV_INTERVAL_MS 5000

esp_err_t cyclic_task_env_init(void);

#endif // CYCLIC_TASK_H_
