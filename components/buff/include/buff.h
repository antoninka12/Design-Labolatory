#ifndef BUFF_H
#define BUFF_H

#include <stdint.h>
#include "driver/i2c.h"


#define BUFFER_SIZE 32

/*typedef enum {
    EN_NONE=0,
    EN_JESC,
    EN_PIC,
    EN_NIE,
    EN_OK
} en_buff; */ //nie potrzebne - korzystamy z char i literek

void clear_buff(void);
void save_buff(char current); //char zamiast en_buff
void read_block_buff(void);
void send_buff(char x); //char

#endif