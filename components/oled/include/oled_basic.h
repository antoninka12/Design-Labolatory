#ifndef OLED_BASIC_H
#define OLED_BASIC_H

#include "esp_err.h"      // dla esp_err_t
#include "driver/i2c.h"   // dla i2c_port_t
#include <stddef.h>       // dla size_t

#ifdef __cplusplus
extern "C" {
#endif

//initialize oled - must have before using other functions
esp_err_t oled_init(i2c_port_t i2c_num);

// celaring oled
esp_err_t oled_clear(i2c_port_t i2c_num);

// bitmap display, using here function from bitmap.h 
esp_err_t oled_draw_bitmap(i2c_port_t i2c_num, const uint8_t* bitmap, size_t size);

#ifdef __cplusplus
}
#endif

#endif // OLED_BASIC_H