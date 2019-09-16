/* LCD pins
 Board I2C / TWI pins
Uno       =   A4 (SDA), A5 (SCL)
*/
#include <IRremote.h>

//Begin: Variables Globales
int alarm[2][1];
int receptor = 3; //pin de recepción del ardino para el control infrarrojo
//Botones del control remoto:
unsigned long b0= 0xFF9867;
unsigned long b1= 0xFFA25D;
unsigned long b2 = 0xFF629D;
unsigned long b3 = 0xFFE21D;
unsigned long b4= 0xFF22DD;
unsigned long b5 = 0xFF02FD;
unsigned long b6 = 0xFFC23D;
unsigned long b7 = 0xFFE01F;
unsigned long b8 = 0xFFA857;
unsigned long b9 = 0xFF906F;
unsigned long bGato = 0xFF6897;
unsigned long bAsterisco = 0xFFB04F;
unsigned long Bok = 0xFF38C7;
unsigned long bArriba = 0xFF18E7;
unsigned long bAbajo = 0xFF4AB5;
unsigned long bDerecha = 0xFF5AA5;
unsigned long bIzquierda = 0xFF10EF;
//End: Variables Globales

//begin: Incialización de puertos;
IRrecv irrecv(receptor); //indico a arduino bajo que pin le llegara info
decode_results codigo; //almacena el codigo hex recibido del control
//end: Inicialización de Puertos

//Funciones para el manejo del control remoto
//Funciones para el manejo del control remoto

//Begin: Funciones creadas para el manejo del tiempo
void TimeSetup(){
  /*lcd.print("entre");
  delay(1000);
  lcd.setCursor(0,0);*/
  Serial.println("Cambiar hora y");
  int opc=0;
  while(true){
    if(irrecv.decode(&codigo)){
    Serial.println(codigo.value,HEX);
    if(codigo.value==b1){
      opc=1;
      break;
      }
    delay(50);
    irrecv.resume();
    }
    
  }//******************
  
  //****************
  delay(1000);
  if(opc==1){
    Serial.print("Modificare");
    }
  else{
    Serial.print("Mejor na");
    }
  }
//End: Funciones creadas para el manejo del tiempo

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);// ss, mm, hh,#D, d, m, y
    /*myRTC.setDS1302Time(00, 45, 15, 7, 9, 2, 2019);
    alarm[0][0]={15};
    alarm[0][1]={46};*/
  //Ajustes del Control Remoto
  irrecv.enableIRIn(); //habilito el sensor
  //Ajustes de la pantalla led:
  /*lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);*/
  TimeSetup();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Ya llege hasta aquí: Main");
  /*
  myRTC.updateTime();
  int TIME[6]={myRTC.dayofmonth,myRTC.month,myRTC.year,myRTC.hours,myRTC.minutes,myRTC.seconds};
  // Se imprime el resultado en el Monitor Serial
  lcd.setCursor(0,0);
  lcd.print("Tiempo: "+(String)TIME[3]+":"+TIME[4]+":"+TIME[5]);
  if(alarm[0][0] == TIME[3] && alarm[0][1]== TIME[4] && (int)TIME[5]==0){
   lcd.setCursor(0,1);
   lcd.print("Es hora de la comida");
   }
  // Un pequeño delay para no repetir y leer más facil
  delay(1000);
  */
}
