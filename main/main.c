#include <stdio.h>
#include "ssd1306.h"
//#include "driver/i2c_master.h"
#include "driver/i2c.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define SDA_PIN 21 //pin number for SDA
#define SCL_PIN 22 //for SCL 
#define I2C_FRQ 400000 //frq= 400kHz
#define I2C_PORT I2C_NUM_0 //first port for I2C 
#define OLED_ADDR 0x3C 

static void i2c_master_init(i2c_port_t i2c_num, gpio_num_t sda_io, gpio_num_t scl_io, uint32_t clk_speed){
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER, //master mode is selected for ESP32 (esp32- master, oled-slave)   
        .sda_io_num = sda_io, //port number for sda as parameter 
        .scl_io_num = scl_io,//port number for scl as parameter 
        .sda_pullup_en = GPIO_PULLUP_ENABLE, //pull-up resistor for sda enabled 
        .scl_pullup_en = GPIO_PULLUP_ENABLE, //pull-up resistor for scl enabled 
        .master.clk_speed = clk_speed, //clocl speed as parameter 
    };
    i2c_param_config(i2c_num, &conf); 
    i2c_driver_install(i2c_num, conf.mode, 0, 0, 0); 
    }

void app_main(void)
{
i2c_master_init(I2C_PORT, SDA_PIN, SCL_PIN, I2C_FRQ); //initialazing parameters from defines

ssd1306_handle_t oled = ssd1306_create(I2C_PORT, OLED_ADDR); //for OlLED addres is 0x3C 


ssd1306_clear_screen(oled, 0x00);
    ssd1306_draw_string(oled, 0, 0, (const uint8_t*)"Hello OLED!", 16, 1);
    ssd1306_draw_line(oled, 0, 63, 127, 0);
    ssd1306_refresh_gram(oled);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}