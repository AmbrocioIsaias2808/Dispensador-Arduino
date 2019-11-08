/* LCD pins
 Board I2C / TWI pins
Uno       =   A4 (SDA), A5 (SCL)
*/
#include <virtuabotixRTC.h> //Libreria para manejar el modulo de reloj
#include <IRremote.h> //Libreria para controlar el modulo Infrarrojo
#include <EEPROM.h> //Libreria para acceder a los sectores de memoria de la EEROM
#include <Servo.h> //Libreria para manejar el servo
#include <LiquidCrystal_I2C.h> //Libreria para manejar el LcD
#include "HX711.h" //Libreria para manejar el modulo de carga
HX711 balanza; // Objeto que referencia al modulo de carga
//Objeto que referencia a la pantalla lcd
LiquidCrystal_I2C lcd (0x27,16,2)  ; // si no te sale con esta direccion  puedes usar (0x3f,16,2) || (0x27,16,2)  ||(0x20,16,2) 
                   //clock,dat,rst
virtuabotixRTC myRTC(5, 6, 4); //Instancio al objeto que referenciara a mi reloj.
//Begin: Variables Globales
Servo m1; //Intancio al objeto que referencia a mi servomotor
int PINSERVO=2;   //Pin de datos del servo
int PULSOMIN=450; // Pulso de 0 grados
int PULSOMAX=2350; //Pulso de 180 grados
int TIME[3];  //Variable donde se almacenara el tiempo en tiempo de ejecución en formato HH:MM:SS
int alarm[3][2]={{0,0},{0,0},{0,0}}; //Arreglo donde almacenar las alarmas en formatos de HH:MM
int receptor = 3; //pin de recepción del ardino para el control infrarrojo
int aux=0; //Variable auxiliar para el control de ciertas opciones
int buzzer=10; //Pin de control del buzzer
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
const int LOADCELL_DOUT_PIN = 8; //Pin de datos de la celda de carga **
const int LOADCELL_SCK_PIN = 9; //Pin de reloj de la celda de carga **
//End: Variables Globales

//begin: Incialización de puertos;
IRrecv irrecv(receptor); //indico a arduino bajo que pin le llegara info
decode_results codigo; //almacena el codigo hex recibido del control
//end: Inicialización de Puertos

//Funciones para el manejo del control remoto
//Funciones para el manejo del control remoto

//Begin: Funciones creadas para el manejo del tiempo
void TimeSetup(){ //Función para establecer el tiempo del reloj al inicio del sistema
  lcd.print("Inicializando...");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ajustar hora?");
  lcd.setCursor(0,1);
  lcd.print("    1.Si/2.No");
  //***************
   while(true){ //Mientras el bucle este ciclado
    if(irrecv.decode(&codigo)){ //Si se recibe codigo del control remoto
    Serial.println(codigo.value,HEX); //Imprimo por serial el codigo numerico recibido
    if(codigo.value==b1){ //Si el codigo coincide con el boton 1
      setTime(); //Llamo a la función para guardar la hora en el reloj
      break; //Salgo del bucle
      }
    if(codigo.value==b2){break;} //Si el codigo coincide con el boton termino el bucle
    delay(50); 
    irrecv.resume(); //Le digo al control que borre el cache y siga escuchando por nuevas entradas.
    }
   }
  //**************
  lcd.clear();
  lcd.setCursor(0,0);
  delay(1000);
  }

void setTime(){ //Funcion para guardar el tiempo actual
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hora: 00:00");
  lcd.setCursor(0,1);
  lcd.print("Ok para Guardar");
  lcd.setCursor(6,0);
  lcd.blink();
  delay(1000);
  //Entro en la función que lee las entradas para ajustar la hora
  GetTime();       // ss,   mm,     hh,    #D, d, m, y
  myRTC.setDS1302Time(00, TIME[1], TIME[0], 0, 1, 1, 2019); //Guardo la hora en el formato dado
  lcd.clear();
}

void GetTime(){
  aux=6;  //Variable auxiliar para saber en que columna se encuentra el cursor del lcd
  int num;   //Variable auxiliar para almacenar el numero tecleado
  int Dh=0, Uh=0, Dm=0, Um=0; //Variables auxiliares para guardar las D decenas y U unidades de hora o minuto
  irrecv.resume(); //Pongo a la escucha de nuevas entradas borrando el cache previo
  //***************
   while(true){    //Se repite el bucle mientras sea verdadero
    if(irrecv.decode(&codigo)){ //Si en efecto se lee un codigo
    Serial.print(codigo.value,HEX); //Imprimo el codigo en pantalla
    if(codigo.value==Bok){ //Si se presiono el boton de ok 
      TIME[0]=Dh+Uh; //Guardo en el arreglo de manejo de tiempo en la posicion de horas la suma de Decenas y Unidades de hora
      TIME[1]=Dm+Um; //Guardo en el arreglo de manejo de tiempo en la posicion de minutos la suma de Decenas y Unidades de minutos
      lcd.noBlink(); //Establesco que el cursor del lcd deje de parpadear
      break;  //Rompo el bucle
    }
    // Si se presiona algun boton numerico
    if(codigo.value==b1 || codigo.value==b2 || codigo.value==b3 ||codigo.value==b4 || codigo.value==b5 || codigo.value==b6 ||codigo.value==b7 || codigo.value==b8 || codigo.value==b9 ||codigo.value==b0){
       num=toNum(codigo.value); //Mando a convertir el codigo hexadecimal del boton en valor numerico
       lcd.print(String(toNum(num))); // muestro tal numero en pantalla en formato de String
       if((aux==6 && num>2) || (aux==9 && num>5)){ //Si el numero es mayor que 2 en la posicion 6 (decenas de hora) o mayor que 5 en la posicion 9 
        lcd.setCursor(aux,0); //En tal posicion de tal fila
        lcd.print("0"); //Imprimo un cero
       }else{ //Por el contrario
               if(aux==6){ //Si es la pocision 6
                  Dh=toNum(codigo.value)*10; //El codigo hexadecimal convertido en numero multiplicado por 10 lo guardo en Decenas de Hora
               }
               if(aux==7){ //Si es la pocision 7
                Uh = toNum(codigo.value); //El codigo hexadecimal convertido en numero lo guardo en Unidades de hora
                }
                if(aux==9){ //Si es la posicion 9
                  Dm=toNum(codigo.value)*10; //El codigo hexadecimal convertido en numero multiplicado por 10 lo guardo en Decenas de Minuto
               }
               if(aux==10){ //Si es la posicion 10 
                  Um = toNum(codigo.value); //El codigo hexadecimal convertido en numero lo guardo en Unidades de Hora
                }
       }
       lcd.setCursor(aux,0); //Coloco el cursor en la posicion tal
    }
    if(codigo.value==bDerecha){ //Si el codigo corresponde el boton de flecha derecha
       aux++;  //Desplazo el cursor hacia la derecha
       if(aux==8) //Si la posicion del cursor es == 8
          aux=9; //Saltamos a la posicion 9 debido a que la posicion 8 son los : en HH:MM
       lcd.setCursor(aux,0); //Y coloco el cursor donde debe ir
    }
    if(codigo.value==bIzquierda){ //Si el codigo corresponde el boton de flecha derecha
       aux--;  //Desplazo el cursor hacia la izquierda
       if(aux==8)//Si la posicion del cursor es == 8
          aux=7; //Saltamos a la posicion 7 debido a que la posicion 8 son los : en HH:MM
       lcd.setCursor(aux,0); //Y coloco el cursor donde debe ir
    }
       if(aux==11){ //Si la variable auxiliar para posicionar el cursor avanza hasta la posicion 11
          aux=10; //Le digo que se quede en la posicion 10 
          lcd.setCursor(aux,0); } //Y lo posiciono
     
       if(aux==5){ //Si la variable auxiliar para posicionar el cursor avanza hasta la posicion 5
          aux=6; //Le digo que se quede en la posicion 6
          lcd.setCursor(aux,0);//Y lo posiciono
       }
    
    delay(50); //Espero 50 milisegundos
    irrecv.resume(); //Resumo la escucha de nuevos codigos por parte del sensor IR
    }
   }
  //**************
  }

//Funcion para convertir los codigos hexadecimales recibidos por el sensor IR a valor numerico entero
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
  
//Funcion para programar alarmas
void setAlarms(){
  lcd.clear(); //Borro el contendio de la lcd
  lcd.setCursor(0,0); 
  lcd.print("Cambiar Alarma: "); 
  lcd.setCursor(0,1);
  lcd.print("  1     2    3 ");
  irrecv.resume();
  //***************
   while(true){ //Mientras el while se repita
    if(irrecv.decode(&codigo)){ //Me mantengo a la escucha de señales por parte del control infrarrojo
    Serial.println(codigo.value,HEX); //Si lo recibe lo imprimo
    if(codigo.value==b1 || codigo.value==b2 || codigo.value==b3){ //Y si corresponde a los codigos de los botones 1, 2 o 3
      AlarmSetup(toNum(codigo.value)); //LLamo a la función para modificar la hora de la alarma pasando como parametro el numero de la alarma
      break; //Rompo el ciclo while
    }
    delay(50); //Espero un tiempo
    irrecv.resume(); //Le digo al sensor IR que borre el cache y se ponga a la escucha de nuevo
    }
   }
   lcd.clear(); //Limpio la pantalla
  //**************
  
  }
 void AlarmSetup(int Alarm){  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alarma a las:");
  lcd.setCursor(0,1);
  showTimeFormat(alarm[Alarm-1][0], alarm[Alarm-1][1]); //Muestro en pantalla la ultima hora programada en la alarma correspondiente
  lcd.print(" 1.Mod 2.Ok"); //Le pregunto si desea modificarla o dejarla como esta
  lcd.setCursor(0,1);
  irrecv.resume();  //Antes de continuar le digo al sensor que borre el cache y escuche por la respuesta del usuario
  //***************
   while(true){ //Mientras el while se repita
    if(irrecv.decode(&codigo)){ //Verfico si recibo codigos
    Serial.println(codigo.value,HEX); //Si, si imprimo
    if(codigo.value==b1){ //Si el codigo es del boton b1
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Hora: 00:00");
      lcd.setCursor(0,1);
      lcd.print("Ok para Guardar");
      lcd.setCursor(6,0);
      lcd.blink(); //Hago las intrucciones anteriores y le indico al lcd que debe parpadear el cursor
      delay(1000);
      GetTime(); //LLamo a la funcion GetTime para guardar la hora introducida por el usuario
      //GetTime guarda la hora en la variable TIME[0] y los minutos en TIME[1];
      alarm[Alarm-1][0]=TIME[0];//Hora:  Por lo tanto en la posicion correspondiente del arreglo alarm guardo el valor de TIME[0]
      alarm[Alarm-1][1]=TIME[1];//minuto: Por lo tanto en la posicion correspondiente del arreglo alarm guardo el valor de TIME[1]
      //Ahora de acuerdo al numero de la alarma
      if(Alarm==1){ //EEPROM: memoria no volatil del Arduino
         EEPROM.put(1,TIME[0]); //Guardo a su vez en la EEPROM en la posicion correspondiente la hora
         EEPROM.put(2,TIME[1]);//Guardo a su vez en la EEPROM en la posicion correspondiente el minuto
                                //De la alarma correspondiente
        }

        //Misma tematica que lo anterior
      if(Alarm==2){
        EEPROM.put(3,TIME[0]);
        EEPROM.put(4,TIME[1]);
        }
      if(Alarm==3){
        EEPROM.put(5,TIME[0]);
        EEPROM.put(6,TIME[1]);
        }
      lcd.clear();
      break; //Por ultimo rompo el ciclo
    }
    if(codigo.value==b2){break;} //Si presiono el boton 2 solo rompo el ciclo
    delay(50); //Espero un momento
    irrecv.resume(); //Y le digo al sensor IR que borre el cache y se mantenga a la escucha de nuevo
    }
   }
  //**************
 }


//Muestra el tiempo en formato de hora
void showTimeFormat(int hora, int minutos){  
  if(hora<10){ //Si la hora es menor que 10
    lcd.print("0"+(String)hora); //Imprimo un cero antes de la hora eje: 09
   }else{ //sino
    lcd.print((String)hora); //Imprimo sin ajustes eje: 11
   }
  lcd.print(":"); //Coloco los 2 puntos
  if(minutos<10){ //Si los minutos son menores a 10
    lcd.print("0"+(String)minutos); //Imprimo un cero antes del minuto eje: HH:09
   }else{ //Sino
    lcd.print((String)minutos); //Imprimo sin ajustes eje:  HH:11
   }
}
//End: Funciones creadas para el manejo del tiempo



//Begin: funciones para el control de alimento

//Funcion para configurar la cantidad de comida a dispensar
void  setGrams(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Comida Disp.:");
    lcd.setCursor(0,1);
    lcd.print((String)grams+"g 1.Mod 2.Ok"); //Pregunto si desea modificar la cantidad o no mostrando la cantidad anterior de comida 
    irrecv.resume(); //Le digo al sensor IR que borre el cache y espere por respuestas
    //***************
   while(true){ //Mientras el ciclo se repita
    if(irrecv.decode(&codigo)){ //Y si se presiona boton 
    Serial.println(codigo.value,HEX); //Imprimo el codigo en pantalla, en el serial
    if(codigo.value==b1){ // Si el boton presionado fue el 1
     lcd.clear(); 
     lcd.setCursor(0,0);
     lcd.print("Gramos: 0000g");
     lcd.setCursor(0,1);
     lcd.print("Ok para Guardar");
     lcd.setCursor(8,0);
      lcd.blink(); //Imprimo lo anterior en la lcd y le digo que el cursor debe parpadear
      delay(1000); //Espero 1 segundo
     getGrams(); //Y le digo que vaya a la funcion que 
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
      EEPROM.put(7,grams);
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
  int peso=0;
  while(peso<grams){
  peso=balanza.get_units(1);
  Serial.print("Peso: ");
  Serial.print(peso);
  Serial.println(" g");
  }
  m1.write(0);
}
//End: funciones para el control de alimento
//Begin: Funciones para el manejo de servo
void alimentar(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hora de comer");
  lcd.setCursor(0,1);
  lcd.print("Alimentando");
  m1.write(60);
  gramsCheck();
  lcd.clear();
  }
//End:  Funciones para el manejo de servo

void bip(){
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW); 
  delay(500); 
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW);
}

void InicializeData(){
  int eeAddress=0;
  for (int alarma=0;alarma<3;alarma++){
    eeAddress++;
    for(int hora=0; hora<2; hora++){ 
      alarm[alarma][hora]=EEPROM.read(eeAddress);
      if(hora<1){eeAddress++;}
    }
  }
  grams=EEPROM.get(7,  grams); 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if(EEPROM.read(0)==255){
    EEPROM.write(0,0);
  }else{
    Serial.println("Not necessary");  
  }
  m1.attach(PINSERVO,PULSOMIN,PULSOMAX);
  m1.write(0);
  InicializeData();
  //Ajustes del Control Remoto
  irrecv.enableIRIn(); //habilito el sensor
  //Ajustes de la pantalla led:
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  myRTC.setDS1302Time(00,45, 15, 0, 1, 1, 2019);
  TimeSetup();
  balanza.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  balanza.read();
  balanza.set_scale(241.3971937); // Establecemos la escala
  balanza.tare(20);  //El peso actual es considerado Tara. 
  pinMode(buzzer , OUTPUT);  //definir pin como salida
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
   bip();
   alimentar();
   bip();
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
