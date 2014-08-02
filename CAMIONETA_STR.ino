/*********************************************************************************************
 This sketch turns reads serial input and uses it to set the pins
 of a 74HC595 shift register.

 Hardware:
 * 74HC595 shift register
 * LEDs attached to each of the outputs of the shift register

 Created 28 Feb 2013
 by Marco Pajares
    Tutorial: Robot 4x4 con Arduino
 ********************************************************************************************/
#include <Arduino.h>
//incluyo librerias para el control IR
#include <NECIRrcv.h>
//incluyo librerias para el control Ultrasonico
#include <Ultrasonic.h>

//defino pin IR
#define IRPIN 10 // ~ PWM
//defino pines de la conexion con la shield de los motores izquierdo y derecho
#define MOTOR1_CTL1  7  // I1
#define MOTOR1_CTL2  6  // I2 ~ PWM
#define MOTOR1_PWM   9 // EA ~ PWM
//motor2 (derecho)
#define MOTOR2_CTL1  3  // I3 ~ PWM
#define MOTOR2_CTL2  4  // I4 
#define MOTOR2_PWM   5 // EB ~ PWM
//Pin connected to latch pin (ST_CP) of 74HC595
#define LATCH        13
//Pin connected to clock pin (SH_CP) of 74HC595
#define CLK          12
////Pin connected to Data in (DS) of 74HC595
#define DATA         11
//defino pines multiplexor
#define MULTI_1      14
#define MULTI_2      15
#define MULTI_3      18
//defino pines Sensor ultrasonidos
#define ULTR_TRIGG 16
#define ULTR_ECHO 17
//defino pin mezcladora (ventilador)
#define MEZCL 19
//defino control IR
unsigned long ircode ;
unsigned long delante= 0xED12BD02;
unsigned long detras= 0xEC13BD02;
unsigned long izquierda= 0xEE11BD02;
unsigned long derecha= 0xEF10BD02;
unsigned long pararse= 0xE916BD02;
unsigned long traseraON= 0xE718BD02;
unsigned long traseraOFF= 0xE51ABD02;
unsigned long delanteraON= 0xE01FBD02;
unsigned long delanteraOFF= 0xF10EBD02;
unsigned long num1 = 0xFF00BD02;
unsigned long num2 = 0xFE01BD02;

//defino la direccion
#define MOTOR_DIR_FORWARD  0
#define MOTOR_DIR_BACKWARD   1

//inicializo el pin de recepcion IR
NECIRrcv ir(IRPIN) ;
// Declaramos el sensor ultrasonico
// En los Pins Digitales elegidos
Ultrasonic ultrasonic(ULTR_TRIGG,ULTR_ECHO); // (Trig PIN,Echo PIN)

void setup()
{
  //IR
  Serial.begin(9600) ;
  Serial.println("NEC IR code reception") ;
  ir.begin() ;
  // Setup pins for MultiPlexor 3-8 + Inverter
   pinMode(MULTI_1, OUTPUT);
   pinMode(MULTI_2, OUTPUT);
   pinMode(MULTI_3, OUTPUT);
  // Setup pins for shiftRegister (74HC595)
   pinMode(LATCH, OUTPUT);
   pinMode(CLK, OUTPUT);
   pinMode(DATA, OUTPUT);
  // Setup pins for motor 1
   pinMode(MOTOR1_CTL1,OUTPUT);
   pinMode(MOTOR1_CTL2,OUTPUT);
   pinMode(MOTOR1_PWM,OUTPUT);
   // Setup pins for motor 2
   pinMode(MOTOR2_CTL1,OUTPUT);
   pinMode(MOTOR2_CTL2,OUTPUT);
   pinMode(MOTOR2_PWM,OUTPUT);
  // Setup pin for Mezcladora
   pinMode(MEZCL,OUTPUT);
}

//metodo para controlar el multiplexor + inverter
void enciendebin(int a){
  if(a==0){
    digitalWrite(MULTI_1, LOW); 
    digitalWrite(MULTI_2, LOW); 
    digitalWrite(MULTI_3, LOW); 
  }
  if(a==1){
    digitalWrite(MULTI_1, LOW); 
    digitalWrite(MULTI_2, LOW); 
    digitalWrite(MULTI_3, HIGH);
  }
  if(a==2){
    digitalWrite(MULTI_1, LOW); 
    digitalWrite(MULTI_2, HIGH); 
    digitalWrite(MULTI_3, LOW);
  }
  if(a==3){
    digitalWrite(MULTI_1, LOW); 
    digitalWrite(MULTI_2, HIGH); 
    digitalWrite(MULTI_3, HIGH);
  }
  if(a==4){
    digitalWrite(MULTI_1, HIGH); 
    digitalWrite(MULTI_2, LOW); 
    digitalWrite(MULTI_3, LOW);
  }
  if(a==5){
    digitalWrite(MULTI_1, HIGH); 
    digitalWrite(MULTI_2, LOW); 
    digitalWrite(MULTI_3, HIGH);
  }
  if(a==6){
    digitalWrite(MULTI_1, HIGH); 
    digitalWrite(MULTI_2, HIGH); 
    digitalWrite(MULTI_3, LOW);
  }
  if(a==7){
    digitalWrite(MULTI_1, HIGH); 
    digitalWrite(MULTI_2, HIGH); 
    digitalWrite(MULTI_3, HIGH);
  }
  delay(1000);
}

//metodo para establecer la velocidad
void setSpeed(char motor_num, char motor_speed)
{
   if (motor_num == 1)
   {
      analogWrite(MOTOR1_PWM, motor_speed);
   }   
   else
   {
      analogWrite(MOTOR2_PWM, motor_speed);
   }
}

//metodo para arrancar el motor
void motorStart(char motor_num, byte direction)
{
   char pin_ctl1;
   char pin_ctl2;
   
   if (motor_num == 1)
   {
      pin_ctl1 = MOTOR1_CTL1;
      pin_ctl2 = MOTOR1_CTL2;
   }   
   else
   {
      pin_ctl1 = MOTOR2_CTL1;
      pin_ctl2 = MOTOR2_CTL2;     
   }
  
   switch (direction)
   {
     case MOTOR_DIR_FORWARD: //HACIA DETRAS
     {
       registerWrite(2, HIGH);
       digitalWrite(pin_ctl1,LOW);
       digitalWrite(pin_ctl2,HIGH);          
     }
     break; 
          
     case MOTOR_DIR_BACKWARD: //HACIA DELANTE
     {
        registerWrite(3, HIGH);
        digitalWrite(pin_ctl1,HIGH);
        digitalWrite(pin_ctl2,LOW);          
     }
     break;         
   }
}

void motorStop(char motor_num)
{
   setSpeed(motor_num, 0);
   if (motor_num == 1)
   {
     digitalWrite(MOTOR1_CTL1,HIGH);
     digitalWrite(MOTOR1_CTL2,HIGH);     
   }
   else
   {
     digitalWrite(MOTOR2_CTL1,HIGH);
     digitalWrite(MOTOR2_CTL2,HIGH);     
   }
   registerWrite(0, HIGH);
}

void acelerar()
{
      Serial.println("Acelerando") ;
      registerWrite(4, HIGH);
      motorStart(1, MOTOR_DIR_BACKWARD);  
      setSpeed(1, 100);
      motorStart(2, MOTOR_DIR_BACKWARD);
      setSpeed(2, 100);
        
        setSpeed(1, 150);
        setSpeed(2, 150);
        
        setSpeed(1, 200);
        setSpeed(2, 200);
        
        setSpeed(1, 250);
        setSpeed(2, 250);
         
}

void desacelerar()
{
   Serial.println("Acelerando") ;
     registerWrite(5, HIGH);
      motorStart(1, MOTOR_DIR_BACKWARD);  
      setSpeed(1, 250);
      motorStart(2, MOTOR_DIR_BACKWARD);
      setSpeed(2, 250);
        
        setSpeed(1, 200);
        setSpeed(2, 200);
        
        setSpeed(1, 150);
        setSpeed(2, 150);
        
        setSpeed(1, 100);
        setSpeed(2, 100);
        
        setSpeed(1, 50);
        setSpeed(2, 50);
        
        motorStop(1);
        motorStop(2);
}

void registerWrite(int whichPin, int whichState) {
// the bits you want to send
  byte bitsToSend = 0;
  // turn off the output so the pins don't light up
  // while you're shifting bits:
  digitalWrite(LATCH, LOW);
  // turn on the next highest bit in bitsToSend:
  bitWrite(bitsToSend, whichPin, whichState);
  // shift the bits out:
  shiftOut(DATA, CLK, MSBFIRST, bitsToSend);
  // turn on the output so the LEDs can light up:
  digitalWrite(LATCH, HIGH);
}

void loop()
{
  //registerWrite(6, HIGH);
  /* Start motors!
  motorStart(1, MOTOR_DIR_FORWARD);  
  setSpeed(1, 300);
  motorStart(2, MOTOR_DIR_FORWARD);      
  setSpeed(2, 300);*/
  long distancia = ultrasonic.Ranging(CM);
  
  Serial.print("Distancia: ");
  Serial.print(distancia); 
  Serial.println(" cm");
  
  if(distancia <= 5){
     registerWrite(5, HIGH);
      motorStart(1, MOTOR_DIR_FORWARD);  
      setSpeed(1, 200);
      motorStart(2, MOTOR_DIR_FORWARD);      
      setSpeed(2, 200);
      delay(1000);
      motorStop(1);
      motorStop(2);
      
    }
   
  while (ir.available()) {
    registerWrite(6, HIGH);
    ircode = ir.read() ;
    Serial.print("codigo: ") ;
    Serial.println(ircode,HEX) ; 
    
    if(ircode == delante){
     /* Serial.println("Hacia Delante") ;
      motorStart(1, MOTOR_DIR_BACKWARD);  
      setSpeed(1, 200);
      motorStart(2, MOTOR_DIR_BACKWARD);      
      setSpeed(2, 200);
      delay(100); */
      acelerar();
    }
    
    if(ircode == detras){
      Serial.println("Hacia Detras") ;
      motorStart(1, MOTOR_DIR_FORWARD);  
      setSpeed(1, 150);
      motorStart(2, MOTOR_DIR_FORWARD);      
      setSpeed(2, 150);
      delay(300);
    }
    
    if(ircode == izquierda){
      Serial.println("Hacia la izquierda") ;
      motorStart(1, MOTOR_DIR_FORWARD);  
      setSpeed(1, 200);
      motorStart(2, MOTOR_DIR_BACKWARD);      
      setSpeed(2, 200);
      delay(300);
    }
    
    
    
    if(ircode == derecha){
      Serial.println("Hacia la derecha") ;
      motorStart(1, MOTOR_DIR_BACKWARD);  
      setSpeed(1, 200);
      motorStart(2, MOTOR_DIR_FORWARD);      
      setSpeed(2, 200);
      delay(300);
    }
    
    if(ircode == pararse){
      registerWrite(5, HIGH);
      Serial.println("STOP") ;
     /* motorStop(1);
      motorStop(2);
      delay(300);*/
      desacelerar();
    }
    
    if(ircode == traseraON){
      Serial.println("LUCES traseras") ;
      registerWrite(4, HIGH);
      enciendebin(1);
      delay(300);
    }
    
    if(ircode == delanteraON){
      Serial.println("LUCES delanteras") ;
      registerWrite(0, HIGH);
      enciendebin(2);
      delay(300);
    }
    
    if(ircode == num1){
      Serial.println("MEZCLADORA") ;
      digitalWrite(MEZCL, HIGH); 
      delay(300);
    }
    
    if(ircode == 2){
      Serial.println("MEZCLADORA") ;
      digitalWrite(MEZCL, LOW); 
      delay(300);
    }
  }
}


