#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_log.h"

//definicje wyjść adc i pin
//1-wskazujący
#define FLEX_CH1 ADC1_CHANNEL_5 
#define FLEX_GPIO1 33 
//2-środkowy
#define FLEX_CH2 ADC1_CHANNEL_4 
#define FLEX_GPIO2 32 
//3-serdeczny
#define FLEX_CH3 ADC1_CHANNEL_6 
#define FLEX_GPIO3 34
//4-mały
#define FLEX_CH4 ADC1_CHANNEL_7
#define FLEX_GPIO4 35
//5-kciuk
#define FLEX_CH5 ADC1_CHANNEL_0
#define FLEX_GPIO5 36

void flex_init(void)
{
    //Reset starych ustawień
    gpio_reset_pin(FLEX_GPIO1);
    gpio_reset_pin(FLEX_GPIO2);
    gpio_reset_pin(FLEX_GPIO3);
    gpio_reset_pin(FLEX_GPIO4);
    gpio_reset_pin(FLEX_GPIO5);

    //Ustawianie wej
    gpio_set_direction(FLEX_GPIO1, GPIO_MODE_INPUT);
    gpio_set_direction(FLEX_GPIO2, GPIO_MODE_INPUT);
    gpio_set_direction(FLEX_GPIO3, GPIO_MODE_INPUT);
    gpio_set_direction(FLEX_GPIO4, GPIO_MODE_INPUT);
    gpio_set_direction(FLEX_GPIO5, GPIO_MODE_INPUT);

    //Rozdzielczosc
    adc1_config_width(ADC_WIDTH_BIT_12); //12 bit
    adc1_config_channel_atten(FLEX_CH1, ADC_ATTEN_DB_12); //tłumienie 3,3V
    adc1_config_channel_atten(FLEX_CH2, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(FLEX_CH3, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(FLEX_CH4, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(FLEX_CH5, ADC_ATTEN_DB_12);
}
    
int flex_read1(void)
{
    return adc1_get_raw(FLEX_CH1); //Zwraca odczyt danych z flex
}

int flex_read2(void)
{
    return adc1_get_raw(FLEX_CH2); //Zwraca odczyt danych z flex
}

int flex_read3(void)
{
    return adc1_get_raw(FLEX_CH3); //Zwraca odczyt danych z flex
}

int flex_read4(void)
{
    return adc1_get_raw(FLEX_CH4); //Zwraca odczyt danych z flex
}

int flex_read5(void)
{
    return adc1_get_raw(FLEX_CH5); //Zwraca odczyt danych z flex
}

bool flex_active(int v, int  n1)
{
    return (v < n1);
}

bool only_one_active(int t, int i, int m, int r, int l, int n1){
    int add=0;
    if(flex_active(t, n1)) add++;
    if(flex_active(i, n1)) add++;
    if(flex_active(m, n1)) add++;
    if(flex_active(r, n1)) add++;
    if(flex_active(l, n1)) add++;
    return (add==1);
}