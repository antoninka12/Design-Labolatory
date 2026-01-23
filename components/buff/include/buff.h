#ifndef BUFF_H
#define BUFF_H

#include <stdint.h>
#include "driver/i2c.h"


#define BUFFER_SIZE 32



void clear_buff(void);
void save_buff(char current); //char zamiast en_buff -A
void read_block_buff(void);
void send_buff(char x); //char -A

#endif