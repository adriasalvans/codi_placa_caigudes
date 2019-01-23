#ifndef spliter_h
#define spliter_h
#include <stdio.h>
#include <stdint.h>
#include <string.h>
# include <avr/io.h>
#include <stdio.h>
# include <util/delay.h>
typedef struct{
  char nom[15]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
  char potencia[4]={'\0','\0','\0','\0'};
} string_t;

string_t splits (char linia[]);

#endif
