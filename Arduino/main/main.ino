#include <DualVNH5019MotorShield.h>
#include <SharpIR.h>

#define SPEED 400
#define REVERSE -400
#define BRAKE 400

#define SRANGE A0 // PS1
#define LRANGE A1 // PS2
#define SMODEL 1080 // Short range sensor
#define LMODEL 20150 // Long range sensor

// Initialise motor shield
DualVNH5019MotorShield md; // M1 = left, M2 = right

// Initialise IR sensors
SharpIR sensorSR(SRANGE, SMODEL);  
SharpIR sensorLR(LRANGE, LMODEL);

void setup()
{  
  Serial.begin(9600);
  Serial.println("Starting...");
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

  int shortDistance = getIRDistance(sensorSR);
  int longDistance = getIRDistance(sensorLR);
  Serial.print("Short Range IR: ");
  Serial.print(shortDistance);
  Serial.print(", Long Range IR: ");
  Serial.println(longDistance);
}

int getIRDistance(SharpIR sensor) {
  return sensor.distance();
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
  md.setBrakes(BRAKE, BRAKE);
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
