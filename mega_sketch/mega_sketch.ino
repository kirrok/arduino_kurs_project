//При интервале таймера счетчика импульсов энкодера в 200000 (0.2 мc)
//Скорость                  Мотор 1   Мотор 2
//
//При скорости 255          8-12      27-32
//
//При скорости 150          4-11      16-20

//При интервале таймера счетчика импульсов энкодера в 1000000 (1 c)
//Скорость                  Мотор 1   Мотор 2
//
//При скорости 255          55-60     138-141
//
//При скорости 150          38        84
//
//При скорости 100          стоит    45-50

//Скорости выравниваются при 200 и 100


#include <AFMotor.h>
#include "TimerOne.h"
// ---------------------- motors ----------------------------
const int PIN_ENCODER_LEFT = 2;
const int PIN_ENCODER_RIGHT = 3;

AF_DCMotor motor2(4);
AF_DCMotor motor1(3);

// ----------------------  encoders  ----------------------------

// 20 .. 50
volatile unsigned int pulsesLeft;
volatile unsigned int pulsesRight;
volatile int pulsesLeftPerTimeUnit;
volatile int pulsesRightPerTimeUnit;

// ----------------------  wifi  ----------------------------
char command;

// -------------------- timer --------------------------
long timerInterval = 1000000; // 0.2 секунды


// -------------------- pi-regular --------------------------
// Диапазон сигнала управления
float minControlActionLeft = 100;
float maxControlActionLeft = 150;
float maxErrorLeft = 30;

float minControlActionRight = 200;
float maxControlActionRight = 255;
float maxErrorRight = 50;

struct PI_CONTROLLER {
  float Kp;
  float Ki;
  float integral;
  float min_integral;
  float max_integral;

  PI_CONTROLLER(float Kp0, float Ki0, float mini, float maxi) {
    Kp = Kp0;
    Ki = Ki0;
    integral = 0;
    max_integral = maxi;
    min_integral = mini;
  }

  float Eval(float err) {
    integral = integral + err;
    if (integral > max_integral) integral = max_integral;
    if (integral < min_integral) integral = min_integral;
    return Kp * err + Ki * integral;
  }

  float y2u(float y,float maxError, float minControlAction, float maxControlAction) {
    float u;
    float maxY = Kp * maxError;
    //    Serial.print("maxY: ");
    //    Serial.print(maxY, DEC);
    float minY = -maxY;
    u = (y - minY) / (maxY - minY) * (maxControlAction - minControlAction) + minControlAction;
    //    Serial.print("u: ");
    //    Serial.print(u, DEC);
    if (u > maxControlAction) u = maxControlAction;
    if (u < minControlAction) u = minControlAction;
    //    Serial.print("u: ");
    //    Serial.print(u, DEC);
    return u;
  }
};

PI_CONTROLLER piControllerLeft(2, 0.2, -100, 100);
PI_CONTROLLER piControllerRight(2, 0.2, -100, 100);

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

  motor2.run(RELEASE);
  motor1.run(RELEASE);

  Serial.println("log.info: setup end");
}

int goalSpeedLeft = 40;
int goalSpeedRight = 40;

volatile int leftSpeedCurrent;
void loop() {

  float errorLeft = goalSpeedLeft - pulsesLeftPerTimeUnit;
  float YLeft = piControllerLeft.Eval(errorLeft);
  float ULeft = piControllerLeft.y2u(YLeft, maxErrorLeft, minControlActionLeft, maxControlActionLeft);

    motor2.setSpeed(ULeft);
    motor2.run(FORWARD);

  float errorRight = goalSpeedRight - pulsesRightPerTimeUnit;
  float YRight = piControllerRight.Eval(errorRight);
  float URight = piControllerRight.y2u(YRight, maxErrorLeft, minControlActionRight, maxControlActionRight);

  motor1.setSpeed(URight);
  motor1.run(FORWARD);
  
  Serial.print("g: ");
  Serial.print(goalSpeedLeft, DEC);
  Serial.print(" speed. ");
  Serial.print("left: ");
  Serial.print(pulsesLeftPerTimeUnit, DEC);
  Serial.print("   right: ");
  Serial.print(pulsesRightPerTimeUnit, DEC);
  //  Serial.print("   y: ");
  //  Serial.print("   left: ");
  //  Serial.print(YLeft, DEC);
  //    Serial.print("   right: ");
  Serial.print("   ");
  Serial.print(YRight, DEC);

  Serial.print(" errorLeft ");
  Serial.print(errorLeft, DEC);
  Serial.print(" errorRight ");
  Serial.print(errorRight, DEC);
  Serial.print(" ul ");
  Serial.print(ULeft, DEC);
  Serial.print(" ur ");
  Serial.print(URight, DEC);
  //  receiveCommand();
  //  doAction(command);
  Serial.println();
  delay(50);

}

void timerInterceptor() {
  pulsesLeftPerTimeUnit = pulsesLeft;
  pulsesLeft = 0;
  pulsesRightPerTimeUnit = pulsesRight;
  pulsesRight = 0;
}

void incrementPulsesLeft() {
  pulsesLeft++;
}

void incrementPulsesRight() {
  pulsesRight++;
}

void doAction(const char direction) {
  //  Serial.println("log.debug: doAction " + direction);
  switch (direction) {
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

void stopAction() {
  //  Serial.println("log.debug: stopAction");
  motor2.run(RELEASE);
  motor1.run(RELEASE);
}

void sendTelemetry() {
  //  Serial3.write("?rpm_left=" +
}
