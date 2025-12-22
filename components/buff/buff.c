#include <stdint.h>
#include "esp_timer.h" //aktualny czas w esp
#include <stdbool.h> //do zmiennych typu bool

#include "buff.h"

en_buff buffer[BUFFER_SIZE]; //powołanie bufora
uint8_t position=0; //pozycja w buforze

//do przytrzymania 2s
bool count=false; //czy już liczymy czas
uint64_t start=0;//czas startu
uint64_t last_data=0; //ostatnie dane jakie dostał bufor
en_buff last=EN_NONE; 

void clear_buff(){
    for(int i=0; i<BUFFER_SIZE; i++){
        buffer[i]=EN_NONE;
    }
    position=0;
    count=false;
    start=0;
    last_data=0;
    last=EN_NONE;
}

void save_buff(en_buff current)
{
    uint64_t now = esp_timer_get_time() / 1000;//czas

    //1. Nie włączony curr=0, last=0 
    if(current==EN_NONE){
        last=EN_NONE;
        count=false;
        start=0;
    } else { //2. zgięcie curr=1 last=0
        last_data = 0;
        if(last==EN_NONE){
            last=current;
            start=now;
            count=true;
        }
    }

    //3.2. jeśli zmienił się stan przy liczeniu
    if(current!=EN_NONE && last!=current){
        last=current;
        start=now;
        count=true;
    }

    //3.1. przytrzymanie przez 2s
    if(count && (now-start>=2000)){
        if(position<BUFFER_SIZE){
            buffer[position]=current;
            position++;
        }
        count=false; //i już nie licz
    }

    if(current==EN_NONE && position>0 && last_data==0){
        last_data=now;
    }

    if (current==EN_NONE && position>0 && 
        last_data!=0 && now-last_data>=5000){
        //read_buff();
        clear_buff();
    }
    
} //void


/*int read_buff(){
    for(int i=0; i<position; i++){
        return buffer[i];
    }
}*/

