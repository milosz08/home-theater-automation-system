#include "peripheral_control.h"
#include "uart_bus.h"

#include <stdint.h>

// private api ---------------------------------------------------------------------------------------------------------

static const uint8_t CMD_SCREEN_DOWN[] = {
  0xFF, 0xEE, 0xEE, 0xEE, 0xEE, 0xFF, 0xEE, 0xEE, 0xEE, 0xEE, 0xFF, 0xEE, 0xEE,
  0xEE, 0xEE, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x28, 0xF4, 0x12, 0x00, 0x00
};

static const uint8_t CMD_SCREEN_STOP[] = {
  0xFF, 0xEE, 0xEE, 0xEE, 0xCC, 0xFF, 0xEE, 0xEE, 0xEE, 0xCC, 0xFF, 0xEE, 0xEE,
  0xEE, 0xCC, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x28, 0xF4, 0x12, 0x00, 0x00
};

static const uint8_t CMD_SCREEN_UP[] = {
  0xFF, 0xEE, 0xEE, 0xEE, 0xDD, 0xFF, 0xEE, 0xEE, 0xEE, 0xDD, 0xFF, 0xEE, 0xEE,
  0xEE, 0xDD, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x28, 0xF4, 0x12, 0x00, 0x00
};

// ~0000 1\r
static const uint8_t CMD_PROJECTOR_ON[] = {
  0x7E, 0x30, 0x30, 0x30, 0x30, 0x20, 0x31, 0x0D
};

// ~0000 0\r
static const uint8_t CMD_PROJECTOR_OFF[] = {
  0x7E, 0x30, 0x30, 0x30, 0x30, 0x20, 0x30, 0x0D
};

//  ~0004 1\r
static const uint8_t CMD_AV_MUTE_ON[] = {
  0x7E, 0x30, 0x30, 0x30, 0x34, 0x20, 0x31, 0x0D
};

//  ~0004 0\r
static const uint8_t CMD_AV_MUTE_OFF[] = {
  0x7E, 0x30, 0x30, 0x30, 0x34, 0x20, 0x30, 0x0D
};

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t peripheral_control_screen_down(void)
{
  return uart_bus_rs485_send(CMD_SCREEN_DOWN, sizeof(CMD_SCREEN_DOWN));
}

esp_err_t peripheral_control_screen_stop(void)
{
  return uart_bus_rs485_send(CMD_SCREEN_STOP, sizeof(CMD_SCREEN_STOP));
}

esp_err_t peripheral_control_screen_up(void)
{
  return uart_bus_rs485_send(CMD_SCREEN_UP, sizeof(CMD_SCREEN_UP));
}

esp_err_t peripheral_control_projector_on(void)
{
  return uart_bus_rs232_send(CMD_PROJECTOR_ON, sizeof(CMD_PROJECTOR_ON));
}

esp_err_t peripheral_control_projector_off(void)
{
  return uart_bus_rs232_send(CMD_PROJECTOR_OFF, sizeof(CMD_PROJECTOR_OFF));
}

esp_err_t peripheral_control_av_mute_on(void)
{
  return uart_bus_rs232_send(CMD_AV_MUTE_ON, sizeof(CMD_AV_MUTE_ON));
}

esp_err_t peripheral_control_av_mute_off(void)
{
  return uart_bus_rs232_send(CMD_AV_MUTE_OFF, sizeof(CMD_AV_MUTE_OFF));
}
