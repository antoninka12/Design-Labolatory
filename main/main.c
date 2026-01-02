#include <stdio.h> 
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h" //do logów
#include "esp_err.h"

#include "oled_basic.h" 
#include "bitmap.h"
#include "flex.h"

//wszystkie includy to must have zeby działało

#define TAG "MAIN" //w mainie korzystam z logów i tagów, w razie błędów można odczytac i jesli bedzie w pliku main to bedzie wlasnie to napisane

//musza byc te definicje do dzialania kodow(to po prostu stałe takie jak piny itp): 
// configuration of i2c 
#define SDA_PIN 21      //pin number for SDA
#define SCL_PIN 22      //for SCL 
#define I2C_PORT I2C_NUM_0 //first port for i2c
#define I2C_FRQ 400000  // frq=400kHz

//musi być:  jest to funkcja która włączy ci i2c w trybie master i bedzie sie mozna komunikowac z oledem, korzysta z tych stałych z #define 
static esp_err_t i2c_master_init(i2c_port_t i2c_num, gpio_num_t sda_io, gpio_num_t scl_io, uint32_t clk_speed){
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER, //master mode is selected for ESP32 (esp32- master, oled-slave)   
        .sda_io_num = sda_io, //port number for sda as parameter 
        .scl_io_num = scl_io,//port number for scl as parameter 
        .sda_pullup_en = GPIO_PULLUP_ENABLE, //pull-up resistor for sda enabled 
        .scl_pullup_en = GPIO_PULLUP_ENABLE, //pull-up resistor for scl enabled 
        .master.clk_speed = clk_speed, //clocl speed as parameter 
    };
    i2c_param_config(i2c_num, &conf); //setting i2c parameters from i2c_master_init_function
    i2c_driver_install(i2c_num, conf.mode, 0, 0, 0); //installing i2c driver with parameters
    return ESP_OK;
} //jak dotąd wszystko po prostu kopiować 

void app_main(void){
    //musi być w pętli głównej:
    //initialization i2c master
    i2c_master_init(I2C_PORT, SDA_PIN, SCL_PIN, I2C_FRQ); //initialazing parameters from defines
   
    //musi być w pętli głównej:
    //initialization OLED
    ESP_LOGI(TAG, "OLED initialization");
    ESP_ERROR_CHECK(oled_init(I2C_PORT)); //checking for errors a

    //warto zawsze na poczatek bo wyczysci ekran
    //clearing OLED display
    ESP_LOGI(TAG, "clearing OLED display");
    ESP_ERROR_CHECK(oled_clear(I2C_PORT));
    vTaskDelay(pdMS_TO_TICKS(500));

    flex_init(); //Wstępna konfiguracja czujnika

    const int n1=700; //próg 
    const int n2=300; //prog2
    
    while(1){
        int data1=flex_read1(); //zmienna na odczyt1
        int data2=flex_read2(); //odczyt 2
        
        if(data1<n1 && data1>n2){
            ESP_LOGI(TAG, "drawing JESC");
            ESP_ERROR_CHECK(oled_draw_bitmap(I2C_PORT, jesc_bitmap, jesc_bitmap_len));
            vTaskDelay(pdMS_TO_TICKS(300));
        } else if(data1<n2) {
            ESP_LOGI(TAG, "drawing PIC");
            ESP_ERROR_CHECK(oled_draw_bitmap(I2C_PORT, pic_bitmap, pic_bitmap_len));
            vTaskDelay(pdMS_TO_TICKS(300));
        }else if(data2<n1 && data2>n2){
            ESP_LOGI(TAG, "drawing NIE");
            ESP_ERROR_CHECK(oled_draw_bitmap(I2C_PORT, nie_bitmap, nie_bitmap_len));
            vTaskDelay(pdMS_TO_TICKS(300));
        }else if(data2<n2) {
            ESP_LOGI(TAG, "drawing OK");
            ESP_ERROR_CHECK(oled_draw_bitmap(I2C_PORT, ok_bitmap, ok_bitmap_len));
            vTaskDelay(pdMS_TO_TICKS(300));
        } else {
            ESP_LOGI(TAG, "clearing OLED display");
            ESP_ERROR_CHECK(oled_clear(I2C_PORT));
            vTaskDelay(pdMS_TO_TICKS(300));
        }
    }


} //kikat 
//opis funkcjonalnosc, ciekawy kod
//prezentacja ze slajdami/ jak projekt na inzynierke na 10min
//filmik jako pokaz działania z tekstem albo głosem

//obsługa 

