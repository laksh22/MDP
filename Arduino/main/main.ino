#include <DualVNH5019MotorShield.h>
//#include <SharpIR.h>

#define SPEED 400
#define REVERSE -400
#define BRAKE 400

#define SPIN A0 // PS1
#define LPIN A1 // PS2
///#define SMODEL 1080 // Short range sensor
//#define LMODEL 20150 // Long range sensor

// Initialise IR sensors
//SharpIR sensorSR(SPIN, /*SharpIR::GP2Y0A21YK0F*/ SMODEL);  
//SharpIR sensorLR(LPIN, /*SharpIR::GP2Y0A02YK0F*/ LMODEL);

// Initialise motor shield
DualVNH5019MotorShield md; // M1 = left, M2 = right

void setup()
{  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  Serial.begin(9600);
  Serial.println("Starting...");
  md.init();
}

void loop()
{
//  moveFront();
//  delay(1000);
//  stop();

//  moveBack();
//  delay(1000);
//  stop();
//
//  turnLeft();
//  delay(1000);
//  stop();
//
//  turnRight();
//  delay(1000);
//  stopMotors();

  delay(500);
  
  int longDistance = getLongIRDistance(LPIN);
  int shortDistance = getShortIRDistance(SPIN);
  
  Serial.print("Short Range IR: ");
  Serial.print(shortDistance);
  Serial.print(", Long Range IR: ");
  Serial.println(longDistance);
}

int getLongIRDistance(int pin) {
  int sensorValue = analogRead(pin);
  float voltage= sensorValue * (5.0 / 1023.0);
  return (int)(60.374 * pow(voltage , -1.16));
}

int getShortIRDistance(int pin) {
  int sensorValue = analogRead(pin);
  float voltage= sensorValue * (5.0 / 1023.0);
  return (int)(29.988 * pow(voltage , -1.173));
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

void stopMotors()
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
