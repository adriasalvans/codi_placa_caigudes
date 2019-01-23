//SDA->PD1  2
//SCL->PD0  3
# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include <avr/io.h>
# include <util/delay.h>
#include <Wire.h>

#include "tmr0.h"
# include "spliter.h"
#include "FSM_Posicio.h"
#include <avr/interrupt.h>
//Mostreig
#define valortimer 155
#define G2ADXL(X) (int)(X/0.0078)
#define TIME_OUT 200
#define TIME_OUT_2 20
#define MIDA 11
// ESP_01
typedef enum{s0,s1,s2,s3,s4,s5} state_t;
int statee=1;
bool lectura=false;
int nombre_pulsacions=0;
//dades wifi and server

String uri="/api/test";
String ssid="AndroidAP5CF2";
String password="cptk0416";
String ip="192.168.43.50";
String port="8000";
String host=ip+":"+port;
state_t state=s0;
//Mostreig
static int values_range[]={0,12,25,38,51,64,76,91,102,115,128};
static int x_range[]={0,0,0,0,0,0,0,0,0,0,0};
static int y_range[]={0,0,0,0,0,0,0,0,0,0,0};
static int z_range[]={0,0,0,0,0,0,0,0,0,0,0};
static volatile bool flag_lectura=false;

//Direccion del dispositivo
const int DEVICE_ADDRESS = (0x1D);  
 
byte _buff[6];
 
//Direcciones de los registros del ADXL345
char POWER_CTL = 0x2D;
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;   //X-Axis Data 0
char DATAX1 = 0x33;   //X-Axis Data 1
char DATAY0 = 0x34;   //Y-Axis Data 0
char DATAY1 = 0x35;   //Y-Axis Data 1
char DATAZ0 = 0x36;   //Z-Axis Data 0
char DATAZ1 = 0x37;   //Z-Axis Data 1
//Placa
int led=5;
int enable=A3;
int boto=9;
int onesp=12;
int en_esp=20;
int rst_esp=13;
int gpio0=8;
int gpio2=10;
//MODUL ESP_01
static void check(void){
  uint8_t c;
  uint8_t count=0;
  bool flag=true;
  while(flag){
    
     if (Serial1.available()) {
        c= Serial1.read();
        Serial.print((char)c);    
        switch(count){
          case 0:
             if(c=='O'){
                count=1;                
             }              
          case 1: 
             if (c=='K'){
                flag = false;
             }
             count =0;
        }
     }
  }
}

void setupESP(void)
{
      
      delay(1000);
      Serial1.write("AT+CWMODE=1\r\n");//mode client
      check();
      Serial1.print("AT+CWJAP=\""+ssid+"\",\""+password+"\"\r\n");//connectar a una wifi.
      check();
      //Serial1.print("AT+CIPSTART=\"TCP\",\""+ip+"\","+port+"\r\n");//ESTABLIR UNA CONEXIO TCP AMB SERVER.
      //check();
}

void send_string(String missatge){
  char command[100]="AT+CIPSEND="; 
  char stringLen[20];
  int len;
  
  Serial.println(missatge);
  String proba="POST "+ uri +" HTTP/1.0\r\nHost:"+ host +"\r\nAccept: */*\r\nContent-Length: " + (String)missatge.length() +"\r\n"+"Content-Type: application/x-www-form-urlencoded\r\n\r\n"+missatge; 
  Serial.println(proba);
  len = (int) proba.length();
  Serial.println(len);
  sprintf(stringLen,"%i",len);
  strcat(command,stringLen);
  strcat(command,"\r\n");
  Serial1.print("AT+CIPSTART=\"TCP\",\""+ip+"\","+port+"\r\n");//ESTABLIR UNA CONEXIO TCP AMB SERVER.
  check();
  Serial1.print(command);
  check();
  Serial1.print(proba);
  check();
  Serial1.print("AT+CIPCLOSE\r\n");
}

string_t split2(char linia[]){
  string_t wifi;
  int pos=0;
  int semaph=0;
  int pos_nom=0;
  int pos_pot=0;
  while(linia[pos]!='\n'){
    switch(semaph){
      case 0:
        if(linia[pos]==','){
          semaph=1;
        }
        break;
      case 1:
        
        if(linia[pos]!=','){
          
          wifi.nom[pos_nom]=linia[pos];
          pos_nom++;
        }
        else{
          semaph=2;
        }
        break;
      case 2:
        
        if(linia[pos]!=','){
          wifi.potencia[pos_pot]=linia[pos];
          pos_pot++;
        }
        else{
          semaph=3;
        }
        delay(1);
        break;
      case 3:
        break;
    }
    pos++;
    
  }
  return wifi;
}

bool check_ok(char caracter, bool flag_final)
{
  flag_final=false;
    if (state==s0&&caracter=='\n'){
      state=s1;
    }
    else if(state==s1)
    {
      if(caracter=='\r')
      {
        state=s2;
      }
      else
      {
        state=s0;
      }
    }
    else if(state==s2)
    {
      if(caracter=='\n')
      {
        state=s3;
      }
      else
      {
        state=s0;
      }
    }
    else if(state==s3)
    {
      if(caracter=='O')
      {
        state=s4;
      }
      else
      {
        state=s0;
      }
    }
    else if(state==s4)
    {
      flag_final=true;
      state=s0;
    }
  return flag_final;
}

void lectura_wifi()
{
  if (!digitalRead(boto)){
    delay(500);
    Serial.println(F("encendre led"));
    digitalWrite(led,true);
    Serial1.write("AT+CWLAP\r\n");
    lectura=false;   
  }
  else{
    digitalWrite(led,false);
  }
  
}

void buida_cadena(char s[])
  {
    for (int i=0; s[i]!='\0';i++)
    {
      s[i]='\0';
    }
    return;
  }
  
void lectura_wifi_simple(){
  
  Serial1.write("AT+CWLAP\r\n");
  Serial.println(F("enviat"));
}



void scaneig_potencies(char recep_potencies[],char wifi1[],char wifi2[],char wifi3[],char wifi4[],char wifi5[],char wifi6[],char wifi7[],int potencies_totals[]){
    int suma_potencies=0;
    int Potencia_final=0;
    int potencia1=0;
    int potencia2=0;
    int potencia3=0;
    int potencia4=0;
    int potencia5=0;
    int potencia6=0;
    int potencia7=0;
    int contador1=0;
    int contador2=0;
    int contador3=0;
    int contador4=0;
    int contador5=0;
    int contador6=0;
    int contador7=0;
    int vegades=0;
    bool flag_final=false;
    int temps=50;
    
    while(vegades<10){
      vegades++;
      Serial.print(F("veg: "));
      Serial.println(vegades);
      lectura_wifi_simple();
      int posicio=0;
    
       while(!flag_final){
       
        if(Serial1.available()){
    
          char caracter_actual=Serial1.read();
          flag_final=check_ok(caracter_actual,flag_final);
           if (caracter_actual>=40 && caracter_actual<=122){
            if(caracter_actual=='+'){
              string_t data = split2(recep_potencies);
              buida_cadena(recep_potencies);
              delay(1);
              posicio=0;
              if(data.nom[0]==wifi1[0] && data.nom[1]==wifi1[1] && data.nom[2]==wifi1[2]){
                int Potencia_total= ((int)data.potencia[1]-48)*10+((int)data.potencia[2]-48);
                if(data.nom[0]==wifi1[0] && data.nom[1]==wifi1[1] && data.nom[2]==wifi1[2] && data.nom[3]==wifi1[3] && data.nom[4]==wifi1[4]&& data.nom[5]==wifi1[5]){
                    Serial.println(Potencia_total);
                    Serial.println(potencia1);
                    potencia1=potencia1+Potencia_total;
                    Serial.print("pot1: ");
                    Serial.println(potencia1);
                    contador1++;
                    
                }
                else if(data.nom[0]==wifi2[0] && data.nom[1]==wifi2[1] && data.nom[2]==wifi2[2] && data.nom[3]==wifi2[3] && data.nom[4]==wifi2[4]&& data.nom[5]==wifi2[5]){
                    Serial.println(Potencia_total);
                    Serial.println(potencia2);
                    potencia2=potencia2+Potencia_total;
                    Serial.print("pot2: ");
                    Serial.println(potencia2);
                    contador2++;
                }
                else if(data.nom[0]==wifi3[0] && data.nom[1]==wifi3[1] && data.nom[2]==wifi3[2] && data.nom[3]==wifi3[3] && data.nom[4]==wifi3[4]&& data.nom[5]==wifi3[5]){
                    Serial.println(Potencia_total);
                    Serial.println(potencia3);
                    //delay(temps);
                    potencia3=potencia3+Potencia_total;
                    Serial.print("pot3: ");
                    Serial.println(potencia3);
                    contador3++;
                }
                else if(data.nom[0]==wifi4[0] && data.nom[1]==wifi4[1] && data.nom[2]==wifi4[2] && data.nom[3]==wifi4[3] && data.nom[4]==wifi4[4]&& data.nom[5]==wifi4[5]){
                    Serial.println(Potencia_total);
                    Serial.println(potencia4);
                    potencia4=potencia4+Potencia_total;
                    Serial.print("pot4: ");
                    Serial.println(potencia4);
                    contador4++;
                }
                else if(data.nom[0]==wifi5[0] && data.nom[1]==wifi5[1] && data.nom[2]==wifi5[2] && data.nom[3]==wifi5[3] && data.nom[4]==wifi5[4]&& data.nom[5]==wifi5[5]){
                    Serial.println(Potencia_total);
                    Serial.println(potencia5);
                    potencia5=potencia5+Potencia_total;
                    Serial.print("pot5: ");
                    Serial.println(potencia5); 
                    contador5++;
                }
                else if(data.nom[0]==wifi6[0] && data.nom[1]==wifi6[1] && data.nom[2]==wifi6[2] && data.nom[3]==wifi6[3] && data.nom[4]==wifi6[4]&& data.nom[5]==wifi6[5]){
                    Serial.println(Potencia_total);
                    Serial.println(potencia6);
                    potencia6=potencia6+Potencia_total;
                    Serial.print("pot6: ");
                    Serial.println(potencia6);
                    contador6++;
                }
                else if(data.nom[0]==wifi7[0] && data.nom[1]==wifi7[1] && data.nom[2]==wifi7[2] && data.nom[3]==wifi7[3] && data.nom[4]==wifi7[4]&& data.nom[5]==wifi7[5]){
                    Serial.println(Potencia_total);
                    Serial.println(potencia7);
                    potencia7=potencia7+Potencia_total;
                    Serial.print("pot7: ");
                    Serial.println(potencia7);
                    contador7++;
                }
              }
            }
        else{
        posicio++;
        recep_potencies[posicio]=caracter_actual;
        }
      }  
    } 
  }
 
  flag_final=false;
}
potencia1=potencia1/contador1;
potencia2=potencia2/contador2;
potencia3=potencia3/contador3;
potencia4=potencia4/contador4;
potencia5=potencia5/contador5;
potencia6=potencia6/contador6;
potencia7=potencia7/contador7;
if (potencia1==-1){
  potencia1=95;
}
if(potencia2==-1){
  potencia2=95;
}
if(potencia3==-1){
  potencia3=95;
}
if(potencia4==-1){
  potencia4=95;
}
if(potencia5==-1){
  potencia5=95;
}
if(potencia6==-1){
  potencia6=95;
}
if(potencia7==-1){
  potencia7=95;
}

potencies_totals[0]=potencia1;
potencies_totals[1]=potencia2;
potencies_totals[2]=potencia3;
potencies_totals[3]=potencia4;
potencies_totals[4]=potencia5;
potencies_totals[5]=potencia6;
potencies_totals[6]=potencia7;
}
//MODUL FSM MOSTREIG
bool first_threshold(const int x, const int y, const int z){

    if ((x>=91) && (y>=91)){
        return true;
    }
    else if((x>=91) && (z>=91)){
        return true;
    }
    else if((y>=91) && (z>=91)){
        return true;
    }
    else{
        return false;
    }
}

static uint8_t maxim (const int *llista){
      uint8_t maxValue=0;
      uint8_t posicio;

      for (uint8_t i=0; i<MIDA;i++){
        if(llista[i]>= maxValue){
            maxValue=llista[i];
            posicio=i;
        }
      }
      return posicio;      
}
void tipus_caiguda(int caiguda,char *recep_potencies,char *wifi1,char *wifi2,char *wifi3,char *wifi4,char *wifi5, char *wifi6, char *wifi7,int *potencies_totals){
  switch(caiguda){
    case 0:
      Serial.println(F("retorn 0"));
      break;
    case 1:
      Serial.println(F("!!!!!!!! DANGER : CAIGUDA DETECTADA !!!!!!!!"));
      ALARMA(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
      digitalWrite(led,true);
      break;
    case 2:
      Serial.println(F("!!!!!!!! DANGER : CAIGUDA DETECTADA !!!!!!!!"));
      ALARMA(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
      digitalWrite(led,true);
      break;
    case 3:
      Serial.println(F("!!!!!!!! POSSIBLE CAIGUDA VERTICAL DETECTADA !!!!!!!!"));
      ALARMA(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
      digitalWrite(led,true);
      break;
    case 4:
      Serial.println(F("******** ALERT : POSSIBLE CAIGUDA DETECTADA ********"));
      ALARMA(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
      digitalWrite(led,true);
      break;
    case 5:
      Serial.println(F("######## CHECK : SITUACIO EXTRANYA ########"));
      ALARMA(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
      digitalWrite(led,true);
      break;
  }
}
void check_zone_threshold(char *recep_potencies,char *wifi1,char *wifi2,char *wifi3,char *wifi4,char *wifi5, char *wifi6, char *wifi7,int *potencies_totals){

    uint8_t pos_x;
    uint8_t pos_y;
    uint8_t pos_z;
    int caiguda=0;
    pos_x = maxim(x_range);
    pos_y = maxim(y_range);
    pos_z = maxim(z_range);
    Serial.print(F("x: "));
    Serial.print(values_range[pos_x]);
    Serial.print(F("y: "));
    Serial.print(values_range[pos_y]);
    Serial.print(F("z: "));
    Serial.print(values_range[pos_z]);
    if((values_range[pos_x]>=91 && values_range[pos_y]>=91)){
        Serial.println(F("x i y superat"));
        caiguda=send_event(Threshold);
        Serial.print(F("caiguda: "));
        Serial.println(caiguda);
    }
    else if (values_range[pos_x]>=91 && values_range[pos_z]>=91){
        Serial.println(F("x i z superat"));
        caiguda=send_event(Threshold);
        Serial.print(F("caiguda: "));
        Serial.println(caiguda);
    }
    else if (values_range[pos_y]>=91 && values_range[pos_z]>=91){
        Serial.println(F("y i z superat"));
        caiguda=send_event(Threshold);
        Serial.print(F("caiguda: "));
        Serial.println(caiguda);
    }
    
    tipus_caiguda(caiguda,recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
}

void classificar_rangX(const int x){

    if (x>=128){
        x_range[10]=x_range[10]+1;
    }
    else if (x>=115){
        x_range[9]=x_range[9]+1;
    }
    else if (x>=102){
        x_range[8]=x_range[8]+1;
    }
    else if (x>=91){
        x_range[7]=x_range[7]+1;
    }
    else if (x>=76){
        x_range[6]=x_range[6]+1;
    }
    else if (x>=64){
        x_range[5]=x_range[5]+1;
    }
    else if (x>=51){
        x_range[4]=x_range[4]+1;
    }
    else if (x>=38){
        x_range[3]=x_range[3]+1;
    }
    else if (x>=25){
        x_range[2]=x_range[2]+1;
    }
    else if (x>=12){
        x_range[1]=x_range[1]+1;
    }
   else if (x>=0){
        x_range[0]=x_range[0]+1;
    }
}
void classificar_rangY(const int y){

    if (y>=128){
        y_range[10]=y_range[10]+1;
    }
    else if (y>=115){
        y_range[9]=y_range[9]+1;
    }
    else if (y>=102){
        y_range[8]=y_range[8]+1;
    }
    else if (y>=91){
        y_range[7]=y_range[7]+1;
    }
    else if (y>=76){
        y_range[6]=y_range[6]+1;
    }
    else if (y>=64){
        y_range[5]=y_range[5]+1;
    }
    else if (y>=51){
        y_range[4]=y_range[4]+1;
    }
    else if (y>=38){
        y_range[3]=y_range[3]+1;
    }
    else if (y>=25){
        y_range[2]=y_range[2]+1;
    }
    else if (y>=12){
        y_range[1]=y_range[1]+1;
    }
   else if (y>=0){
        y_range[0]=y_range[0]+1;
    }
}

void classificar_rangZ(const int z){

    if (z>=128){
        z_range[10]=z_range[10]+1;
    }
    else if (z>=115){
        z_range[9]=z_range[9]+1;
    }
    else if (z>=102){
        z_range[8]=z_range[8]+1;
    }
    else if (z>=91){
        z_range[7]=z_range[7]+1;
    }
    else if (z>=76){
        z_range[6]=z_range[6]+1;
    }
    else if (z>=64){
        z_range[5]=z_range[5]+1;
    }
    else if (z>=51){
        z_range[4]=z_range[4]+1;
    }
    else if (z>=38){
        z_range[3]=z_range[3]+1;
    }
    else if (z>=25){
        z_range[2]=z_range[2]+1;
    }
    else if (z>=12){
        z_range[1]=z_range[1]+1;
    }
   else if (z>=0){
        z_range[0]=z_range[0]+1;
    }
}

void reset_buffer_2(){
    for(int i=0;i<MIDA;i++)
    {
      x_range[i]=0;
      y_range[i]=0;
      z_range[i]=0;
    }
    

}
void ALARMA(char *recep_potencies,char *wifi1,char *wifi2,char *wifi3,char *wifi4,char *wifi5, char *wifi6, char *wifi7,int *potencies_totals){
  
  Serial.println(F("aqui"));   
  scaneig_potencies(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
  Serial.println(F("Potencies per ordre: "));  
  Serial.println(potencies_totals[0]);
  Serial.println(potencies_totals[1]);
  Serial.println(potencies_totals[2]);
  Serial.println(potencies_totals[3]);
  Serial.println(potencies_totals[4]);
  Serial.println(potencies_totals[5]);  
  Serial.println(potencies_totals[6]);
  String AP="";
  
  
  for (int i=0;i<7;i++)
    {
      if (i==6)
      {
        String c=("AP_"+(String)(i+1)+","+(String)potencies_totals[i]);  
        AP=AP+c;
        //Serial.print(i);
        //Serial.print(c);
        
        break;
      }
      else
      {
        String c=("AP_"+(String)(i+1)+","+(String)potencies_totals[i]+";");
        AP=AP+c;
        
        //Serial.print(i);
        //Serial.print(c);
      }
    }
   
  //Serial.println(AP);
  
  delay(200);
  
  delay(200);
  //Serial.println(missatge_complet);
  
  send_string("ssids_rssis="+AP+"&idPlaca=7");
  Serial.println(F(" final1"));
  delay(100);
  
}
void run_FSM(){
    static int state=1;
    static int x,y,z;
    static uint8_t finestra=0;
    char recep_potencies[80]{};
    char wifi1[]="POS001";
    char wifi2[]="POS002";
    char wifi3[]="POS003";
    char wifi4[]="POS004";
    char wifi5[]="POS005";
    char wifi6[]="POS006";
    char wifi7[]="POS007";
    int potencies_totals[7]{};
    
    while(true){
        
        lectura=true;
        lectura_wifi();
        if(lectura==false){
          ALARMA(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
        }
        while(!read_adxl(&x,&y,&z)); // espera nova dades segons la frequencia de mostreig
        //Serial.println("llegit");
        //Serial.print("x: ");
        //Serial.print( x );
        //Serial.print(" y: ");
        //Serial.print( y );
        //Serial.print(" z: ");
        //Serial.println( z );
        classificar_rangX(abs(x));   // modificar llista x_range, es a dir, modificar la llista de contadors
        classificar_rangY(abs(y));
        classificar_rangZ(abs(z));
        
        if (state==1){
           
            if (finestra==TIME_OUT){             
                setBuffers(x_range,y_range,z_range); // FSM POSICIO
                reset_buffer_2();
                finestra=0;
                Serial.println(F("time_out"));
                send_event(Time_out); // FSM POSICIO
            }
            else if (first_threshold(abs(x),abs(y),abs(z))){
                state=2;
                reset_buffer_2();
                finestra=0;
            }
            
        }
        else{
            if (finestra==TIME_OUT_2){             
                state=1;
                Serial.println(F("check_zone"));
                check_zone_threshold(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
                reset_buffer_2();
                finestra=0;
            }
        }
        finestra=finestra+1;
    }
}
//Funcion auxiliar de escritura
void writeTo(int device, byte address, byte val) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission(); 
}
 
//Funcion auxiliar de lectura
void readFrom(int device, byte address, int num, byte _buff[]) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.endTransmission();
 
  Wire.beginTransmission(device);
  Wire.requestFrom(device, num);
 
  int i = 0;
  while(Wire.available())
  { 
    _buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
}
bool read_adxl(int *x, int *y, int *z){

    if (flag_lectura==true){
        flag_lectura=false;
        readAccel(x,y,z);         // llegir valors de laccelarometre x,y,z
        return true;
    }
    return false;
}
void readAccel(int *x, int *y, int *z) {
  //Leer los datos
  uint8_t numBytesToRead = 6;
  readFrom(DEVICE_ADDRESS, DATAX0, numBytesToRead, _buff);
 
  //Leer los valores del registro y convertir a int (Cada eje tiene 10 bits, en 2 Bytes LSB)
  *x = (((int)_buff[1]) << 8) | _buff[0];   
  *y = (((int)_buff[3]) << 8) | _buff[2];
  *z = (((int)_buff[5]) << 8) | _buff[4];
  //Serial.print("x: ");
  //Serial.print( x );
  //Serial.print(" y: ");
  //Serial.print( y );
  //Serial.print(" z: ");
  //Serial.println( z );
}
void setup() {
  // put your setup code here, to run once:
  pinMode(boto,INPUT);
  pinMode(led,OUTPUT);
  pinMode(enable,OUTPUT);
  pinMode(onesp,OUTPUT);
  pinMode(en_esp,OUTPUT);
  pinMode(rst_esp,OUTPUT);
  pinMode(gpio0,INPUT);
  pinMode(gpio2,INPUT);
  digitalWrite(led,false);
  //senyals necessaries per la activació de la placa, alimentacio de placa i de esp
  digitalWrite(enable,true);
  digitalWrite(onesp,true);
  digitalWrite(en_esp,true);
  digitalWrite(rst_esp,true);
  //baudrates config
  Serial.begin(9600);
  delay(500);
  Serial1.begin(9600);
  delay(500);
  digitalWrite(led,true);
  setupESP();
  
  Wire.begin();
  writeTo(DEVICE_ADDRESS, DATA_FORMAT, 0x01); //Poner ADXL345 en +- 4G
  writeTo(DEVICE_ADDRESS, POWER_CTL, 0x08);  //Poner el ADXL345 
  setup_tmr0(valortimer,1024);
  posicio_init();
  sei();
  digitalWrite(led,false);
}

void loop() {
  // put your main code here, to run repeatedly:
  run_FSM();
   
  
 
}

ISR(TIMER0_COMPA_vect){   // frequencia de 100 Hz
  
  flag_lectura=true;   

}
