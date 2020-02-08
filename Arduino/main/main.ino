#include <DualVNH5019MotorShield.h>

// Movement variables
#define SPEED 400
#define REVERSE -400
#define BRAKE 400

// Sensor pins
#define LSPIN A0 // PS1
#define LLPIN A1 // PS2
#define RSPIN A1 // PS3
#define FLPIN A1 // PS4

// Sensor calculation parameters
#define LCONST 60.374
#define SCONST 29.988
#define L_EXP -1.16
#define S_EXP -1.173

// Motor shield | M1 = left, M2 = right
DualVNH5019MotorShield md;

void setup()
{  
  pinMode(LSPIN, INPUT);
  pinMode(LLPIN, INPUT);
  pinMode(RSPIN, INPUT);
  pinMode(FLPIN, INPUT);
  
  Serial.begin(9600);
  Serial.println("Starting...");
  
  md.init();
}

void loop()
{
  
  int LSDistance = getLongIRDistance(LSPIN);
  int LLDistance = getLongIRDistance(LLPIN);
  int RSDistance = getShortIRDistance(RSPIN);
  int FLDistance = getLongIRDistance(FLPIN);
  
  delay(500);
}

int getLongIRDistance(int pin) {
  int sensorValue = analogRead(pin);
  float voltage= sensorValue * (5.0 / 1023.0);
  return (int)(LCONST * pow(voltage , L_EXP));
}

int getShortIRDistance(int pin) {
  int sensorValue = analogRead(pin);
  float voltage= sensorValue * (5.0 / 1023.0);
  return (int)(SCONST * pow(voltage , S_EXP));
}

void moveFront(){
  md.setSpeeds(SPEED, SPEED);
  stopIfFault();
}

void moveBack(){
  md.setSpeeds(REVERSE, REVERSE);
  stopIfFault();
}

void turnLeft(){
  md.setSpeeds(REVERSE, SPEED);
  stopIfFault();
}

void turnRight(){
  md.setSpeeds(SPEED, REVERSE);
  stopIfFault();
}

void stopMotors(){
  md.setBrakes(BRAKE, BRAKE);
}

void stopIfFault() {
  if (md.getM1Fault()) {
    Serial.println("M1 fault");
    while (1)
      ;
  }
  
  if (md.getM2Fault()) {
    Serial.println("M2 fault");
    while (1)
      ;
  }
}
