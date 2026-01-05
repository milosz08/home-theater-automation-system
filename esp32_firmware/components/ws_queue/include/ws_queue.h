#ifndef WS_QUEUE_H_
#define WS_QUEUE_H_

#include "esp_err.h"

#include <stdbool.h>

#define WS_QUEUE_SIZE 10
#define WS_EVENT_NAME_MAX 16

// event names
#define WS_EVENT_TYPE_ENV_UPDATE "env"

typedef struct
{
  char event[WS_EVENT_NAME_MAX];
  char *payload;
} ws_msg_t;

esp_err_t ws_queue_init(void);
bool ws_queue_send(const char *event_name, const char *json_data);
bool ws_queue_receive(ws_msg_t *out_msg, uint32_t ticks_to_wait);

#endif // WS_QUEUE_H_
