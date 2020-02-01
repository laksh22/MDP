#include "DualVNH5019MotorShield.h"

DualVNH5019MotorShield md;

void setup()
{
  Serial.begin(115200);
  Serial.println("Dual VNH5019 Motor Shield");
  md.init();
}

void loop()
{
  // put your main code here, to run repeatedly:
  for (int i = -400; i < 400; i++)
  {
    setLeftSpeed(i);
    setRightSpeed(i);

    stopIfFault();
    if (i % 100 == 0)
    {
      Serial.print("M1 current: ");
      Serial.print(md.getM1CurrentMilliamps());
      Serial.print(", M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }
}

void setLeftSpeed(int speed)
{
  md.setM1Speed(speed);
}

void setRightSpeed(int speed)
{
  md.setM2Speed(speed);
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
