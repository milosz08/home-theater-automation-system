#ifndef WS_QUEUE_H_
#define WS_QUEUE_H_

#include "esp_err.h"

#include <stdbool.h>

/*! \brief Max messages in the queue waiting to be sent. */
#define WS_QUEUE_SIZE 10
/*! \brief Max length of the event name string. */
#define WS_EVENT_NAME_MAX 16

// event names

/*! \brief Event name for environmental updates (temp/hum). */
#define WS_EVENT_TYPE_ENV_UPDATE "env"
/*! \brief Event name for getting system info (e.g., version, name) */
#define WS_EVENT_TYPE_SYS_INFO "sys-info"
/*! \brief Event name for getting firmware ota update status (e.g., percentage, bytes count) */
#define WS_EVENT_TYPE_OTA_UPDATE_PROGRESS "update-progress"

/*! \brief Structure representing a queued message. */
typedef struct
{
  char event[WS_EVENT_NAME_MAX];  /*!< Event type (e.g., "env"). */
  char *payload;                  /*!< JSON payload (heap allocated). */
} ws_msg_t;

/*! \brief Initializes the WebSocket Message Queue.
 *
 * Creates the FreeRTOS queue used to buffer messages between tasks (e.g., sensor task -> ws dispatcher task).
 *
 * \retval ESP_OK         If success.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       If websocket message queue initialization failure.
 */
esp_err_t ws_queue_init(void);

/*! \brief Pushes a message into the queue.
 *
 * Note: the `json_data` must be heap-allocated. The queue takes ownership and will free it after sending.
 *
 * \param event_name  Type of event.
 * \param json_data   JSON string (must be malloc'd).
 * 
 * \return `true` if queued, `false` if queue full (payload freed automatically).
 */
bool ws_queue_send(const char *event_name, const char *json_data);

/*! \brief Pops a message from the queue.
 *
 * \param[out] out_msg  Structure to fill with event/payload.
 * \param ticks_to_wait How long to wait for a message.
 *
 * \return `true` if a message was received.
 */
bool ws_queue_receive(ws_msg_t *out_msg, uint32_t ticks_to_wait);

#endif // WS_QUEUE_H_
