#include <stdio.h> // for printf 

#include "esp_log.h"
#include "esp_err.h"

#include "oled_text.h"
#include "oled_basic.h"
#include "X11fixed7x14.h" //font
#include <string.h>

#define TAG "OLED_TEXT" //tag for logs

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGES (OLED_HEIGHT / 8) //each page is 8 pixels height = 8 pages
 
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
} //sets buffor to zeros, and cursord to zeros

esp_err_t oled_text_init(void)
{
    return oled_text_clear();
} //same as clear but more things can be add in future

//drawing character , x- columns 
static esp_err_t oled_text_draw_char(uint8_t x, uint8_t page, char c)
{
   if(c < 0x20 || c >= 0x7F){ //from X11fixed7x14.h
       ESP_LOGE(TAG, "Invalid character");
         return ESP_ERR_INVALID_ARG;
        }
   if(page+1 >= OLED_PAGES){
             ESP_LOGE(TAG, "Page out of range");
             return ESP_ERR_INVALID_SIZE;
         }
    uint16_t start_offset = 6 + (c - 0x20) * 12; //(c - 0x20)
    // -  zmiana na to ktory jest w tablicy, kazdy znak to 12 bajtow, wiec mnozymy aby odnalezc poczatek
    const uint8_t *wsk = &X11fixed7x14[start_offset]; //wskaznik do tej tablicy od poczatku znaku "c", wskazuje na te 12 bajtów

    uint16_t index_low  = page * OLED_WIDTH + x; //potrzebujemy dwoch stron na jeden znak
    uint16_t index_high = (page + 1) * OLED_WIDTH + x;

    for (uint8_t col = 0; col < FONT_WIDTH; col++) {
        oled_buffer[index_low  + col] = wsk[col];
        oled_buffer[index_high + col] = wsk[col + FONT_WIDTH];
    }//zapis znaku do bufora 

    return ESP_OK;
}
esp_err_t oled_text_newline(void)
{
    cursor_x = 0;
    cursor_page += FONT_PAGES; //zchodzenie w dół o jedna linie znaku : 2 strony 

    if (cursor_page >= OLED_PAGES) {
        cursor_page = 0;
        ESP_LOGW(TAG, "Cursor wrapped to top");
    }

    return ESP_OK;
}
esp_err_t oled_text_put_char(char c){
    if (c == '\n') {
    return oled_text_newline(); //przejscie do nowej linii jak \n
    }
    if (cursor_x > (OLED_WIDTH - FONT_WIDTH)) { //sprawdzenie czy znak zmiesci sie w poziomie, jak nie to nastepna linia
        cursor_x = 0;
        cursor_page += FONT_PAGES;
    }
    if (cursor_page + 1 >= OLED_PAGES) { //sprawdzenie czy w pionie jest miejsce, czy jest jeszcze strona, czysci jak pełne
        ESP_LOGW(TAG, "Screen is full, cursor reset");
        return oled_text_clear(); 
    }
    esp_err_t ret = oled_text_draw_char(cursor_x, cursor_page, c); //wywolanie rysowania, x to kolumny, y to wiersze, c to znak
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to draw char"); //sprawdzenie
        return ret;
    }

    cursor_x += FONT_WIDTH; //po narysowaniu znaku przesuwamy kursor x
    return ESP_OK;
}


esp_err_t oled_text_backspace(void) //cofanie literki
{
    if (cursor_x < FONT_WIDTH) { //jesli jest na poczatku to nie ma co usuwac, dziala tylko w danej linii
        return ESP_OK;
    }

    cursor_x -= FONT_WIDTH; //przesuwa o szerokosc jednego znaku 

    uint16_t index_low  = cursor_page * OLED_WIDTH + cursor_x;
    uint16_t index_high = (cursor_page + 1) * OLED_WIDTH + cursor_x;

    memset(&oled_buffer[index_low],  0x00, FONT_WIDTH); //czyszczenie
    memset(&oled_buffer[index_high], 0x00, FONT_WIDTH);

    return ESP_OK;
}
esp_err_t oled_text_flush(i2c_port_t i2c_num)
{
    esp_err_t ret = oled_draw_bitmap(i2c_num, oled_buffer, sizeof(oled_buffer)); //rysowanie bitmapy
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to flush buffer to OLED");
        return ret;
    }

    return ESP_OK;
}