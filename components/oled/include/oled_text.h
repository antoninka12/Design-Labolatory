#ifndef OLED_TEXT_H
#define OLED_TEXT_H

#include <stdint.h> 
#include "driver/i2c.h" // for i2c_port_t
#include "esp_err.h"

esp_err_t oled_text_init(void);
esp_err_t oled_text_clear(void);
esp_err_t oled_text_put_char(char c); //wpisywanie char do bufora
esp_err_t oled_text_backspace(void); //cofanie znaku, nic nie robi ejsli jest na poczatku linii, nie da sie cofnac do poprzedniej linii
esp_err_t oled_text_newline(void); //nowa linia
esp_err_t oled_text_flush(i2c_port_t i2c_num); //wysylanie
#endif // OLED_TEXT_H