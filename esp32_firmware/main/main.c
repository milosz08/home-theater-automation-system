#include "app_api.h"
#include "cyclic_task.h"
#include "env_sensor.h"
#include "eth_w5500.h"
#include "https_server.h"
#include "i2c_bus.h"
#include "io_expander.h"
#include "io_button.h"
#include "io_input.h"
#include "sys_ind.h"
#include "nvs_manager.h"
#include "storage.h"
#include "uart_bus.h"
#include "ui_manager.h"
#include "ui.h"
#include "ws_cmd_router.h"
#include "ws_dispatcher.h"
#include "ws_queue.h"

#include <stddef.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PROGRESS_BAR_COOLDOWN_MS  100 // time between next steps
#define TOTAL_STEPS               11  // total steps

#define CHECK_CRITICAL(x, msg) do { \
  esp_err_t err_rc = (x); \
  if (err_rc != ESP_OK) { \
    sys_ind_set_error(true); \
    sys_ind_buzzer_sound(2, BUZZER_PERIOD_MS); \
    ui_show_critical_error(msg, err_rc); \
  } \
} while(0)

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "MAIN";

static void on_server_running(void)
{
  sys_ind_set_error(false);
  ui_restore_screen();
}

static void on_server_stop(void)
{
  sys_ind_set_error(true);
  sys_ind_buzzer_sound(2, BUZZER_PERIOD_MS);
  ui_show_error("Net: link DOWN");
}

static void on_server_error(esp_err_t err)
{
  sys_ind_set_error(true);
  sys_ind_buzzer_sound(2, BUZZER_PERIOD_MS);
  ui_show_error("Server fail");
}

static void on_command_action(const char *friendly_name)
{
  sys_ind_led_io_cmd_execution();
  if (friendly_name != NULL) ui_show_fixed_temp_cmd_message(friendly_name);
}

// public api ----------------------------------------------------------------------------------------------------------

void app_main(void)
{
  ESP_LOGI(TAG, "System starting...");

  int current_step = 0;
  system_config_t s_config;

  // 1. I2C bus and expander init (before LCD)
  ESP_ERROR_CHECK(i2c_bus_init());
  ESP_ERROR_CHECK(io_expander_init());
  ESP_ERROR_CHECK(sys_ind_init());

  // 2. UI and LCD init (init driver and load icons)
  ESP_ERROR_CHECK(ui_init());
  ui_show_boot_progress("Booting...", ++current_step, TOTAL_STEPS);
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 3. env sensors
  ui_show_boot_progress("Init env sensors", ++current_step, TOTAL_STEPS);
  CHECK_CRITICAL(env_sensors_init(), "Env sensors fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 4. flash + auto-recovery
  ui_show_boot_progress("Init NVS flash", ++current_step, TOTAL_STEPS);
  CHECK_CRITICAL(nvs_manager_init(), "NVS init fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 5. filesystem (storage)
  ui_show_boot_progress("Mounting storage", ++current_step, TOTAL_STEPS);
  CHECK_CRITICAL(storage_init(), "Storage fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 6. config
  ui_show_boot_progress("Loading config", ++current_step, TOTAL_STEPS);
  CHECK_CRITICAL(storage_load_system_config(&s_config), "Config fail");
  const nvs_entry_t nvs_entries[] = {
    { .key = AUTH_NVS_KEY, .value = s_config.default_password },
    { .key = AUTH_NVS_DEFAULT_KEY, .value = s_config.default_password } // only for restore
  };
  CHECK_CRITICAL(nvs_manager_save_strings_batch(AUTH_NVS_NS, nvs_entries,
                                                sizeof(nvs_entries) / sizeof(nvs_entries[0])), "NVS persist fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 7. IO and peripherals
  ui_show_boot_progress("Config IO ports", ++current_step, TOTAL_STEPS);
  CHECK_CRITICAL(io_input_init(io_button_get_buttons(), io_button_get_buttons_count()), "IO input fail");
  CHECK_CRITICAL(uart_bus_rs485_init(), "RS485 fail");
  CHECK_CRITICAL(uart_bus_rs232_init(), "RS232 fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 8. system services
  ui_show_boot_progress("Start services", ++current_step, TOTAL_STEPS);
  CHECK_CRITICAL(esp_netif_init(), "Netif fail");
  CHECK_CRITICAL(esp_event_loop_create_default(), "EventLoop fail");
  CHECK_CRITICAL(gpio_install_isr_service(0), "GPIO ISR fail"); // enable interrupts
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 9. http service
  ui_show_boot_progress("Config HTTPS", ++current_step, TOTAL_STEPS);
  https_server_config_t https_cfg = {
    .port               = s_config.https_port,
    .cacert_path        = "/storage/cacert.pem",
    .prvtkey_path       = "/storage/prvtkey.pem",
    .endpoints          = app_api_get_endpoints(),
    .num_endpoints      = app_api_get_endpoints_count(),
    .on_running         = on_server_running,
    .on_stop            = on_server_stop,
    .on_error           = on_server_error,
    .on_request_success = on_command_action
  };
  CHECK_CRITICAL(ws_queue_init(), "WS queue fail");
  CHECK_CRITICAL(ws_dispatcher_init(), "WS disp fail");
  CHECK_CRITICAL(ws_cmd_router_init(on_command_action), "WS router fail");
  CHECK_CRITICAL(https_server_service_init(&https_cfg), "HTTPS fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 10. ethernet (w5500)
  ui_show_boot_progress("Init Ethernet", ++current_step, TOTAL_STEPS);
  eth_config_t eth_cfg = {
    .ip       = s_config.ip,
    .gateway  = s_config.gateway,
    .netmask  = s_config.netmask,
    .dns      = s_config.dns
  };
  eth_callbacks_t eth_callbacks = {
    .on_link_state_changed = sys_ind_led_eth_set_link,
    .on_packet_received = sys_ind_led_eth_packet_activity
  };
  CHECK_CRITICAL(eth_w5500_init(&eth_cfg, &eth_callbacks), "Ethernet fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  // 11. cyclic tasks
  ui_show_boot_progress("Init tasks", ++current_step, TOTAL_STEPS);
  CHECK_CRITICAL(cyclic_task_env_init(), "Tasks fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  ui_show_boot_progress("System ready!", ++current_step, TOTAL_STEPS);
  sys_ind_buzzer_sound(1, BUZZER_PERIOD_MS);
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS * 5));

  CHECK_CRITICAL(ui_manager_init(), "UI manager fail");
}
