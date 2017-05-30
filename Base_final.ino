/*SFIT ROBOCON TEAM
 *CONFIDENTIAL
  23/02/2017
 *Wall following locomotion code BETA
 *Straight Outta Programmer's Desk
 *
*/

//BETA

#include <Cytron_PS2Shield.h>
#include <Wire.h>

Cytron_PS2Shield ps2(10,11); //Rx, Tx


#define MAX_ERROR 40
#define MIN_ERROR -40
#define pwm1 7
#define pwm2 6
#define pwm3 8
#define pwm4 9
#define dir1 28
#define dir2 30
#define dir3 24
#define dir4 26

#define TRIG1 43 // ultra 1
#define ECHO1 45
#define TRIG2 47 // ultra 2 
#define ECHO2 49
#define RELAY1 35
#define RELAY2 37
#define manualSpeed 100 // speed for manual locomotion // initially it was 40
#define turnSpeed 60

void setSpeed(float);
void readPS2(void);
void sendData(char);
int calculateError(void);
void accelerate(void);
void manualLoco(void);
void shoot(int);
int setPoint = 65;
int currentSpeed = 0;

void setup(){
  Wire.begin();
  ps2.begin(9600);
  Serial.begin(9600);
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
  pinMode(pwm4, OUTPUT);
  
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(dir3, OUTPUT);
  pinMode(dir4, OUTPUT);
//  pinMode(AN, INPUT);
//  digitalWrite(UEN, LOW);
  pinMode(TRIG1, OUTPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(ECHO2, INPUT);
  
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 0);
//pins for relay
//  pinMode(32,OUTPUT);
//  digitalWrite(32, HIGH);
  pinMode(RELAY1,OUTPUT);
  pinMode(RELAY2,OUTPUT); 
//  pinMode(SLAVE_RESET, OUTPUT);
//  digitalWrite(SLAVE_RESET, LOW); 
 }

int integral = 0; 
int proportional = 0;
int derivative = 0;
float kp = 1.8;
float kd = 1;
//float ki = 0;
long int prevMillis = 0;
int lastError = 0;
int foo = 0;
int error = 0;
int averageSpeed = 150; // initially 150
bool change = false;
int bar = 0; // joystick right (manual loco)
int relayPin = RELAY2; //R1 default relayPin
//bool toggle = false;

void loop() {

  readPS2();
   //Serial.println(foo); 
  if(foo){ 
  //  averageSpeed = foo + 30;
  //Line following section
 /*         
      error = map(analogRead(AN),0,910,-20,20);
      if(error > 20 )
        error = lastError;
*/
  //Wall following section
    error = calculateError();
  //  error = 0;
    if(error > MAX_ERROR)
      error = MAX_ERROR;
    else if(error < MIN_ERROR)
      error = MIN_ERROR;
 //   if(error > -8 && error < 8)
 //     integral = 0;
    //Serial.println(error);
    proportional = error;
    derivative = (error - lastError);
  //  integral = integral + error;
    float turn = proportional*kp + derivative*kd;
//      Serial.print("turn   = ");
//      Serial.println(turn);   
//      prevMillis = millis();
      setSpeed(turn);  
    } 
//    Serial.print(" \t");
//    Serial.println(error); 
}


void setSpeed(float turn){
      int speed1 = averageSpeed + turn;
      int speed2 = averageSpeed - turn;
    
      if(speed1 > 255) 
        speed1 = 255;
      if(speed2 > 255)
        speed2 = 255;
      if(speed1 < 0)
        speed1 = 0;
      if(speed2 < 0)
        speed2 = 0;
        
      analogWrite(pwm1, speed1);
      analogWrite(pwm2, speed1);
      analogWrite(pwm3, speed2);
      analogWrite(pwm4, speed2);
//      Serial.print(speed1);
//      Serial.print("\t");
//      Serial.println(speed2);      
//      Serial.print(" \tError = ");
//      Serial.println(error);
//      Serial.print("Turn = ");
//      Serial.println(turn);
      lastError = error; 
}

void readPS2(){
 // Serial.println("zxc");
    if(!ps2.readButton(PS2_TRIANGLE)){
        sendData('U');
        while(!ps2.readButton(PS2_TRIANGLE));
      
      }
    else if(!ps2.readButton(PS2_CROSS)){
        sendData('D');
        while(!ps2.readButton(PS2_CROSS));

      }
    else if(!ps2.readButton(PS2_UP)){
        sendData('+');
        while(!ps2.readButton(PS2_UP));

      }
    else if(!ps2.readButton(PS2_DOWN)){
        sendData('-');
        while(!ps2.readButton(PS2_DOWN));
       
      }
    else if(!ps2.readButton(PS2_LEFT_1)){
        sendData('L');
        while(!ps2.readButton(PS2_LEFT_1));
       // relayPin = 37;
      }
    else if(!ps2.readButton(PS2_LEFT_2)){
        sendData('Z');
        while(!ps2.readButton(PS2_LEFT_2));
      //  relayPin = 35;
      }
     else if(!ps2.readButton(PS2_RIGHT_1)){
       // shoot(RELAY1);
        digitalWrite(RELAY1, HIGH);
        delay(100);
        digitalWrite(RELAY1, LOW);
       // relayPin = 37;
      }
    else if(!ps2.readButton(PS2_CIRCLE) || !ps2.readButton(PS2_SQUARE)){
     /*   sendData('S');
        while(!ps2.readButton(PS2_CIRCLE) || !ps2.readButton(PS2_SQUARE));
        delay(500);
        sendData('X'); */
        shoot(relayPin);
      //  Serial.println("back circle");
      }
 /*   else if(!ps2.readButton(PS2_LEFT_2) || !ps2.readButton(PS2_RIGHT_2)){
//       /* sendData('S');
//        
//        delay(100);
//        sendData('X');
//         */
//        relayPin = 37;
//        shoot(relayPin);
//        while(!ps2.readButton(PS2_SQUARE));
//        Serial.println("back square");
//          digitalWrite(SLAVE_RESET, HIGH);
//          delay(500);
//          digitalWrite(SLAVE_RESET, LOW);
//      } 
// ULTRA dependent Joystick
    else if(ps2.readButton(PS2_JOYSTICK_LEFT_UP)){
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, HIGH);
      digitalWrite(dir3, HIGH);
      digitalWrite(dir4, HIGH);
      foo = ps2.readButton(PS2_JOYSTICK_LEFT_UP);
      while(currentSpeed <= averageSpeed && foo){
       // Serial.println(foo);
        accelerate();
        foo = ps2.readButton(PS2_JOYSTICK_LEFT_UP);
        }
        if(!foo){
        freeze();
        }  
     }
    else if(ps2.readButton(PS2_JOYSTICK_LEFT_DOWN)){
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, LOW);
      digitalWrite(dir3, LOW);
      digitalWrite(dir4, LOW);
      foo = ps2.readButton(PS2_JOYSTICK_LEFT_DOWN);
      while(currentSpeed <= averageSpeed && foo){
       // Serial.println(foo);
        accelerate();
        foo = ps2.readButton(PS2_JOYSTICK_LEFT_DOWN);
      }
      if(!foo){
          freeze();
        } 
    }
// Manual override joystick
    else if(ps2.readButton(PS2_JOYSTICK_RIGHT_UP)){
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, HIGH);
      digitalWrite(dir3, HIGH);
      digitalWrite(dir4, HIGH);
      bar = ps2.readButton(PS2_JOYSTICK_RIGHT_UP);
      while(currentSpeed <= manualSpeed && bar){
       // Serial.println(foo);
        accelerate();
        bar = ps2.readButton(PS2_JOYSTICK_RIGHT_UP);
        }
        if(!bar){
        freeze();
        } 
    //  Serial.println(bar);
       }

    else if(ps2.readButton(PS2_JOYSTICK_RIGHT_DOWN)){
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, LOW);
      digitalWrite(dir3, LOW);
      digitalWrite(dir4, LOW);
      bar = ps2.readButton(PS2_JOYSTICK_RIGHT_DOWN);
      while(currentSpeed <= manualSpeed && bar){
       // Serial.println(foo);
        accelerate();
        bar = ps2.readButton(PS2_JOYSTICK_RIGHT_DOWN);
        }
        if(!bar){
        freeze();
        }
   //   Serial.println(bar);
      }
    else if(ps2.readButton(PS2_JOYSTICK_RIGHT_LEFT)){
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, HIGH);
      digitalWrite(dir3, LOW);
      digitalWrite(dir4, LOW);
      bar = ps2.readButton(PS2_JOYSTICK_RIGHT_LEFT);
//      while(currentSpeed <= manualSpeed && bar){
//       // Serial.println(foo);
//        accelerate();
//        bar = ps2.readButton(PS2_JOYSTICK_RIGHT_LEFT);
//        }
//        if(!bar){
//        freeze();
//        }
      while(bar){
          manualLoco();
          bar = ps2.readButton(PS2_JOYSTICK_RIGHT_LEFT);
        }
      analogWrite(pwm1, 0);
      analogWrite(pwm2, 0);
      analogWrite(pwm3, 0);
      analogWrite(pwm4, 0); 
    //  Serial.println(bar);
     }
    else if(ps2.readButton(PS2_JOYSTICK_RIGHT_RIGHT)){
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, LOW);
      digitalWrite(dir3, HIGH);
      digitalWrite(dir4, HIGH);
      bar = ps2.readButton(PS2_JOYSTICK_RIGHT_RIGHT);
      while(bar){
          manualLoco();
          bar = ps2.readButton(PS2_JOYSTICK_RIGHT_RIGHT);
        }      
      analogWrite(pwm1, 0);
      analogWrite(pwm2, 0);
      analogWrite(pwm3, 0);
      analogWrite(pwm4, 0);      
   //   Serial.println(bar);
      }
   else{
      foo = 0;
      bar = 0;
      freeze();
   }
}

void sendData(char bar){
      Wire.beginTransmission(8);
      Wire.write(bar);      
      Wire.endTransmission();
     // Serial.println(bar);
}

int calculateError(){
     //int a = analogRead(SENSOR_FRONT);
     //int b = analogRead(SENSOR_BACK);
     digitalWrite(TRIG1, LOW);
     delayMicroseconds(2);
     digitalWrite(TRIG1, HIGH);
     delayMicroseconds(5);
     digitalWrite(TRIG1, LOW);

     int a = pulseIn(ECHO1, HIGH)/29/2;
   //  int a=60;
     digitalWrite(TRIG2, LOW);
     delayMicroseconds(2);
     digitalWrite(TRIG2, HIGH);
     delayMicroseconds(5);
     digitalWrite(TRIG2, LOW);

     int b = pulseIn(ECHO2, HIGH)/29/2;
     Serial.print(a);
     Serial.print(", ");
     Serial.println(b); 
     if(a <= 0 || b <= 0 || a > 170 || b > 170){
      freeze();
      delay(2000);
    //  foo = 0;
      return 0;
     }
     if(digitalRead(dir1) || digitalRead(dir2)){
        return ((a - setPoint) + (a - b)/5);
     }
    else{
        return ((b - setPoint) + (b - a)/5);
      }
}

void manualLoco(){
    analogWrite(pwm1, turnSpeed);
    analogWrite(pwm2, turnSpeed);
    analogWrite(pwm3, turnSpeed);
    analogWrite(pwm4, turnSpeed);
  }
void accelerate(){
        analogWrite(pwm1, currentSpeed);
        analogWrite(pwm2, currentSpeed);
        analogWrite(pwm3, currentSpeed);
        analogWrite(pwm4, currentSpeed);
        delay(4);
        currentSpeed++;
        Serial.println(currentSpeed);             
  }
void freeze(){
    for( ; currentSpeed > 0; currentSpeed--){
        analogWrite(pwm1, currentSpeed);
        analogWrite(pwm2, currentSpeed);
        analogWrite(pwm3, currentSpeed);
        analogWrite(pwm4, currentSpeed);
        delay(5);
       // Serial.println(currentSpeed);
      }
      foo = 0;
      bar = 0;
      analogWrite(pwm1, 0);
      analogWrite(pwm2, 0);
      analogWrite(pwm3, 0);
      analogWrite(pwm4, 0);
  }


void shoot(int r){
/*  if(!toggle){
    digitalWrite(r, HIGH);
    toggle = true; 
  }
  else{
    digitalWrite(r, LOW);
    toggle = false;
  }
  Serial.println(digitalRead(r)); */
  digitalWrite(r, HIGH);
  delay(500);
  digitalWrite(r, LOW);
  Serial.println("shoot");
  }
