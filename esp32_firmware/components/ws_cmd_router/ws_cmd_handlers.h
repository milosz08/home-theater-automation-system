#ifndef WS_CMD_HANDLERS_H_
#define WS_CMD_HANDLERS_H_

#include "cJSON.h"

#define MAX_COMMANDS 3

// projection screen
void ws_cmd_screen_down(const cJSON *payload);
void ws_cmd_screen_stop(const cJSON *payload);
void ws_cmd_screen_up(const cJSON *payload);

#endif
