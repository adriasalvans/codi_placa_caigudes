
#include <stdio.h>
#include <stdint.h>

#include "FSM_Posicio.h"

#define RANGE 11
#define LLINDAR_1 0.7
#define LLINDAR_2 0.5
#define G2ADXL(X) (uint8_t)(X/0.0078)
#define LLINDAR1 G2ADXL(LLINDAR_1)
#define LLINDAR2 G2ADXL(LLINDAR_2)


typedef struct {
  uint8_t x_range[RANGE];
  uint8_t y_range[RANGE];
  uint8_t z_range[RANGE];
  uint8_t values_range[RANGE];
} range_t;

typedef enum {Desconegut, Tumbat, Erecte} posicio_t;

typedef enum {s0, s1} estat_t;


//PROTOTIPES

static void reset_buffer(void);
static posicio_t update_posicio(void);
static int check_fall(void);
static int run_FSM(event_t event);
static uint8_t maxim (const int *llista);



// GLOBAL VARIABLES
static range_t buffer;
static posicio_t posicio_actual;
static posicio_t posicio_anterior;



static uint8_t maxim (const uint8_t *llista) {
  uint8_t maxValue = 0;
  uint8_t posicio = 0;



  for (uint8_t i = 0; i < RANGE; i++) {
    if (llista[i] >= maxValue) {

      maxValue = llista[i];
      posicio = i;
    }
  }

  return posicio;
}


int send_event(event_t event)
{
  int caiguda=0;
  caiguda=run_FSM(event);
  return caiguda;
}

void posicio_init(void)
{

  float x = 0;

  //setup_ESP01("Redmi","ohyq8158","192.168.43.116", "5000");

  for (int i = 0 ; i < RANGE; i++)
  {
    buffer.values_range[i] = G2ADXL(x);
    x += 0.1;
  }
  reset_buffer();

  posicio_anterior = Desconegut;
  posicio_actual = Desconegut;
}


static void reset_buffer(void)
{
  for (int i = 0 ; i < RANGE; i++)
  {
    buffer.x_range[i] = 0;
    buffer.y_range[i] = 0;
    buffer.z_range[i] = 0;
  }
}

void setBuffers(int x[], int y[], int z[]) // int -> uint8_t
{
  for (uint8_t i = 0; i < RANGE; i++)
  {
    buffer.x_range [i] = x[i];
    buffer.y_range [i] = y[i];
    buffer.z_range [i] = z[i];
  }
}

static posicio_t update_posicio(void)
{
  uint8_t pos_x, maxX;
  uint8_t pos_y, maxY;
  uint8_t pos_z, maxZ;



  pos_x = maxim(buffer.x_range);
  pos_y = maxim(buffer.y_range);
  pos_z = maxim(buffer.z_range);

  maxX = buffer.values_range[pos_x];
  maxY = buffer.values_range[pos_y];
  maxZ = buffer.values_range[pos_z];


  reset_buffer();

  if (maxX >= LLINDAR1 && maxY <= LLINDAR2 && maxZ <= LLINDAR2)
  {
    return Erecte;
  }
  else if (maxX <= LLINDAR2 && maxY <= LLINDAR2 && maxZ >= LLINDAR1) // tumbat de cara o esquena
  {
    return Tumbat;
  }
  else if (maxX <= LLINDAR2 && maxY >= LLINDAR1 && maxZ <= LLINDAR2) // tumbat de costat dret o esquerra
  {
    return Tumbat;
  }
  else
  {
    return Desconegut;
  }
}

static int check_fall(void)
{
  int tipus_caiguda=0;
  if (posicio_anterior == Erecte && posicio_actual == Tumbat)
  {
      tipus_caiguda=1;  
   // sendstring_ESP01("!!!!!!!! DANGER : CAIGUDA DETECTADA !!!!!!!!" );
  }
  else if (posicio_anterior == Tumbat && posicio_actual == Tumbat)
  {
      tipus_caiguda=2;
   // sendstring_ESP01( "!!!!!!!! DANGER : CAIGUDA DETECTADA !!!!!!!!" );
  }
  else if (posicio_anterior == Erecte && posicio_actual == Erecte)
  {
      tipus_caiguda=3;
   // sendstring_ESP01( "!!!!!!!! POSSIBLE CAIGUDA VERTICAL DETECTADA !!!!!!!!" );
  }
  else if (posicio_anterior == Desconegut && posicio_actual == Tumbat)
  {
      tipus_caiguda=4;
   // sendstring_ESP01( "******** ALERT : POSSIBLE CAIGUDA DETECTADA ********"   );
  }
  else if (posicio_anterior == Desconegut && posicio_actual == Desconegut)
  {
      tipus_caiguda=5;
//    sendstring_ESP01("######## CHECK : SITUACIO EXTRANYA ########");
  }
  return tipus_caiguda;
}

static int run_FSM(event_t event)
{
  static estat_t estat = s0;
  static int caiguda=0;
  switch (estat) {
    case s0:
      if (event == Time_out) {

        posicio_anterior = update_posicio();
        
      }

      else if (event == Threshold)
      {
        estat = s1;
      }
      break;

    case s1:
      if (event == Time_out)
      {
        estat = s0;
        posicio_actual = update_posicio();
        caiguda=check_fall();
        
      }
      break;
    default :
      break;
  }
  return caiguda;
}
