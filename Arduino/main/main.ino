#include "DualVNH5019MotorShield.h"

#define SPEED 400
#define REVERSE -400
#define BRAKE 400

DualVNH5019MotorShield md; // M1 = left, M2 = right

void setup()
{
  Serial.begin(115200);
  Serial.println("Dual VNH5019 Motor Shield");
  md.init();
}

void loop()
{
  moveFront();
  delay(1000);
  stop();

  moveBack();
  delay(1000);
  stop();

  turnLeft();
  delay(1000);
  stop();

  turnRight();
  delay(1000);
  stop();
}

void moveFront()
{
  md.setSpeeds(SPEED, SPEED);
  stopIfFault();
}

void moveBack()
{
  md.setSpeeds(REVERSE, REVERSE);
  stopIfFault();
}

void turnLeft()
{
  md.setSpeeds(REVERSE, SPEED);
  stopIfFault();
}

void turnRight()
{
  md.setSpeeds(SPEED, REVERSE);
  stopIfFault();
}

void stop()
{
  md.setBrakes(BRAKE, BRAKE)
}

void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    while (1)
      ;
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    while (1)
      ;
  }
}