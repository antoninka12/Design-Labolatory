#ifndef BUFF_H
#define BUFF_H

#include <stdint.h>

#define BUFFER_SIZE 32

typedef enum {
    EN_NONE=0,
    EN_JESC,
    EN_PIC
} en_buff; 

extern en_buff buffer[BUFFER_SIZE]; //powołanie bufora z c
extern uint8_t position;

void clear_buff(void);
void save_value(en_buff val);


#endif