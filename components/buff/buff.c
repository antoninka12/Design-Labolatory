#include <stdint.h>
#include "esp_timer.h" //aktualny czas w esp
#include <stdbool.h> //do zmiennych typu bool
#include "esp_log.h" 

#include "buff.h"
#include "oled_basic.h" //to tez nie bedzie potrzebne - A
#include "bitmap.h" //nie bedzie potrzebne -A
//dodane: -A
#include "oled_text.h"  //to -A
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define I2C_PORT I2C_NUM_0 //do I2C z kodu Tosi

static char buffer[BUFFER_SIZE]; //powołanie bufora
uint8_t position=0; //pozycja w buforze

//save buff
bool saved=false; //czy dana już zapisana
uint64_t start=0;//czas startu
uint64_t last_data=0; //ostatnie dane jakie dostał bufor



//read buff
bool read=false;

void clear_buff(){
    for(int i=0; i<BUFFER_SIZE; i++){
        buffer[i]='\0';
    }
    position=0;
    saved=false;
    start=0;
    last_data=0;

}

void save_buff(char current)
{
    uint64_t now = esp_timer_get_time() / 1000;//czas 
    
    if(current=='\0'){
        start=0;
        saved=false;
        //ESP_LOGI("BUFF", "pos=%u", position);
    }

    if(current!='\0' && start==0){
        start=now;
        //ESP_LOGI("BUFF", "stan=%llu", start);
    }

    if(!saved && current!='\0' && (now-start)>=4000){
        if(position<BUFFER_SIZE){
            buffer[position]=current;
            position++;
            ESP_LOGI("BUFF", "Zapis[%d] = %d", position, current);
            saved=true;
            last_data=0;
        }
    }

    if(current=='\0' && position>0 && last_data==0){
        last_data=now;
        ESP_LOGI("BUFF", "time");
    }

    if(position>0 && (now-last_data>=3000) && current=='\0'){
   
        ESP_LOGI("BUFF", "read");
        for(int i=0; i<position; i++){
            send_buff(buffer[i]);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        clear_buff();
        
    }
} //void


//wysyłanie po jednej rzeczy co sekundę
void read_block_buff(){
    read=true;

    
}

void send_buff(char x)
{
    /*switch(x){
        case EN_JESC: 
            ESP_ERROR_CHECK(oled_draw_bitmap(I2C_PORT, jesc_bitmap, jesc_bitmap_len)); break;
        case EN_PIC:  
            ESP_ERROR_CHECK(oled_draw_bitmap(I2C_PORT, pic_bitmap, pic_bitmap_len));  break;
        case EN_NIE:   
            ESP_ERROR_CHECK(oled_draw_bitmap(I2C_PORT, nie_bitmap, nie_bitmap_len));  break;
        case EN_OK:   
            ESP_ERROR_CHECK(oled_draw_bitmap(I2C_PORT, ok_bitmap, ok_bitmap_len));   break;
        default:       
            ESP_ERROR_CHECK(oled_clear(I2C_PORT)); break;
    }*/
   //nowa logika:
   if(x=='\0'){
    return; //pusty znak - nic nie robimy
   }
   ESP_ERROR_CHECK(oled_text_put_char(x));
   ESP_ERROR_CHECK(oled_text_flush(I2C_PORT));

}

