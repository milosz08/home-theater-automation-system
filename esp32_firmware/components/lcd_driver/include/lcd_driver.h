#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include <stdint.h>

#include "esp_err.h"

// internal bits for PCF8574 -> LCD mapping
#define LCD_RS_BIT          0x01
#define LCD_RW_BIT          0x02
#define LCD_EN_BIT          0x04
/*! \brief Bit responsible for the backlight (needs to be kept high to light up). */
#define LCD_BL_BIT          0x08

/*! \brief I2C address of the LCD backpack (usually 0x27 or 0x3F). */
#define LCD_I2C_ADDR        0x27

/*! \brief Wakes up and configures the LCD.
 *
 * Performs the magic HD44780 initialization sequence (nibble resets), sets 4-bit mode, configures the cursor (off), and
 * clears the screen.
 *
 * \retval ESP_OK   If the display responded.
 * \retval ESP_FAIL If no communication with display (also via I2C).
 */
esp_err_t lcd_driver_init(void);

/*! \brief Defines a custom character in the LCD's memory (CGRAM).
 *
 * Useful for icons like degrees, arrows, or smileys.
 *
 * \param location  Memory slot (0-7).
 * \param charmap   Array of 8 bytes defining the pixel map (5x8 dots).
 *
 * \retval ESP_OK   On success.
 * \retval ESP_FAIL On error.
 */
esp_err_t lcd_driver_create_char(uint8_t location, const uint8_t *charmap);

/*! \brief Sends a raw command to the LCD controller.
 *
 * Used for low-level control like moving cursor, clearing screen, etc.
 *
 * \param cmd The command byte.
 *
 * \retval ESP_OK   If all good.
 * \retval ESP_FAIL If I2C error or connection is lost.
 */
esp_err_t lcd_driver_cmd(uint8_t cmd);

/*! \brief Prints a text string to the current cursor position.
 *
 * Iterates through the string and sends each character as data to the display.
 * 
 * \param str Null-terminated string.
 *
 * \retval ESP_OK   If all good.
 * \retval ESP_FAIL If I2C error or connection is lost.
 */
esp_err_t lcd_driver_print(const char *str);

/*! \brief Moves the cursor to a specific spot.
 *
 * Handles the weird memory mapping of 16x2 / 20x4 displays automatically.
 *
 * \param col Column index (0-based).
 * \param row Row index (0 or 1).
 *
 * \retval ESP_OK On success.
 */
esp_err_t lcd_driver_set_cursor(uint8_t col, uint8_t row);

/*! \brief Wipes the screen clean.
 *
 * Sends the clear command and waits a bit (because this operation is slow on the LCD).
 *
 * \retval ESP_OK On success.
 */
esp_err_t lcd_driver_clear(void);

#endif // LCD_DRIVER_H_
