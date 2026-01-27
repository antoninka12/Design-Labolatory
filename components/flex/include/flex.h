#ifndef FLEX_H
#define FLEX_H

void flex_init(void);
int  flex_read1(void);
int  flex_read2(void);
int  flex_read3(void);
int  flex_read4(void);
int  flex_read5(void);

bool flex_active(int v, int  n1);
bool only_one_active(int t, int i, int m, int r, int l);

#endif