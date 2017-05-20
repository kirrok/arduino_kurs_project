#include <AFMotor.h>
#include "TimerOne.h"
// ---------------------- motors ----------------------------
const int PIN_ENCODER_LEFT = 2;
const int PIN_ENCODER_RIGHT = 3;
const float ENCODER_COEFFICIENT = 8.88;

/*info
  255 - 27..30 pulses per 0.2 sec среднее 28.5
  100 - 8..12 pulses per 0.2 sec среднее 10

  k = 8.33..9.44 среднее 8.88
*/

int goalSpeedLeft = 20;
int goalSpeedRight = 20;

AF_DCMotor motor2(4);
AF_DCMotor motor1(3);

// ----------------------  encoders  ----------------------------
float minPulsesPerTimeUnit = 8;
float maxPulsesPerTimeUnit = 30;
volatile unsigned int pulsesLeft;
volatile unsigned int pulsesRight;
volatile int pulsesLeftPerTimeUnit;
volatile int pulsesRightPerTimeUnit;

// ----------------------  wifi  ----------------------------
char command;

// -------------------- timer --------------------------
long timerInterval = 500000; // 0.2 секунды


// -------------------- pi-regular --------------------------
// Диапазон сигнала управления
float minControlActionLeft = 40;
float maxControlActionLeft = 255;
float maxError = goalSpeedLeft;

float minControlActionRight = 150;
float maxControlActionRight = 255;

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

  float y2u(float y, float minControlAction, float maxControlAction) {
    float u;
    float maxY = Kp * maxError + Ki * max_integral;
    //    Serial.print("maxY: ");
    //    Serial.print(maxY, DEC);
    float minY = -maxY;
    u = (y - minY) / (maxY - minY) * (maxControlAction - minControlAction) + minControlAction;
    //    Serial.print("u: ");
    //    Serial.print(u, DEC);
    if (u > maxControlAction) u = maxControlAction;
    if (u < minControlAction) u = minControlAction;
    return u;
  }
};

PI_CONTROLLER piControllerLeft(2, 0.001, -10, 1000);
PI_CONTROLLER piControllerRight(10, 0.001, -10, 1000);

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

void loop() {

  float errorLeft = goalSpeedLeft - pulsesLeftPerTimeUnit;
  float YLeft = piControllerLeft.Eval(errorLeft);
  float ULeft = piControllerLeft.y2u(YLeft, minControlActionLeft, maxControlActionLeft);



//  motor2.setSpeed(250);
//  motor2.run(FORWARD);
// 255 59
// 150 35
  float errorRight = goalSpeedRight - pulsesRightPerTimeUnit;
  float YRight = piControllerRight.Eval(errorRight);
  float URight = piControllerRight.y2u(YRight, minControlActionRight, maxControlActionRight);

// 1деление 15 град
// 0.7 оборота в секунду = 252 град 
// 0.2 об в секунду 72 град
// 255  25
// 150 15
  motor1.setSpeed(200);
  motor1.run(FORWARD);

  //  Serial.print("Y: ");
  //  Serial.print(Y, DEC);


  Serial.print("speed. ");
  Serial.print("left: ");
  //  Serial.print(goalSpeedLeft, DEC);
  //  Serial.print(" realSpeedLeft: ");
  Serial.print(pulsesLeftPerTimeUnit, DEC);
  Serial.print("   right: ");
  Serial.println(pulsesRightPerTimeUnit, DEC);
//    Serial.print(" U: ");
//    Serial.println(U, DEC);


  //  receiveCommand();
  //  doAction(command);
  delay(50);
}

void timerInterceptor() {
  Serial.print("   tick ");
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
