#include <AFMotor.h>
#include "TimerOne.h"
// ---------------------- motors ----------------------------
const int PIN_ENCODER_LEFT = 2;
const int PIN_ENCODER_RIGHT = 3;

volatile unsigned int pulsesLeft;
volatile unsigned int pulsesRight;

volatile int realSpeedLeft;
volatile int realSpeedRight;

AF_DCMotor motor2(4);
AF_DCMotor motor1(3);

// ----------------------  wifi  ----------------------------
char command;

// -------------------- pi-regular --------------------------
float kp=10;
float ki = 0.001;
const float I_MIN = -360;
const float I_MAX = 360;
float iSum = 0;
long timerInterval = 200000;
 
void setup() {
  Serial.println("log.info: setup start");
  
  Serial3.begin(115200);
  Serial.begin(9600);

  Timer1.initialize(timerInterval);
  Timer1.attachInterrupt(timerInterceptor);
  
  pinMode(PIN_ENCODER_LEFT, INPUT);
  pinMode(PIN_ENCODER_RIGHT, INPUT);
  
  pulsesLeft = 0;
  pulsesRight = 0;
  
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_LEFT), incrementPulsesLeft, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_RIGHT), incrementPulsesRight, FALLING);
  
  motor2.setSpeed(255);
  motor2.run(RELEASE);
  motor1.setSpeed(255);
  motor1.run(RELEASE);
  
  Serial.println("log.info: setup end");
}
 
void loop() {  
  Serial.print("left pulses per 0.2 sec = ");
  Serial.println(realSpeedLeft, DEC);

  receiveCommand();
  doAction(command);
  delay(200);
}

float PIctl(float error) {
 float up, ui;
 // Пропорциональная компонента
 up = kp*error;
 
 // Интегральная компонента
 iSum = iSum+error; // Накапливаем (суммируем)
 
 // Проверяем граничные значение
 if(iSum<I_MIN) {
  iSum = I_MIN; 
 }
 if(iSum>I_MAX) {
  iSum = I_MAX;
 }
 ui = ki*iSum;
 return up+ui;
}

void timerInterceptor() {
  Serial.println("log.debug timerInterceptor");
  realSpeedLeft = pulsesLeft;
  pulsesLeft = 0;
  realSpeedRight = pulsesRight;
  pulsesRight = 0;  
}

void incrementPulsesLeft() {
  pulsesLeft++;
  delay(1);
}

void incrementPulsesRight() {
 pulsesRight++;
}

void doAction(const char direction) {
//  Serial.println("log.debug: doAction " + direction);
  switch(direction) {
    case 'f':
      moveForward();
      break;
    case 'b':
      moveBack();
      break;
    case 'r':
      turnRight();
      break;
    case 'l':
      turnLeft();
      break;
    case '!':
      stopAction();
      break;      
  }
}

void receiveCommand() {
  while (Serial3.available() > 0) {
    command = Serial3.read();
    Serial.println("log.debug: receiveCommand");
  }
}

void moveForward() {
//  Serial.println("log.debug: moveForward");
  motor2.run(FORWARD);
//  motor1.run(FORWARD);
}

void moveBack() {
  Serial.println("log.debug: moveBack");
  motor2.run(BACKWARD);
  motor1.run(BACKWARD);
}

void turnRight() {
  Serial.println("log.debug: turnRight");
}

void turnLeft() {
  Serial.println("log.debug: turnLeft");
}

void stopAction(){
//  Serial.println("log.debug: stopAction");
  motor2.run(RELEASE);
  motor1.run(RELEASE);
}

void sendTelemetry() {
//  Serial3.write("?rpm_left=" + 
}

