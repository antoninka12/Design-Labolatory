#include <stdio.h> // for printf 

#include "esp_log.h"
#include "esp_err.h"

#include "oled_text.h"
#include "oled_basic.h"
#include "X11fixed7x14.h"
#include <string.h>

#define TAG "OLED_TEXT" //tag for logs

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGES (OLED_HEIGHT / 8) //each page is 8 pixels height
 
#define FONT_WIDTH 6
#define FONT_HEIGHT 14
#define FONT_PAGES 2 //14 pixels height -> 2 pages (8+6) 

//static __uint8_t font_buffer[FONT_WIDTH * FONT_PAGES]; //buffer for one character
static uint8_t oled_buffer[OLED_WIDTH * OLED_PAGES]; //buffer for whole oled

static uint8_t cursor_x=0; //position x for cursor 
static uint8_t cursor_page=0; //there is two pages for one character - 14(16) pixels

esp_err_t oled_text_clear(void)
{
    memset(oled_buffer, 0x00, sizeof(oled_buffer));
    cursor_x = 0;
    cursor_page = 0;
    return ESP_OK;
}

esp_err_t oled_text_init(void)
{
    return oled_text_clear();
}
//ogarnac:
static esp_err_t oled_text_draw_char(uint8_t x, uint8_t page, char c)
{
   if(c < 0x20 || c >= 0x7F){
       ESP_LOGE(TAG, "Invalid character");
         return ESP_ERR_INVALID_ARG;
        }
   if(page+1 >= OLED_PAGES){
             ESP_LOGE(TAG, "Page out of range");
             return ESP_ERR_INVALID_SIZE;
         }
    uint16_t glyph_offset = 6 + (c - 0x20) * 12;
    const uint8_t *glyph = &X11fixed7x14[glyph_offset];

    uint16_t index_low  = page * OLED_WIDTH + x;
    uint16_t index_high = (page + 1) * OLED_WIDTH + x;

    for (uint8_t col = 0; col < FONT_WIDTH; col++) {
        oled_buffer[index_low  + col] = glyph[col];
        oled_buffer[index_high + col] = glyph[col + FONT_WIDTH];
    }

    return ESP_OK;
}
esp_err_t oled_text_newline(void)
{
    cursor_x = 0;
    cursor_page += FONT_PAGES;

    if (cursor_page >= OLED_PAGES) {
        cursor_page = 0;
        ESP_LOGW(TAG, "Cursor wrapped to top");
    }

    return ESP_OK;
}
esp_err_t oled_text_put_char(char c){
    if (c == '\n') {
    return oled_text_newline();
    }
    if (cursor_x > (OLED_WIDTH - FONT_WIDTH)) {
        cursor_x = 0;
        cursor_page += FONT_PAGES;
    }
    if (cursor_page + 1 >= OLED_PAGES) {
        ESP_LOGW(TAG, "Screen full, cursor reset");
        cursor_page = 0;
    }
    esp_err_t ret = oled_text_draw_char(cursor_x, cursor_page, c);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to draw char");
        return ret;
    }

    cursor_x += FONT_WIDTH;
    return ESP_OK;
}


esp_err_t oled_text_backspace(void)
{
    if (cursor_x < FONT_WIDTH) {
        return ESP_OK;
    }

    cursor_x -= FONT_WIDTH;

    uint16_t index_low  = cursor_page * OLED_WIDTH + cursor_x;
    uint16_t index_high = (cursor_page + 1) * OLED_WIDTH + cursor_x;

    memset(&oled_buffer[index_low],  0x00, FONT_WIDTH);
    memset(&oled_buffer[index_high], 0x00, FONT_WIDTH);

    return ESP_OK;
}
esp_err_t oled_text_flush(i2c_port_t i2c_num)
{
    esp_err_t ret = oled_draw_bitmap(i2c_num, oled_buffer, sizeof(oled_buffer));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to flush buffer to OLED");
        return ret;
    }

    return ESP_OK;
}