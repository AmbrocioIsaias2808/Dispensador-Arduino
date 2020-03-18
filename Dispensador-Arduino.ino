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
               if(aux==6){ //Si es la posicion 6
                  Dh=toNum(codigo.value)*10; //El codigo hexadecimal convertido en numero multiplicado por 10 lo guardo en Decenas de Hora
               }
               if(aux==7){ //Si es la posicion 7
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
    lcd.print((String)((grams+10)*2)+"g 1.Mod 2.Ok"); //Pregunto si desea modificar la cantidad o no mostrando la cantidad anterior de comida 
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
     getGrams(); //Y le digo que vaya a la funcion que lee los gramos introducidos por medio del control remoto
      break; //Despues termina el ciclo while
    }
    if(codigo.value==b2){break;} //Si el boton presionado fue el 2
    delay(50); //Espera 50 milisengundos
    irrecv.resume(); //Despues borro el cache del sensor IR y le digo que resuma la escucha de otras entradas
    }
   }
   lcd.clear(); //Limpio el contenido de la pantalla lcd
  //**************
  }

 void getGrams(){
  aux=8; //Coloco la variable auxiliar en 8, esta variable ayuda a distinguir la posicion del curso.
  int GM=0, GC=0, GD=0, GU=0; 
  /*
  GM variable para almacenar los Millares de los gramos
  GC variable para almacenar las centenas de los gramos
  GD variable para almacenar las decenas de los gramos
  GU variable para almacenar las unidades de los gramos 
  */
  irrecv.resume(); // borro el cache del sensor IR y le digo que resuma la escucha de otras entradas
  //***************
   while(true){ //Mientras el ciclo se repita
    if(irrecv.decode(&codigo)){ //Y si recibe efectivamente algun valor desde el control
    Serial.print(codigo.value,HEX); //Imprime el código por el serial
    if(codigo.value==Bok){ //Si el botón presionado es OK
      grams=0; //La variable gramos se coloca a 0
      grams=((grams+GM+GC+GD+GU)/2)-10; //Despues se suman los gramos + GM+GC+GD+GU, esta suma se divide entre dos y se le resta 10 unidades
      //Esta operación se realiza de tal manera en orden de ajustar la medición mas precisa posible de comida que lee la balanza
      EEPROM.put(7,grams); //El valor se guarda en la EEPROM
      lcd.noBlink(); //Le decimos al LCD que el cursor debe de dejar de parpadear
      Serial.print("Gramos guardados: "); //Se muestra el mensaje por serial
      Serial.print(grams); //Indicando la información guardada
      break; //Y rompe el ciclo
    }

    //El siguiente if esta a la escucha de que alguno de los botones numericos del control sean presionados
    if(codigo.value==b1 || codigo.value==b2 || codigo.value==b3 ||codigo.value==b4 || codigo.value==b5 || codigo.value==b6 ||codigo.value==b7 || codigo.value==b8 || codigo.value==b9 ||codigo.value==b0){
       lcd.print(String(toNum(codigo.value))); //Si se presiona algun valor valido imprime el pantalla el numero correspondiente

               if(aux==8){ //Si el cursor esta en la pocisión 8
                  GM=toNum(codigo.value)*1000; //Se calculan los Millares de unidad
               }
               if(aux==9){ //Si el cursor esta en la pocisión 9
                GC = toNum(codigo.value)*100; //Se calculan las centenas
                }
                if(aux==10){ //Si el cursor esta en la pocisión 10
                  GD=toNum(codigo.value)*10; //Se calculan las decenas
               }
               if(aux==11){ //Si el cursor esta en la pocisión 11
                  GU = toNum(codigo.value); //Se guardan las unidades
                }
       
       lcd.setCursor(aux,0); //despues de ello el valor leido se imprime en pantalla en la posicion indicada
    }
    if(codigo.value==bDerecha){ //Si se presiona el botón derecha
       aux++; //La variable auxiliar se aumenta
       lcd.setCursor(aux,0); //Y se desplaza el cursor
    }
    if(codigo.value==bIzquierda){//Si es izquierda
       aux--; //Se disminuye el valor
       lcd.setCursor(aux,0); //Y se desplaza el cursor
    }
       if(aux==12){ //Si el cursor (aux) es == 12
          aux=11; //Lo retengo en la pocisión 11
          lcd.setCursor(aux,0); } //y lo coloco en pantalla
       if(aux==7){ //Si el cursor (aux) es ==7
          aux=8;  //Lo retengo en la pocisión 8
          lcd.setCursor(aux,0); //Y coloco el cursor
       }
    
    delay(50); //Espero 50 milisegundos
    irrecv.resume(); //Bueno, ya lo repeti muchas veces jejejeje
    }
   }
  //**************
 }

void gramsCheck(){  //Esta función revisa al momento de dispensar, si ya se ha dispensado la cantidad adecuada de alimento
  int peso=0; //De manera que al ser invocada se declara una variable peso igualada a 0
  while(peso<=grams){ //Y entra el while, este se encargara de medir la cantidad de comida que es leida por la celda de carga
    //Mientras el peso leido sea menor o igual a la cantidad de alimento que se programo
  peso=balanza.get_units(1); //Estara leyendo continuamente la información tomando 1 sola muestra.

  /*
  la función get_units(muestras) toma la cantidad de muestras indicada del sensor y una vez tomada devuelve un valor promedio
  En este caso le señalo que solo tome una muestra, pues de otro modo se demora mucho tiempo en el proceso
  */
  
  //Serial.print("Peso: "); //Estas lineas imprimian 
  //Serial.print(peso);     //El peso leido
  //Serial.println(" g");   //En el serial
  }
  m1.write(0); //Una vez que se rompio el ciclo coloco el servomotor en 0° para cerrar la compuerta
}
//End: funciones para el control de alimento


//Begin: Funciones para el manejo de servo
void alimentar(){ //Al momento de que llega la hora de la comida
  lcd.clear(); //Se limpia el contenido de la pantalla 
  lcd.setCursor(0,0); //Se posiciona el cursor
  lcd.print("Hora de comer"); //Se imprime este mensaje 
  lcd.setCursor(0,1); //Posiono el cursor
  lcd.print("Alimentando"); //Imprimo este mensaje
  m1.write(60); //Abro la compuerta colocando el servo en 60°
  gramsCheck(); //Mando llamar a esta función
  lcd.clear();//una vez se regrese el control la compuerta debe estar cerrada y se limpia la pantalla
  }
//End:  Funciones para el manejo de servo

void bip(){ //Hace sonar el buzzer
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW); 
  delay(500); 
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW);
}

void InicializeData(){ //Esta función carga los valores de la EEPROM a las variables de trabajo
  int eeAddress=0; //Coloco esta variable en 0 simbolizando la primer celda de memoria de la EEPROM
  for (int alarma=0;alarma<3;alarma++){ //En este momento recorro el arreglo de las alarmas
    eeAddress++; //Y cada vez que repita aumento la posición de memoria de la EEPROM a leer
    for(int hora=0; hora<2; hora++){  //Al entrar a este for, este recorrera el interno de Alarma, recordemos que este es bidimencional

      /*
      El arrar alarma por tanto consta de 3 filas y dos columnas (para mi es como un array dentro de otro array)
               |  Hora    |   Min      |
      Alarma 1 |          |            |
      Alarma 2 |          |            |
      Alarma 3 |          |            |

      El chiste es que este for interno recorre cada columna de cada fila y va almacenando dichos valores en la celda correspondiente
      
      */

      //De esta manera, en la primera pasada
      alarm[alarma][hora]=EEPROM.read(eeAddress); //Debo guardar en la columna Hora lo almacenado en la posicion correspondiente de la EEPROM
      if(hora<1){eeAddress++;} //Despues, si es que estamos en la primera pasada, aumentamos la posición que deberemos leer de la EEPROM
      //Para que al repetirse el for se lea de la EEPROM los Minutos de la alarma correspondiente 
    }
  }
  grams=EEPROM.get(7,  grams);  //Una vez que terminamos de cargar las alarmas, es en ese entonces que cargamos de la EEPROM los gramos de alimento a dispensar.
}
// AL FIN EL SETUP, perdon pero llevo varios dias comentando el codigo XD
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //Le idico a arduino que debe establecer comunicación con el monitor serial a una frecuencia de 9600
  if(EEPROM.read(0)==255){ //Si es la primera vez que se enciende el dispensador las celdas de la EEPROM deben de contener un valor numerico de 255
    EEPROM.write(0,0); //Por tanto para evitar problemas a la hora de cargar los datos de la EEPROM por primera vez
      
    //*?
    for(int i=1;i<8;i++){
       EEPROM.write(0,0);//Coloco un cero en todas las celdas de memoria que voy a estar ocupando
    }
    //La primera celda (0) no la utilizare para nada mas que para detectar si es la primer encendida del dispensador

    
  }else{ //Si no lo es
    Serial.println("Not necessary");  //No sera necesario hacer el proceso anterior
  }
  m1.attach(PINSERVO,PULSOMIN,PULSOMAX); //Despues coloco las instrucciones para controlar el servomotor
  m1.write(0); //Y le digo que apenas encendiendo la compuerta debe estar cerrada
  InicializeData(); //Despues cargo las variables almacenadas de la EEPROM a las variables de trabajo
  //Ajustes del Control Remoto
  irrecv.enableIRIn(); //habilito el sensor infrarrojo
  //Ajustes de la pantalla led:
  lcd.init(); //Indico a arduino que trabajare con un modulo LCD
  lcd.backlight(); //Indico que debe tener encendida la luz de fondo
  lcd.clear(); //Y que antes de cualquier cosa debe limpiar la pantalla
  lcd.setCursor(0,0); //Y colocar el cursor en la primer linea en la primer columna
  myRTC.setDS1302Time(00,45, 15, 0, 1, 1, 2019); // Y configuro el reloj a las 15:45 en enero de 2019 (la fecha no es importante)
  //Lo anterior solo para que el modulo de reloj empieze a funcionar
  TimeSetup(); //Luego mando llamar esta función donde se pregunta si se desea ajustar manualmente la hora
  balanza.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); //Habilito tambien el uso de la celda de carga
  balanza.read(); //De inicio no debe colocarse nada sobre la balanza, pues de esta manera configuramos su sistema de mediciones y despues
  balanza.set_scale(241.3971937); // Establecemos la escala
  balanza.tare(20);  //El peso actual es considerado Tara. 
  pinMode(buzzer , OUTPUT);  //definir pin como salida
  delay(1000);//Espero un segundo antes de continuar
}

void loop() { //Y llegamos al loop principal
  // put your main code here, to run repeatedly:
  myRTC.updateTime(); //Aquí le digo al reloj que actualice el valor de tiempo
  TIME[0]=myRTC.hours; TIME[1]=myRTC.minutes; TIME[2]=myRTC.seconds; //Para guardar dicho valor en la variables correspondientes
  // Begin: Se imprime la hora con formato en la pantalla lcd
  if((int)TIME[2]==0){lcd.clear();} //Si los segundos son iguales a 0
  lcd.setCursor(0,0); //Posiciono el cursor
  lcd.print("Tiempo: "); //Imprimo esta letania
  showTimeFormat(TIME[0],TIME[1]); //Y mando a llamar la función que me formatea la hora y la imprimira en pantalla (solo imprime hora:minutos)
  lcd.print(":"); //Despues imprimo el : para separar ahora los segundos
  if((int)TIME[2]<10){ //Y si los segundos son menores que 10
  lcd.print("0"+(String)TIME[2]); //Agrego un 0 antes del numero e imprimo en pantalla
  }else //Por el contrario
  {
  lcd.print((String)TIME[2]); //Solo imprimo en pantalla los segundos
  }
  // End: Se imprime la hora con formato en la pantalla lc


  //Ahora, si el tiempo actual coincide con alguna de las alarmas
  if(         ((alarm[0][0] == (int)TIME[0]) && (alarm[0][1]== (int)TIME[1]) && (int)TIME[2]==0)     ||   ((alarm[1][0] == (int)TIME[0]) && (alarm[1][1]== (int)TIME[1]) && (int)TIME[2]==0)           ||        ((alarm[2][0] == (int)TIME[0]) && (alarm[2][1]== (int)TIME[1]) && (int)TIME[2]==0)                 ){
   bip(); //hago sonar el buzzer
   alimentar(); //Dispenso la comida
   bip(); //Y terminado el proceso hago sonar el buzzer
   }
   if(irrecv.decode(&codigo)){ //Ademas me mantendo a la escucha de recibir comandos del control remoto
    Serial.println(codigo.value,HEX); //Si recibo un comando imprimo el codigo en pantalla
    if(codigo.value==bGato){ //Si el boton presionado es GATO
      setTime(); //Entro a la función de configuración de hora
      }
    if(codigo.value==bAsterisco){ //Por el contrario, si se presiona el asterico
      /*Para las alarmas*/
      setAlarms(); //Entro a la función de configuración de alarmas
      }
    if(codigo.value==b0){ //Y si es el 0
      setGrams(); // A la configuración de la cantidad de alimento a dispensar
      }
    delay(50); //Antes de continuar espero 50 milisegundos
    irrecv.resume(); //Y borro el cache e indico que debe mantenerse a la escucha de la introducción de otro código
    }
   
  // Un pequeño delay para no repetir y leer más facil
  delay(1000);
}
