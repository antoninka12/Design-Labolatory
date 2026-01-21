#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"

#include "oled_basic.h"
#include "oled_text.h"
#include "buff.h"
#include "flex.h"

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


void app_main(void)
{   //musi być w pętli głównej:
    //initialization i2c master
    i2c_master_init(I2C_PORT, SDA_PIN, SCL_PIN, I2C_FRQ); //initialazing parameters from defines
   
    //musi być w pętli głównej:
    //initialization OLED
    ESP_LOGI(TAG, "OLED initialization");
    ESP_ERROR_CHECK(oled_init(I2C_PORT)); //checking for errors a
    ESP_ERROR_CHECK(oled_text_init()); //ustawianie kursorow

    ESP_ERROR_CHECK(oled_text_clear()); //czyszczenie na start
    ESP_ERROR_CHECK(oled_text_flush(I2C_PORT)); //wysylanie czystej bitmapy
  
    vTaskDelay(pdMS_TO_TICKS(500));

    flex_init(); //Wstępna konfiguracja czujnika

    const int n1=700; //próg 
    const int n2=400; //prog2
    clear_buff(); //wyczysć przed startem

    //const char text[] = "hello world"; //literki ktore chcemy wypisac -  symulacja podawania literek i wpiswywania ich
    //KORZYSTAMY Z BUFORA I SEND BUFF BO ZOSTALO TAM DODANY OLED
    while (1) {
        int thumb = flex_read5();
        if (thumb < n1) {
            oled_text_backspace();
            ESP_ERROR_CHECK(oled_text_flush(I2C_PORT));
            vTaskDelay(pdMS_TO_TICKS(200));   // debounce / antyspam
            continue;                         // NIE czytaj reszty czujników w tej iteracji
        }

        // 2) Wskazujący
        int index_finger = flex_read1();
        if (index_finger < n1 && index_finger > n2) {
            send_buff('A');
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        } else if (index_finger < n2) {
            send_buff('E');
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        // 3) Środkowy
        int middle_finger = flex_read2();
        if (middle_finger < n1 && middle_finger > n2) {
            send_buff('L');
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        } else if (middle_finger < n2) {
            send_buff('M');
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        // 4) Serdeczny
        int ring_finger = flex_read3();
        if (ring_finger < n1 && ring_finger > n2) {
            send_buff('L');
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        } else if (ring_finger < n2) {
            send_buff('M');
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        // 5) Mały
        int little_finger = flex_read4();
        if (little_finger < n1 && little_finger > n2) {
            send_buff('L');
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        } else if (little_finger < n2) {
            send_buff('M');
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

    // jeśli nic się nie stało
    vTaskDelay(pdMS_TO_TICKS(20));
        /*for (int i = 0; text[i] != '\0'; i++) {
            send_buff(text[i]);                 // symulacja pisania 
            vTaskDelay(pdMS_TO_TICKS(400));     // prędkość „pisania”
        }*/

        vTaskDelay(pdMS_TO_TICKS(3000));

        ESP_ERROR_CHECK(oled_text_clear()); //jesli chcemy wyczyscic
        ESP_ERROR_CHECK(oled_text_flush(I2C_PORT)); //wyslac, robimy to tak bo tego nie ma w buff.c

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
