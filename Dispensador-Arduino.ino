/* LCD pins
 Board I2C / TWI pins
Uno       =   A4 (SDA), A5 (SCL)
*/
#include <virtuabotixRTC.h>
#include <IRremote.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x27,16,2)  ; // si no te sale con esta direccion  puedes usar (0x3f,16,2) || (0x27,16,2)  ||(0x20,16,2) 
                   //clock,dat,rst
virtuabotixRTC myRTC(5, 6, 4); 
//Begin: Variables Globales
Servo m1;
int PINSERVO=2;
int PULSOMIN=450; // Pulso de 0 grados
int PULSOMAX=2350; //Pulso de 180 grados
int TIME[3];
int alarm[3][2]={{0,0},{0,0},{0,0}};
int receptor = 3; //pin de recepción del ardino para el control infrarrojo
int aux=0; //Variable auxiliar para el control de ciertas opciones
int led=A3;
int grams=0; //Variable para el control de alimento en gramos
//Botones del control remoto:
const unsigned long b0= 0xFF9867;
const unsigned long b1= 0xFFA25D;
const unsigned long b2 = 0xFF629D;
const unsigned long b3 = 0xFFE21D;
const unsigned long b4= 0xFF22DD;
const unsigned long b5 = 0xFF02FD;
const unsigned long b6 = 0xFFC23D;
const unsigned long b7 = 0xFFE01F;
const unsigned long b8 = 0xFFA857;
const unsigned long b9 = 0xFF906F;
const unsigned long bAsterisco = 0xFF6897;
const unsigned long bGato = 0xFFB04F;
const unsigned long Bok = 0xFF38C7;
const unsigned long bArriba = 0xFF18E7;
const unsigned long bAbajo = 0xFF4AB5;
const unsigned long bDerecha = 0xFF5AA5;
const unsigned long bIzquierda = 0xFF10EF;
//End: Variables Globales

//begin: Incialización de puertos;
IRrecv irrecv(receptor); //indico a arduino bajo que pin le llegara info
decode_results codigo; //almacena el codigo hex recibido del control
//end: Inicialización de Puertos

//Funciones para el manejo del control remoto
//Funciones para el manejo del control remoto

//Begin: Funciones creadas para el manejo del tiempo
void TimeSetup(){
  lcd.print("entre");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Cambiar hora y");
  lcd.setCursor(0,1);
  lcd.print("fecha? 1.Si/2.No");
  //***************
   while(true){
    if(irrecv.decode(&codigo)){
    Serial.println(codigo.value,HEX);
    if(codigo.value==b1){
      setTime();
      break;
      }
    if(codigo.value==b2){break;}
    delay(50);
    irrecv.resume();
    }
   }
  //**************
  lcd.clear();
  lcd.setCursor(0,0);
  delay(1000);
  }

void setTime(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hora: 00:00");
  lcd.setCursor(0,1);
  lcd.print("Ok para Guardar");
  lcd.setCursor(6,0);
  lcd.blink();
  delay(1000);
  GetTime();       // ss,   mm,     hh,    #D, d, m, y
  myRTC.setDS1302Time(00, TIME[1], TIME[0], 0, 1, 1, 2019);
  lcd.clear();
}

void GetTime(){
  aux=6;
  int num;
  int Dh=0, Uh=0, Dm=0, Um=0;
  irrecv.resume();
  //***************
   while(true){
    if(irrecv.decode(&codigo)){
    Serial.print(codigo.value,HEX);
    if(codigo.value==Bok){
      TIME[0]=Dh+Uh;
      TIME[1]=Dm+Um;
      lcd.noBlink();
      break;
    }
    if(codigo.value==b1 || codigo.value==b2 || codigo.value==b3 ||codigo.value==b4 || codigo.value==b5 || codigo.value==b6 ||codigo.value==b7 || codigo.value==b8 || codigo.value==b9 ||codigo.value==b0){
       num=toNum(codigo.value);
       lcd.print(String(toNum(num)));
       if((aux==6 && num>2) || (aux==9 && num>5)){
        lcd.setCursor(aux,0);
        lcd.print("0"); 
       }else{
               if(aux==6){
                  Dh=toNum(codigo.value)*10;
               }
               if(aux==7){
                Uh = toNum(codigo.value);
                }
                if(aux==9){
                  Dm=toNum(codigo.value)*10;
               }
               if(aux==10){
                  Um = toNum(codigo.value);
                }
       }
       lcd.setCursor(aux,0);
    }
    if(codigo.value==bDerecha){
       aux++; 
       if(aux==8)
          aux=9;
       lcd.setCursor(aux,0);
    }
    if(codigo.value==bIzquierda){
       aux--; 
       if(aux==8)
          aux=7;
       lcd.setCursor(aux,0);
    }
       if(aux==11){
          aux=10;
          lcd.setCursor(aux,0); }
       if(aux==5){
          aux=6; 
          lcd.setCursor(aux,0);
       }
    
    delay(50);
    irrecv.resume();
    }
   }
  //**************
  }

int toNum(unsigned long code){
  int c;
  switch(code){
    case b1:
      c=1;
      break;
    case b2:
      c=2;
      break;
    case b3:
      c=3;
      break;
    case b4:
      c=4;
      break;
    case b5:
      c=5;
      break;
    case b6:
      c=6;
      break;
    case b7:
      c=7;
      break;
    case b8:
      c=8;
      break;
    case b9:
      c=9;
      break;
    case b0:
      c=0;
      break;
    }
    return c;
  }

void setAlarms(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Cambiar Alarma: ");
  lcd.setCursor(0,1);
  lcd.print("  1     2    3 ");
  irrecv.resume();
  //***************
   while(true){
    if(irrecv.decode(&codigo)){
    Serial.println(codigo.value,HEX);
    if(codigo.value==b1 || codigo.value==b2 || codigo.value==b3){
      AlarmSetup(toNum(codigo.value));
      break;
    }
    delay(50);
    irrecv.resume();
    }
   }
   lcd.clear();
  //**************
  
  }
 void AlarmSetup(int Alarm){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alarma a las:");
  lcd.setCursor(0,1);
  showTimeFormat(alarm[Alarm-1][0], alarm[Alarm-1][1]);
  lcd.print(" 1.Mod 2.Ok");
  lcd.setCursor(0,1);
  irrecv.resume();
  //***************
   while(true){
    if(irrecv.decode(&codigo)){
    Serial.println(codigo.value,HEX);
    if(codigo.value==b1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Hora: 00:00");
      lcd.setCursor(0,1);
      lcd.print("Ok para Guardar");
      lcd.setCursor(6,0);
      lcd.blink();
      delay(1000);
      GetTime();
      alarm[Alarm-1][0]=TIME[0];//Hora
      alarm[Alarm-1][1]=TIME[1];//minuto
      lcd.clear();
      break;
    }
    if(codigo.value==b2){break;}
    delay(50);
    irrecv.resume();
    }
   }
  //**************
 }

void showTimeFormat(int hora, int minutos){
  if(hora<10){
    lcd.print("0"+(String)hora);
   }else{
    lcd.print((String)hora);
   }
  lcd.print(":");
  if(minutos<10){
    lcd.print("0"+(String)minutos);
   }else{
    lcd.print((String)minutos);
   }
}
//End: Funciones creadas para el manejo del tiempo
//Begin: funciones para el control de alimento
void  setGrams(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Comida Disp.:");
    lcd.setCursor(0,1);
    lcd.print((String)grams+"g 1.Mod 2.Ok");
    irrecv.resume();
    //***************
   while(true){
    if(irrecv.decode(&codigo)){
    Serial.println(codigo.value,HEX);
    if(codigo.value==b1){
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Gramos: 0000g");
     lcd.setCursor(0,1);
     lcd.print("Ok para Guardar");
     lcd.setCursor(8,0);
      lcd.blink();
      delay(1000);
     getGrams();
      break;
    }
    if(codigo.value==b2){break;}
    delay(50);
    irrecv.resume();
    }
   }
   lcd.clear();
  //**************
  }

 void getGrams(){
  aux=8;
  int GM=0, GC=0, GD=0, GU=0;
  irrecv.resume();
  //***************
   while(true){
    if(irrecv.decode(&codigo)){
    Serial.print(codigo.value,HEX);
    if(codigo.value==Bok){
      grams=0;
      grams=grams+GM+GC+GD+GU;
      lcd.noBlink();
      Serial.print(grams);
      break;
    }
    if(codigo.value==b1 || codigo.value==b2 || codigo.value==b3 ||codigo.value==b4 || codigo.value==b5 || codigo.value==b6 ||codigo.value==b7 || codigo.value==b8 || codigo.value==b9 ||codigo.value==b0){
       lcd.print(String(toNum(codigo.value)));

               if(aux==8){
                  GM=toNum(codigo.value)*1000;
               }
               if(aux==9){
                GC = toNum(codigo.value)*100;
                }
                if(aux==10){
                  GD=toNum(codigo.value)*10;
               }
               if(aux==11){
                  GU = toNum(codigo.value);
                }
       
       lcd.setCursor(aux,0);
    }
    if(codigo.value==bDerecha){
       aux++; 
       lcd.setCursor(aux,0);
    }
    if(codigo.value==bIzquierda){
       aux--; 
       lcd.setCursor(aux,0);
    }
       if(aux==12){
          aux=11;
          lcd.setCursor(aux,0); }
       if(aux==7){
          aux=8; 
          lcd.setCursor(aux,0);
       }
    
    delay(50);
    irrecv.resume();
    }
   }
  //**************
 }

void gramsCheck(){
  delay(grams);  
}
//End: funciones para el control de alimento
//Begin: Funciones para el manejo de servo
void alimentar(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hora de comer");
  lcd.setCursor(0,1);
  lcd.print("Alimentando");
  m1.write(50);
  gramsCheck();
  m1.write(0);
  lcd.clear();
  }
//End:  Funciones para el manejo de servo

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  m1.attach(PINSERVO,PULSOMIN,PULSOMAX);
  m1.write(0);
    alarm[0][0]={15};
    alarm[0][1]={46};
  //Ajustes del Control Remoto
  irrecv.enableIRIn(); //habilito el sensor
  //Ajustes de la pantalla led:
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  myRTC.setDS1302Time(00,45, 15, 0, 1, 1, 2019);
  TimeSetup();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  myRTC.updateTime();
  TIME[0]=myRTC.hours; TIME[1]=myRTC.minutes; TIME[2]=myRTC.seconds;
  // Begin: Se imprime la hora con formato en la pantalla lcd
  if((int)TIME[2]==0){lcd.clear();}
  lcd.setCursor(0,0);
  lcd.print("Tiempo: ");
  showTimeFormat(TIME[0],TIME[1]);
  lcd.print(":");
  if((int)TIME[2]<10){
  lcd.print("0"+(String)TIME[2]);
  }else
  {
  lcd.print((String)TIME[2]);
  }
  // End: Se imprime la hora con formato en la pantalla lcd
  if(         ((alarm[0][0] == (int)TIME[0]) && (alarm[0][1]== (int)TIME[1]) && (int)TIME[2]==0)     ||   ((alarm[1][0] == (int)TIME[0]) && (alarm[1][1]== (int)TIME[1]) && (int)TIME[2]==0)           ||        ((alarm[2][0] == (int)TIME[0]) && (alarm[2][1]== (int)TIME[1]) && (int)TIME[2]==0)                 ){
   alimentar();
   }
   if(irrecv.decode(&codigo)){
    Serial.println(codigo.value,HEX);
    if(codigo.value==bGato){
      setTime();
      }
    if(codigo.value==bAsterisco){
      /*Para las alarmas*/
      setAlarms();
      }
    if(codigo.value==b0){
      /*Para las alarmas*/
      setGrams();
      }
    delay(50);
    irrecv.resume();
    }
   
  // Un pequeño delay para no repetir y leer más facil
  delay(1000);
}
