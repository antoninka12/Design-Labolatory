#ifndef BUFF_H
#define BUFF_H

#include <stdint.h>
#include "driver/i2c.h"


#define BUFFER_SIZE 32

typedef enum {
    EN_NONE=0,
    EN_JESC,
    EN_PIC,
    EN_NIE,
    EN_OK
} en_buff; 

void clear_buff(void);
void save_buff(en_buff current);
void read_block_buff(void);
void send_buff(en_buff x);

#endif