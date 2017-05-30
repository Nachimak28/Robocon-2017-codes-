#include <TM1637Display.h>
#include <Arduino.h>
//#include <EEPROM.h>
/*SFIT ROBOCON TEAM
 *CONFIDENTIAL
 *18/02/2017
 *promise day
 *Locomotion Code
 *Straight Outta Programmer's Desk
 *Theme: Asobi(The flying disc)
*/
// motor 1 is for angle
//motor 2 and motor 3 is for shooting

#include<Wire.h>

void action(char x);
void receive(void);
void setAngle(int);

#define CLK1 51
#define DIO1 53
#define CLK2 47
#define DIO2 49
#define m1p1 26
#define m1p2 24
#define m2p1 28
#define m2p2 30
#define m3p1 32
#define m3p2 34
#define pwm1 12
#define pwm2 11
#define pwm3 10
#define STEP 100
//#define RELAY1 37
//#define RELAY2 35

TM1637Display displayAngle= TM1637Display(CLK1, DIO1);
TM1637Display displaySpeed= TM1637Display(CLK2, DIO2);

int MAX = 200;
int MIN = 24;
int defaultSpeed = 24;
//int relayPin = RELAY1;
int encoderPin1 = 2;
int encoderPin2 = 3;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0; 
int lastMSB = 0;
int lastLSB = 0;
long int bar = 0;
int dir = 0;
//bool side = false;
//int addr = 0; // EPROM address to store encoder value

void updateEncoder(void);

void setup() {
 // EEPROM.write(0,0);
 // EEPROM.write(1,0);
  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(pwm1, OUTPUT);
  digitalWrite(m1p1, LOW);
  digitalWrite(m1p2, LOW);
  analogWrite(pwm1, 50);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);
  pinMode(pwm2, OUTPUT);
  digitalWrite(m2p1, HIGH);
  digitalWrite(m2p2, LOW);
  analogWrite(pwm2, 0);

  pinMode(m3p1, OUTPUT);
  pinMode(m3p2, OUTPUT);
  pinMode(pwm3, OUTPUT);
  digitalWrite(m3p1, HIGH);
  digitalWrite(m3p2, LOW);
  analogWrite(pwm3, 0);
//  pinMode(RELAY1, OUTPUT);
//  pinMode(RELAY2, OUTPUT);
  
  Wire.begin(8);
  Wire.onReceive(receive);

  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
 
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
 
  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(3), updateEncoder, CHANGE);
  //if(EEPROM.read(1) == 1)
  //  encoderValue = -EEPROM.read(addr)*100;
  //else
  //  encoderValue = EEPROM.read(addr)*100;
  //bar = encoderValue;
  Serial.begin(9600);
//  Serial.println(EEPROM.read(addr));
 // Serial.println(bar);
  //7 segment functions
  displaySpeed.setBrightness(20);
  displayAngle.setBrightness(20);
  displaySpeed.showNumberDec(0);
  displayAngle.showNumberDec(encoderValue);
  
}

void loop() {
    displayAngle.showNumberDec(abs(encoderValue)/100);
   // Serial.println("I'm ON");
    if(dir == 1){
       while(encoderValue > (bar - STEP)){
        //  Serial.println(encoderValue);
            displayAngle.showNumberDec(abs(encoderValue)/100);
        }
        digitalWrite(m1p1, LOW);
        digitalWrite(m1p2, LOW);
        dir = 0;
    //    EEPROM.write(addr, abs(encoderValue)/100);
    //    if(encoderValue < 0){
    //        EEPROM.write(1, 1);
    //      }
    //    else
    //        EEPROM.write(1, 0);
      }
     if(dir == 2){
        while(encoderValue < (bar + STEP)){
        //   Serial.println(encoderValue); 
            displayAngle.showNumberDec(abs(encoderValue)/100); 
        }
        digitalWrite(m1p1, LOW);
        digitalWrite(m1p2, LOW);
        dir = 0;
     //   EEPROM.write(addr, abs(encoderValue)/100);
     //   if(encoderValue < 0)
     //       EEPROM.write(1, 1);
     //   else
     //       EEPROM.write(1, 0);
      }
     
        
}

void receive(){  
 // while(1 < Wire.available()){
 //   char c = Wire.read();
 //   Serial.println(c);
 //   }
  char foo = Wire.read();
 // Serial.println(foo);
  action(foo);  
}

void action(char foo){
  
  if(foo == '+'){
    defaultSpeed += 3;
    if(defaultSpeed > MAX){
        defaultSpeed = MAX;
      }
    analogWrite(pwm2, defaultSpeed);
    analogWrite(pwm3, defaultSpeed);
   // Serial.println(defaultSpeed);
    displaySpeed.showNumberDec(defaultSpeed);
   // Serial.println("inside +");
    
  }
  
  else if(foo == '-'){
    defaultSpeed -= 3;
    if(defaultSpeed < MIN){
      defaultSpeed = MIN;
      analogWrite(pwm2, 0);
      analogWrite(pwm3, 0);
     // Serial.println(defaultSpeed);
      displaySpeed.showNumberDec(0);
      return;
      }
    analogWrite(pwm2, defaultSpeed);
    analogWrite(pwm3, defaultSpeed);
   // Serial.println(defaultSpeed);
    displaySpeed.showNumberDec(defaultSpeed); 
   // Serial.println("inside -");   
  }

  else if(foo == 'L'){
      digitalWrite(m2p1, LOW);
      digitalWrite(m2p2, HIGH);
      digitalWrite(m3p1, LOW);
      digitalWrite(m3p2, HIGH);
//      side = true;
//      Serial.println("DIR1");
    }
  else if(foo == 'R'){
      digitalWrite(m2p1, HIGH);
      digitalWrite(m2p2, LOW);
      digitalWrite(m3p1, HIGH);
      digitalWrite(m3p2, LOW);
//      side = false;
//      Serial.println("DIR2");    
    }
  else if(foo == 'U'){
      digitalWrite(m1p1, HIGH);
      digitalWrite(m1p2, LOW);
      bar = encoderValue;
      dir = 1;
    }
  else if(foo == 'D'){
      digitalWrite(m1p1, LOW);
      digitalWrite(m1p2, HIGH);
      bar = encoderValue;
      dir = 2;
  }
/*  else if(foo == 'S'){
    if(!side){
      relayPin = RELAY2;
      }
    else{ 
      relayPin = RELAY1;
      }
    digitalWrite(relayPin, HIGH);
    Serial.println(relayPin);
  }
  else if(foo == 'X'){
    digitalWrite(relayPin, LOW);
    Serial.println("off");
    } */
  else
   // Serial.println("Error bro");
}
  
void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue--;
 
  lastEncoded = encoded; //store this value for next time
 // Serial.println(encoderValue);
}
  
