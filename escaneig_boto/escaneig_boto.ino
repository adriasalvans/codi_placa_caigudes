# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include <avr/io.h>
# include <util/delay.h>
# include "spliter.h"

typedef enum{s0,s1,s2,s3,s4,s5} state_t;


//dades wifi and server

String uri="/api/test";
String ssid="AndroidAPDAC0";
String password="qkin5824";
String ip="192.168.43.159";
String port="8000";
String host=ip+":"+port;
state_t state=s0;

int statee=1;
int led=5;
int enable=A3;
int boto=9;
int onesp=12;
int en_esp=20;
int rst_esp=13;
int gpio0=8;
int gpio2=10;
bool lectura=false;
int nombre_pulsacions=0;

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
  char stringLen[10];
  int len;

  String proba="POST "+ uri +" HTTP/1.0\r\nHost:"+ host +"\r\nAccept: */*\r\nContent-Length: " + (String)missatge.length() +"\r\n"+"Content-Type: application/x-www-form-urlencoded\r\n\r\n"+missatge; 
  
  len = (int) proba.length();
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
    Serial.println("encendre led");
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
  Serial.println("enviat");
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
      Serial.print("veg: ");
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


void setup() {
  pinMode(boto,INPUT);
  pinMode(led,OUTPUT);
  pinMode(enable,OUTPUT);
  pinMode(onesp,OUTPUT);
  pinMode(en_esp,OUTPUT);
  pinMode(rst_esp,OUTPUT);
  pinMode(gpio0,INPUT);
  pinMode(gpio2,INPUT);
  digitalWrite(led,false);
  digitalWrite(enable,true);
  digitalWrite(onesp,true);
  digitalWrite(en_esp,true);
  digitalWrite(rst_esp,true);
  Serial.begin(9600);
  delay(500);
  Serial1.begin(9600);
  delay(500);
  digitalWrite(led,true);
  setupESP();
}

void loop() 
{
  //delay(2000);
  Serial.println("inici");
  char recep_potencies[80]{};
  static bool primera_vegada=true;
  
  char wifi1[]="POS001";
  char wifi2[]="POS002";
  char wifi3[]="POS003";
  char wifi4[]="POS004";
  char wifi5[]="POS005";
  char wifi6[]="POS006";
  char wifi7[]="POS007";
  int potencies_totals[7]{};
  
  lectura=true;
  
  Serial.println("Preparat");
  //enviament de codi per llegir potencia wifis
  delay(10);
  while(lectura){//blocant fins que no es prem el boto  
    lectura_wifi();     
  }
  
  Serial.println("aqui");   
  scaneig_potencies(recep_potencies,wifi1,wifi2,wifi3,wifi4,wifi5,wifi6,wifi7,potencies_totals);
  Serial.println("Potencies per ordre: ");  
  Serial.println(potencies_totals[0]);
  Serial.println(potencies_totals[1]);
  Serial.println(potencies_totals[2]);
  Serial.println(potencies_totals[3]);
  Serial.println(potencies_totals[4]);
  Serial.println(potencies_totals[5]);  
  Serial.println(potencies_totals[6]);
  String message="ssids_rssis="; 
  for (int i=0;i<7;i++)
    {
      if (i==6)
      {
        String c=("AP_"+(String)(i+1)+","+(String)potencies_totals[i]);
        message=message+c;
      }
      else
      {
        String c=("AP_"+(String)(i+1)+","+(String)potencies_totals[i]+";");
        message=message+c;
      }
    }
  
  send_string(message+"&idPlaca=7&tipus_caiguda=Abelitodimetu");
  Serial.println(" final1");
  delay(100);
  Serial.println("ultim print");
  delay(100);
}



 
 
