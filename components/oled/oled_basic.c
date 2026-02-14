#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "driver/i2c.h"
#include "oled_basic.h"

#define TAG "OLED_BASIC" //tag for logs  (esp_log.h)

#define OLED_ADDR 0x3C 

static esp_err_t oled_write_cmd(i2c_port_t i2c_num, uint8_t cmd) 
{
    uint8_t data[2]={0x00, cmd}; //firdt byte is control byte, there will be command after it and every command need only 1 byte
    //comendd/; f.e turn on/off 
    esp_err_t ret= i2c_master_write_to_device(i2c_num, OLED_ADDR, data, sizeof(data), pdMS_TO_TICKS(100));
    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to write cmd to oled: %s", esp_err_to_name(ret));
    }
    return ret; 
}

static esp_err_t oled_write_data(i2c_port_t i2c_num,const uint8_t* data, size_t data_len){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); //create command link - alocates resources for I2C commands
    esp_err_t ret;
    ret= i2c_master_start(cmd); //start condition, adding START to cmd
    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to create start condition: %s", esp_err_to_name(ret));
        return ret;
    }
    ret=i2c_master_write_byte(cmd, (OLED_ADDR <<1) | I2C_MASTER_WRITE, true); //write addr with write bit
     if(ret!=ESP_OK){ //true - expect ack
        ESP_LOGE(TAG, "Failed to write addres: %s", esp_err_to_name(ret));
        return ret;
    }
    ret=i2c_master_write_byte(cmd, 0x40, true); //control byte for data
     if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to write control byte for data: %s", esp_err_to_name(ret));
        return ret;
    }
    ret=i2c_master_write(cmd, (uint8_t*) data, data_len, true); //control byte for data
     if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to write data: %s", esp_err_to_name(ret));
        return ret;
    }
    ret=i2c_master_stop(cmd); //stop 
    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to stop %s", esp_err_to_name(ret));
        return ret;
    }

    ret=i2c_master_cmd_begin(i2c_num, cmd, pdMS_TO_TICKS(100));//send cmd link to i2c bus
    i2c_cmd_link_delete(cmd); //free resources
     if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to send data %s", esp_err_to_name(ret));
        return ret;
    }
    return ESP_OK;
}

    //initialize oled :((( using oled_writew_cmd we need to set up the oled for it to work prperly 
esp_err_t oled_init(i2c_port_t i2c_num){
    oled_write_cmd(i2c_num, 0xAE); // Display off (sleep mode)
    oled_write_cmd(i2c_num, 0x20); // Set Memory Addressing Mode
    oled_write_cmd(i2c_num, 0x00); // Horizontal addressing mode
    oled_write_cmd(i2c_num, 0xB0); // Page start address
    oled_write_cmd(i2c_num, 0xC8); // COM output scan direction // z c8
    oled_write_cmd(i2c_num, 0x00); // Low column
    oled_write_cmd(i2c_num, 0x10); // High column
    oled_write_cmd(i2c_num, 0x40); // Start line
    oled_write_cmd(i2c_num, 0x81); 
    oled_write_cmd(i2c_num, 0x7F); // Contrast
    oled_write_cmd(i2c_num, 0xA1); // Segment remap, z a1
    oled_write_cmd(i2c_num, 0xA6); // Normal display
    oled_write_cmd(i2c_num, 0xA8); 
    oled_write_cmd(i2c_num, 0x3F); // Multiplex ratio
    oled_write_cmd(i2c_num, 0xA4); // Display follows RAM content
    oled_write_cmd(i2c_num, 0xD3); 
    oled_write_cmd(i2c_num, 0x00); // Display offset
    oled_write_cmd(i2c_num, 0xD5); 
    oled_write_cmd(i2c_num, 0x80); // Clock divide
    oled_write_cmd(i2c_num, 0xD9);
    oled_write_cmd(i2c_num, 0xF1); // Pre-charge
    oled_write_cmd(i2c_num, 0xDA); 
    oled_write_cmd(i2c_num, 0x12); // COM pins config
    oled_write_cmd(i2c_num, 0xDB); 
    oled_write_cmd(i2c_num, 0x40); // VCOM detect
    oled_write_cmd(i2c_num, 0x8D); 
    oled_write_cmd(i2c_num, 0x14); // Enable charge pump
    oled_write_cmd(i2c_num, 0xAF); // Display ON

    ESP_LOGI(TAG, "OLED initialized");
    return ESP_OK;
}

//oled clear, one page=one byte = 8pixels vertically, 128 pages horizontally
esp_err_t oled_clear(i2c_port_t i2c_num){
    uint8_t clear_data[128]; //our width is 128 pixels 
    memset(clear_data, 0x00, sizeof(clear_data)); //fill array with zeros 

    for(uint8_t page=0; page<8; page++){ //8 pages vertically 
        oled_write_cmd(i2c_num, 0xB0 + page); //set page address 0XB0 - Set Page Start Address (from 0 to 7), ustawia która strone w pamieci chcemy zapsiac
        oled_write_cmd(i2c_num, 0x00); //set low column address 
        oled_write_cmd(i2c_num, 0x10); //set high column address 
        oled_write_data(i2c_num, clear_data, sizeof(clear_data)); //writing data to oled
    }
    return ESP_OK;
}

esp_err_t oled_draw_bitmap(i2c_port_t i2c_num, const uint8_t* bitmap, size_t size) {
    for (uint8_t page = 0; page < 8; page++) {
        oled_write_cmd(i2c_num, 0xB0 + page);
        oled_write_cmd(i2c_num, 0x00);
        oled_write_cmd(i2c_num, 0x10);
        oled_write_data(i2c_num, bitmap + (page * 128), 128);
    }
    return ESP_OK;
}


