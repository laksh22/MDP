#include "DualVNH5019MotorShield.h"

DualVNH5019MotorShield md; // M1 = left, M2 = right

void setup()
{
  Serial.begin(115200);
  Serial.println("Dual VNH5019 Motor Shield");
  md.init();
}

void loop()
{
  moveFront(200);
  stopIfFault();
  delay(1000);
  md.setBrakes(400, 400);

  moveBack(200);
  stopIfFault();
  delay(1000);
  md.setBrakes(400, 400);

  turnLeft(200);
  stopIfFault();
  delay(1000);
  md.setBrakes(400, 400);

  turnRight(200);
  stopIfFault();
  delay(1000);
  md.setBrakes(400, 400);
}

void moveFront(int distance)
{
  md.setSpeeds(distance, distance);
}

void moveBack(int distance)
{
  distance = 0-distance;
  md.setSpeeds(distance, distance);
}

void turnLeft(int angle)
{
  int leftAngle = 0 - angle;
  md.setSpeeds(leftAngle, angle);
}

void turnRight(int angle)
{
  int rightAngle = 0 - angle;
  md.setSpeeds(angle, rightAngle);
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