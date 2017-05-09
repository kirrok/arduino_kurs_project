#include <AFMotor.h>
const int PIN_DO_LEFT = 2;  // Пин прерываний для датчика скорости ЛЕВОГО колеса
const int PIN_DO_RIGHT = 3; // Пин прерываний для датчика скорости ПРАВОГО колеса
volatile unsigned int pulsesLeft;
volatile unsigned int pulsesRight;

AF_DCMotor motor2(4);
AF_DCMotor motor1(3);

char command;

void incrementPulsesLeft() {
 pulsesLeft++;
}

void incrementPulsesRight() {
 pulsesRight++;
}
 
void setup() {
  Serial.println("log.info: setup start");
  
  Serial3.begin(115200);
  Serial.begin(9600);
  
  pinMode(PIN_DO_LEFT, INPUT);
  pinMode(PIN_DO_RIGHT, INPUT);
  pulsesLeft = 0;
  pulsesRight = 0;
  
  attachInterrupt(digitalPinToInterrupt(PIN_DO_LEFT), incrementPulsesLeft, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_DO_RIGHT), incrementPulsesRight, FALLING);
  motor2.setSpeed(200);
  motor2.run(RELEASE);
  motor1.setSpeed(200);
  motor1.run(RELEASE);
  
  Serial.println("log.info: setup end");
}
 
void loop() {
  
//  Serial.print("RPM1 = ");
//  Serial.println(pulsesLeft * 4, DEC);
//  pulsesLeft = 0;
//
//  receiveCommand();
//  doAction(command);
//  delay(50);



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
  Serial.println("log.debug: moveForward");
  motor2.run(FORWARD);
  motor1.run(FORWARD);
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
  Serial.println("log.debug: stopAction");
  motor2.run(RELEASE);
  motor1.run(RELEASE);
}


