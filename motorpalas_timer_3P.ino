#include <LiquidCrystal_I2C.h>
#include<TimerOne.h>
#include <Wire.h>


//PINES DE CONTROL DEL MOTOR
#define STEPPIN 12
#define DIRPIN 11
#define ENAPIN 10
#define ACTIONPIN 8                        //acciona el arranque del motor

#define Led 7                             //señala que el timer 1 esta activo parpadeando 
//PINES ANALOGICOS DE AJUSTE DEL TIEMPO 
#define seconds A1                         //ajusta con potenciometro el tiempo de segundo 
#define minuts A2                          //ajusta con potenciometro el tiempo de minutos 
#define hours A3                           //ajusta con potenciometro el tiempo de horas 
#define menu A0                           //inicia y abre el menu de temporizador
#define fin 4
//#define eleccion 4
//PINES DIGITALES DE AJUSTE DEL TIEMPO 
#define temp 14
#define inicio 15

//VARIABLES DE LECTURA ANALOGICA 
int Valor_s=0;
int Valor_m=0;
int Valor_h=0;
int Valor_b=0;
int selector=1;

//VARIABLES DEL TEMPORIZADOR
volatile int tiempo_t=0;
int acceso=0;
//volatile int seg=0;
//volatile int minut=0;
//volatile int hora=0;
int Start=0;
int hold=0;
char Text[10];

//VARIABLES DE CONTROL DE MOTORES 
const int STEPTIME = 3;

//LIBRERIA DE PANTALLA LCD CON I2C
//#include "DFRobot_LCD.h"
LiquidCrystal_I2C lcd(0x27,20,4); 

//VARIABLES DE PANTALLA LCD 
  const int colorR = 255;
  const int colorG = 0;
  const int colorB = 0;
//  DFRobot_LCD lcd(16,2);  //16 characters and 2 lines of show

void setup() {
  // put your setup code here, to run once:
  //asignacion de pines de motores
   pinMode(STEPPIN,OUTPUT);
   pinMode(DIRPIN,OUTPUT);
   pinMode(ENAPIN,OUTPUT); 
   pinMode(ACTIONPIN,INPUT);
   pinMode(Led, OUTPUT);
   //inicializacion de pantalla lcd
  // lcd.init();
   //lcd.setRGB(colorR, colorG, colorB);//If the module is a monochrome screen, you need to shield it
   lcd.init();         //prende la pantalla 
   lcd.backlight();
   lcd.print("prueba");
   delay(1000);
   lcd.clear();
   lcd.print("Timer");
   delay(500);
   lcd.clear();
  /* sprintf(Text," seg min hor");
   lcd.print(Text);
   lcd.setCursor(1,2);
  /* seg=0;
   minut=0;
   sprintf(Text," 0%d : 0%d : 0%d",seg,minut,hora);
   lcd.print(Text);*/
   //Timer1.initialize(1000000);                      //Configura el timer en 1 segundo 
  // Timer1.attachInterrupt(Temporizador);            //Configura la interrupcion del timer 1
}

void loop() {
  // put your main code here, to run repeatedly:
  int seg=0;
  int minut=0;
  int hora=0;
  //int selector=0;
  lcd.setCursor(0,0);
  sprintf(Text," seg min hor");
  lcd.print(Text);
  //lcd.print("entra?");
  //delay(500);
  //lcd.clear();
  lcd.setCursor(1,2);
  sprintf(Text," 0%d : 0%d : 0%d",seg,minut,hora);
  lcd.print(Text);
  Valor_b= analogRead(menu);
  if((Valor_b>=179)&&(Valor_b<=181))acceso=1;
  while(acceso==1)                                      //Ciclo de configuración del tiempo
  {
     Valor_s=analogRead(seconds);
      seg=(Valor_s*60)/1023;                           //transforma la lectura del potenciometro en una escala de 60 para SEGUNDOS
      if(seg<0)seg=seg+64;                           //esta linea es para compensar el extraño error de conversion, no me explico el por que descompensa a partir de la mitad un valor de 64
      lcd.setCursor(1,1);
      if(seg<10)lcd.print("0");                     //condicional estetico para valores inferiores a 10
      lcd.print(seg);
      //lcd.clear();
      Valor_m=analogRead(minuts);
      minut=(Valor_m*60)/1023;                        //transforma la lectura del potenciometro en una escala de 60 para MINUTOS 
      if(minut<0)minut=minut+64;                    //esta linea es para compensar el extraño error de conversion, no me explico el por que descompensa a partir de la mitad un valor de 64
      lcd.setCursor(6,1);
      if(minut<10)lcd.print("0");                   //condicional estetico para valores inferiores a 10
      lcd.print(minut);
      Valor_h=analogRead(hours);
      hora=(Valor_h*24)/1023;                           //transforma la lectura del potenciometro en una escala de 60 para HORAS
      if(hora<0)hora=hora+28;                         //esta linea es para compensar el extraño error de conversion, no me explico el por que descompensa a partir de la mitad un valor de 28
      lcd.setCursor(10,1);
      if(hora<10)lcd.print("0");                      //condicional estetico para valores inferiores a 10
      lcd.print(hora);
      Valor_b=analogRead(menu);
      if(Valor_b>=465 && Valor_b<=468)
      {
        Starter(seg,minut,hora);
        break;
      }
    }                                               //fin del while de segundos 
                            
  }



void Temporizador(void)
{
  digitalWrite(Led, digitalRead(Led)^1);   //invierte el estado del led
  if(Start==1)                             //condicional para que se efectue la cuenta regresiva 
  {
    tiempo_t--;
   // sprintf(Text,"tiempo: %d",tiempo);
   // lcd.print(tiempo);
   if(tiempo_t==0)                           //Reinicia los conteos a 0 para volver a empezar
    {
      Start=2;
      //seg=0;
      //minut=0;
      //hora=0;
      digitalWrite(fin,HIGH);
      digitalWrite(ENAPIN,HIGH);
    
    }
  }
}

void Starter(int seg,int minut, int hora)
{
  tiempo_t=seg+(minut*60)+(hora*3600);
  lcd.clear();
  lcd.print(tiempo_t);
  //delay(400);
  acceso=0;                                     // activa la condicion falsa para el while de configuracion
  //selector=0;
  Start=1;                                         //Activa en las interrupciones la ejecucion de la cuenta regresiva
  Timer1.initialize(1000000);                      //Configura el timer en 1 segundo 
  Timer1.attachInterrupt(Temporizador);            //Configura la interrupcion del timer 1
  digitalWrite(ENAPIN,LOW);
  do{
      forward(200);
    }
  while(tiempo_t>0);
}

void forward(int steps){
  int i;
 // digitalWrite(ENAPIN,LOW);//ENABLE IS ACTIVE LOW
  digitalWrite(DIRPIN,HIGH);//SET DIRECTION 
  for(i=0;i<steps;i++){
    digitalWrite(STEPPIN,HIGH);
    delay(STEPTIME);
    digitalWrite(STEPPIN,LOW);
    delay(STEPTIME);
  }
 //digitalWrite(ENAPIN,HIGH);//DISABLE STEPPER
}

void reverse(int steps){
  int i;
 // digitalWrite(ENAPIN,LOW);//ENABLE IS ACTIVE LOW
  digitalWrite(DIRPIN,LOW);//SET DIRECTION 
  for(i=0;i<steps;i++){
    digitalWrite(STEPPIN,HIGH);
    delay(STEPTIME);
    digitalWrite(STEPPIN,LOW);
    delay(STEPTIME);
  }
  digitalWrite(ENAPIN,HIGH);//DISABLE STEPPER
}
