#include <DualVNH5019MotorShield.h>
#include <ArduinoSort.h>

// Movement variables
#define SPEED 400
#define REVERSE -400
#define BRAKE 400

// Sensor pins
#define LSPIN A0 // PS1
#define LLPIN A1 // PS2
#define RSPIN A2 // PS3
#define FLPIN A3 // PS4
#define FLSPIN A4 // PS5
#define FRSPIN A5 // PS6

// Buffer for sensor values
#define BUFFER 30

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
  pinMode(FLSPIN, INPUT);
  pinMode(FRSPIN, INPUT);
  
  Serial.begin(9600);
  Serial.println("Starting...");
  
  md.init();
}

void loop()
{  
  printSensors();
}

void printSensors() {
  int LSDistance = getShortIRDistance(LSPIN);
  int LLDistance = getLongIRDistance(LLPIN);
  int RSDistance = getShortIRDistance(RSPIN);
  int FLDistance = getLongIRDistance(FLPIN);
  int FLSDistance = getLongIRDistance(FLSPIN);
  int FRSDistance = getLongIRDistance(FRSPIN);
  Serial.print("LS: ");
  Serial.print(LSDistance);
  Serial.print(", LL: ");
  Serial.print(LLDistance);
  Serial.print(", RS: ");
  Serial.print(RSDistance);
  Serial.print(", FL: ");
  Serial.println(FLDistance);
  Serial.print(", FLS: ");
  Serial.println(FLSDistance);
  Serial.print(", FRS: ");
  Serial.println(FRSDistance);
}

int getLongIRDistance(int pin) {
  int sensorValues[BUFFER];
  for(int i = 0; i < BUFFER; i++){
    int sensorValue = analogRead(pin);
    float voltage= sensorValue * (5.0 / 1023.0);
    int distance = LCONST * pow(voltage , L_EXP);
    sensorValues[i] = distance;
  }
  sortArray(sensorValues, BUFFER);
  return findMedian(sensorValues, BUFFER);
}

int getShortIRDistance(int pin) {
  int sensorValues[BUFFER];
  for(int i = 0; i < BUFFER; i++){
    int sensorValue = analogRead(pin);
    float voltage= sensorValue * (5.0 / 1023.0);
    int distance = SCONST * pow(voltage , S_EXP);
    sensorValues[i] = distance;
  }
  sortArray(sensorValues, BUFFER);
  return findMedian(sensorValues, BUFFER);
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
    while (1);
  }
  if (md.getM2Fault()) {
    Serial.println("M2 fault");
    while (1);
  }
}

int findMedian(int a[], int n) { 
    sortArray(a, n); 
    if (n % 2 != 0) {return a[n/2];} 
    return (a[(n-1)/2] + a[n/2])/2.0; 
} 
