#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_log.h"

//definicje wyjść adc i pin
#define FLEX_CH1 ADC1_CHANNEL_5
#define FLEX_GPIO1 33
#define FLEX_CH2 ADC1_CHANNEL_4
#define FLEX_GPIO2 32

void flex_init(void)
{
    //Reset starych ustawień
    gpio_reset_pin(FLEX_GPIO1);
    gpio_reset_pin(FLEX_GPIO2);

    //Ustawianie wej
    gpio_set_direction(FLEX_GPIO1, GPIO_MODE_INPUT);
    gpio_set_direction(FLEX_GPIO2, GPIO_MODE_INPUT);

    //Rozdzielczosc
    adc1_config_width(ADC_WIDTH_BIT_12); //12 bit
    adc1_config_channel_atten(FLEX_CH1, ADC_ATTEN_DB_12); //tłumienie 3,3V
    adc1_config_channel_atten(FLEX_CH2, ADC_ATTEN_DB_12); 
}
    
int flex_read1(void)
{
    return adc1_get_raw(FLEX_CH1); //Zwraca odczyt danych z flex
}

int flex_read2(void)
{
    return adc1_get_raw(FLEX_CH2); //Zwraca odczyt danych z flex
}

