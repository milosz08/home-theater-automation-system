#ifndef UI_ICON_H_
#define UI_ICON_H_

#include <stdint.h>

/*! \brief Custom character map for a progress bar block.
 *
 * Used to draw the loading bar during boot (5x8 pixels filled). Occupies slot 1 in LCD CGRAM.
 */
extern const uint8_t lcd_icon_rectangle_full[8];

/*! \brief Custom character map for the degree symbol (°).
 * 
 * Standard ASCII doesn't always have this on HD44780 controllers. Occupies slot 2 in LCD CGRAM.
 */
extern const uint8_t lcd_icon_degree[8];

#endif // UI_ICON_H_
