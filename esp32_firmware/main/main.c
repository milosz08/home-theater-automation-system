#include "app_api.h"
#include "env_sensor.h"
#include "eth_w5500.h"
#include "https_server.h"
#include "i2c_bus.h"
#include "io_expander.h"
#include "io_input.h"
#include "led_ind.h"
#include "nvs_manager.h"
#include "storage.h"
#include "uart_bus.h"
#include "ui_manager.h"
#include "ui.h"

#include <stddef.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define CHECK_CRITICAL(x, msg) do { \
  esp_err_t err_rc = (x); \
  if (err_rc != ESP_OK) { \
    led_ind_set_error(true); \
    ui_show_critical_error(msg, err_rc); \
  } \
} while(0)

#define PROGRESS_BAR_COOLDOWN_MS  100   // time between next steps
#define TOTAL_STEPS               10    // total steps
#define REQ_MESS_DURATION_MS      2000  // request success message duration

// pcf8574 expander buttons and other io inputs (4-7)
#define PCF_PIN_IN_BTN_RESET      4
#define PCF_PIN_IN_BTN_MENU       5

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "MAIN";

static void on_server_running(void)
{
  led_ind_set_error(false);
  ui_restore_screen();
}

static void on_server_stop(void)
{
  led_ind_set_error(true);
  ui_show_error("Net: link DOWN");
}

static void on_server_error(esp_err_t err)
{
  led_ind_set_error(true);
  ui_show_error("Server fail");
}

static void on_server_req_success(const char *endpoint_name)
{
  led_ind_io_cmd_execution();
  if (endpoint_name != NULL) ui_show_temp_cmd_message(endpoint_name, REQ_MESS_DURATION_MS);
}

static void on_eth_link_state_changed(bool is_up)
{
  led_ind_eth_set_link(is_up);
}

static void on_eth_packet_received(void)
{
  led_ind_eth_packet_activity();
}

static void on_reset_btn_click(io_input_action_t action)
{
  if (action == BTN_CLICK_LONG)
  {
    char default_password[64] = {0};
    esp_err_t err = nvs_manager_load_str(AUTH_NVS_NS, AUTH_NVS_DEFAULT_KEY, default_password, sizeof(default_password));
    if (err != ESP_OK) return;

    err = nvs_manager_save_str(AUTH_NVS_NS, AUTH_NVS_KEY, default_password);
    if (err != ESP_OK) return;

    ui_show_temp_cmd_message("Password reset", REQ_MESS_DURATION_MS);
  }
}

static void on_menu_btn_click(io_input_action_t action)
{
  if (action == BTN_CLICK_LONG) ui_manager_switch_mode();
  else if (action == BTN_CLICK_SHORT) ui_manager_manual_mode();
}

// public api ----------------------------------------------------------------------------------------------------------

void app_main(void)
{
  ESP_LOGI(TAG, "System starting...");

  int current_step = 0;
  system_config_t s_config;

  // 1. I2C bus and expander init (before LCD)
  ESP_ERROR_CHECK(i2c_bus_init());
  ESP_ERROR_CHECK(io_expander_init(0xF0)); // 11110000 (in: 7,6,5,4; out: 3,2,1,0)
  ESP_ERROR_CHECK(led_ind_init());

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
  const io_input_config_t inputs[] = {
    { .pin = PCF_PIN_IN_BTN_RESET, .callback = on_reset_btn_click, .name = "RESET" },
    { .pin = PCF_PIN_IN_BTN_MENU, .callback = on_menu_btn_click,  .name = "MENU" }
  };
  CHECK_CRITICAL(io_input_init(inputs, sizeof(inputs) / sizeof(inputs[0])), "IO input fail");
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
    .on_request_success = on_server_req_success
  };
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
    .on_link_state_changed = on_eth_link_state_changed,
    .on_packet_received = on_eth_packet_received
  };
  CHECK_CRITICAL(eth_w5500_init(&eth_cfg, &eth_callbacks), "Ethernet fail");
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS));

  ui_show_boot_progress("System ready!", ++current_step, TOTAL_STEPS);
  vTaskDelay(pdMS_TO_TICKS(PROGRESS_BAR_COOLDOWN_MS * 5));

  CHECK_CRITICAL(ui_manager_init(), "UI manager fail");
}
