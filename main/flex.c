#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_log.h"

//Pierwszy
#define LED 18
#define FLEX_CH ADC1_CHANNEL_5
#define FLEX_GPIO 33

void app_main(void)
{
    //Reset starych ustawień
    gpio_reset_pin(LED);
    gpio_reset_pin(FLEX_GPIO);

    //Ustawianie wej wyj
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(FLEX_GPIO, GPIO_MODE_INPUT);

    //Rozdzielczosc
    adc1_config_width(ADC_WIDTH_BIT_12); //12 bit
    adc1_config_channel_atten(FLEX_CH, ADC_ATTEN_DB_12); //tłumienie 3,3V

    while(1){
        int read=adc1_get_raw(FLEX_CH);
        int threshold=700;

        if(read<threshold){
            gpio_set_level(LED, 1);
            ESP_LOGI("LED", "OFF %d", read);
            vTaskDelay(pdMS_TO_TICKS(2000));
        } else {
            gpio_set_level(LED, 0);
            ESP_LOGI("LED", "OFF %d", read);
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}
