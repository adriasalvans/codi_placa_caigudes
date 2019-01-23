#ifndef FSM_Posicio_h
#define FSM_Posicio_h
#include <stdint.h>


typedef enum{Threshold, Time_out} event_t;

void posicio_init(void);
int send_event(event_t event);
void setBuffers(int x[],int y[],int z[]);

#endif
