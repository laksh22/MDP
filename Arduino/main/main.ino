#include "DualVNH5019MotorShield.h"
#include "SharpIR.h"
#include "EnableInterrupt.h"
#include "PID_v1.h"
#include "ArduinoSort.h"

// Move 1 block
#define FORWARD_TARGET_TICKS 0

// Move 90 degrees
#define LEFT_ROTATE_DEGREES 90
#define RIGHT_ROTATE_DEGREES 93
#define ROTATE_LEFT_180 184.5

//Move Forward
#define FORWARD_DISTANCE 10
#define MULTIPLE_FORWARD_FACTOR 4.1/3

// For communication
char source = 't';

// For Sensor data
#define BUFFER 8
float getSensorMedian(int, int = BUFFER);

#define RBPIN A0  // PS1
#define LLPIN A1  // PS2
#define RFPIN A2  // PS3
#define FSPIN A3  // PS4
#define FLSPIN A4 // PS5
#define FRSPIN A5 // PS6

#define SRmodel 1080
#define LRmodel 20150

SharpIR sr1 = SharpIR(RBPIN, SRmodel);
SharpIR sr2 = SharpIR(LLPIN, LRmodel);
SharpIR sr3 = SharpIR(RFPIN, SRmodel);
SharpIR sr4 = SharpIR(FSPIN, SRmodel);
SharpIR sr5 = SharpIR(FLSPIN, SRmodel);
SharpIR sr6 = SharpIR(FRSPIN, SRmodel);

// For motors
DualVNH5019MotorShield md;

// For speed data
byte encoder1A = 3;
byte encoder1B = 5;
byte encoder2A = 11;
byte encoder2B = 13;

double speedL, speedR; // In PWM

// For operation mode
bool FASTEST_PATH = false;
bool DEBUG = true;
byte delayExplore = 2.5;
byte delayFastestPath = 1;

// For PID
volatile word ticksL = 0;
volatile word ticksR = 0;
word ticks_moved = 0;
double currentTicksL, currentTicksR, oldticksL, oldticksR;
double a = 0;

//PID PIDControlStraight(&currentTicksL, &speedL, &currentTicksR, 3.5, 0, 0.75, DIRECT);
//PID PIDControlLeft(&currentTicksL, &speedL, &currentTicksR, 3, 0, 0.5, DIRECT);
//PID PIDControlRight(&currentTicksL, &speedL, &currentTicksR, 3, 0, 0.5, DIRECT);

PID PIDControlStraight(&a, &a, &a, a, a, a, DIRECT);
PID PIDControlLeft(&a, &a, &a, a, a, a, DIRECT);
PID PIDControlRight(&a, &a, &a, a, a, a, DIRECT);

/*
 * ==============================
 * Main Program
 * ==============================
 */
void setup()
{
  // Initialise the motor
  md.init();

  //Attach interrupts to counts ticks
  pinMode(encoder1A, INPUT);
  pinMode(encoder2A, INPUT);
  enableInterrupt(encoder1A, E1Pos, RISING);
  enableInterrupt(encoder2A, E2Pos, RISING);

  // Init values
  currentTicksL = currentTicksR = oldticksL = oldticksR = 0;

  // Begin communication
  Serial.begin(9600);
}

void loop()
{
  //Serial.println(sr5.distance() - sr6.distance());
//    float offset = 2.0;
//    float LFreading = getSensorMedian(FLSPIN, 15);
//    float RFreading = getSensorMedian(FRSPIN, 15);
//    float error = RFreading - (LFreading - offset); //Higher to the left, Lower to the right
//    Serial.println(error);
//  Serial.print(getSensorMedian(FLSPIN));
//  Serial.print(" : ");
//  Serial.print(getSensorMedian(FSPIN));
//  Serial.print(" : ");
//  Serial.println(getSensorMedian(FRSPIN));

  runCommands();
}

void runCommands()
{      
  Serial.flush();
  char command_buffer[10];
  int i = 0;
  char newChar;

  while (!Serial.available())
  {
  }
  while (1)
  {
    if (Serial.available())
    {
      newChar = Serial.read();
      command_buffer[i] = newChar;
      i++;
      if (newChar == '|')
      {
        break;
      }
    }
  }

  char command;

  //First character is the source
  source = command_buffer[0];
  //Second character in array is the command
  command = command_buffer[1];

  /*---------------------------------------------------------------------------------------------------
                                          Input Commands
                                          --------------
  LEGEND:
  -------
  W ---> Move Forward
  A ---> Rotate Left
  D ---> Rotate Right
  E ---> Read Sensor Values
  C ---> Recalibrate Robot's Center
  T ---> Avoiding Obstacle In A Straight Line
  L ---> Gradual Left Turn
  R ---> Gradual Right Turn
  ---------------------------------------------------------------------------------------------------*/
  switch (command)
  {
  case 'W':
  {
    moveForward(FORWARD_DISTANCE);
    sendAck();
    break;
  }
  case 'A':
  {
    rotateLeft(LEFT_ROTATE_DEGREES);
    sendAck();
    break;
  }
  case 'D':
  {
    rotateRight(RIGHT_ROTATE_DEGREES);
    sendAck(); 
    break;
  }
  case 'E':
  {
    sendSensors();
    break;
  }
  case 'C':
  {
    harveyAlignment();
    sendAck();
    break;
  }
  case 'L':
  {
    harveyCalibrateDistanceLeft();
    sendAck();
    break;
  }
  case 'M':
  {
    harveyCalibrateDistanceMiddle();
    sendAck();
    break;
  }
  case 'R':
  {
    harveyCalibrateDistanceRight();
    sendAck();
    break;
  }
  case 'O':
  {
    rotateLeft(ROTATE_LEFT_180);
    sendAck();
    break;
  }
  case '1':
  {
    moveForward(FORWARD_DISTANCE);
    sendAck();
    break;
  }
  case '2':
  {
    moveForward(FORWARD_DISTANCE*2 + MULTIPLE_FORWARD_FACTOR);
    sendAck();
    break;
  }
  case '3':
  {
    moveForward(FORWARD_DISTANCE*3 + MULTIPLE_FORWARD_FACTOR*2);
    sendAck();
    break;
  }
  case '4':
  {
    moveForward(FORWARD_DISTANCE*4 + MULTIPLE_FORWARD_FACTOR*3);
    sendAck();
    break;
  }
  case '5':
  {
    moveForward(FORWARD_DISTANCE*5 + MULTIPLE_FORWARD_FACTOR*4);
    sendAck();
    break;
  }
  case '6':
  {
    moveForward(FORWARD_DISTANCE*6 + MULTIPLE_FORWARD_FACTOR*5);
    sendAck();
    break;
  }
  case '7':
  {
    moveForward(FORWARD_DISTANCE*7 + MULTIPLE_FORWARD_FACTOR*6);
    sendAck();
    break;
  }
  case '8':
  {
    moveForward(FORWARD_DISTANCE*8 + MULTIPLE_FORWARD_FACTOR*7);
    sendAck();
    break;
  }

  default:
  {
    sendAck();
    break;
  }
  }
  memset(command_buffer, 0, sizeof(command_buffer));
}

// ===========================================================================
// =========================COMMUNICATION SECTION=============================
// ===========================================================================
// Send an acknowledgement after every command is executed
void sendAck()
{
  delay(50);
  Serial.print("@");
  Serial.print(source);
  Serial.println("Y!");
  Serial.flush();
}

// Send the sensor data to the RPi
void sendSensors()
{
  double RBDistance = getSensorMedian(RBPIN);
  double RFDistance = getSensorMedian(RFPIN);
  double FSDistance = getSensorMedian(FSPIN);
  double FLSDistance = getSensorMedian(FLSPIN);
  double FRSDistance = getSensorMedian(FRSPIN);
  double LLDistance = getSensorMedian(LLPIN);

  Serial.print("@");
  Serial.print(source);
  Serial.print(leftLongToGrids(LLDistance));
  Serial.print(":");
  Serial.print(rightFrontToGrids(RFDistance));
  Serial.print(":");
  Serial.print(frontToGrids(FSDistance));
  Serial.print(":");
  Serial.print(frontLeftToGrids(FLSDistance));
  Serial.print(":");
  Serial.print(frontRightToGrids(FRSDistance));
  Serial.println("!");
  Serial.flush();
}

// ===========================================================================
// ============================SENSOR SECTION=================================
// ===========================================================================
int rightFrontToGrids(int dis)
{
  if (dis <= 19)
    return 1;

  else if (dis > 19 && dis <= 29)
    return 2;

  else
    return 3;
}

int rightBackToGrids(int dis)
{
  if (dis <= 21)
    return 1;

  else if (dis > 21 && dis <= 33)
    return 2;

  else
    return 3;
}

int leftLongToGrids(int dis)
{
  if (dis <= 19)
    return 1;

  else if (dis > 19 && dis <= 25)
    return 2;

  else if (dis > 25 && dis <= 34)
    return 3;

  else if (dis > 34 && dis <= 44)
    return 4;

  else if (dis > 44 && dis <= 57)
    return 5;

  else
    return 6;
}

int frontToGrids(int dis)
{
  if (dis <= 15)
    return 1;

  else if (dis > 15 && dis <= 26)
    return 2;

  else
    return 3;
}

int frontRightToGrids(int dis)
{
  if (dis <= 18.30)
    return 1;

  else if (dis > 18.30 && dis <= 32)
    return 2;

  else
    return 3;
}

int frontLeftToGrids(int dis)
{

  if (dis <= 18.38)
    return 1;

  else if (dis > 18.38 && dis <= 30.94)
    return 2;

  else
    return 3;
}

// Find the median distance of a sensor
float getSensorMedian(int pin, int bufferSize)
{
  float sensorValues[bufferSize];
  switch (pin)
  {
  case RBPIN:
    for (int i = 0; i < bufferSize; i++)
    {
      float sensorValue = sr1.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case LLPIN:
    for (int i = 0; i < bufferSize; i++)
    {
      float sensorValue = sr2.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case RFPIN:
    for (int i = 0; i < bufferSize; i++)
    {
      float sensorValue = sr3.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case FSPIN:
    for (int i = 0; i < bufferSize; i++)
    {
      float sensorValue = sr4.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case FLSPIN:
    for (int i = 0; i < bufferSize; i++)
    {
      float sensorValue = sr5.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case FRSPIN:
    for (int i = 0; i < bufferSize; i++)
    {
      float sensorValue = sr6.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  }
  return findMedian(sensorValues, bufferSize);
}

// Find the median value out of an array
float findMedian(float a[], int n)
{
  sortArray(a, n);
  if (n % 2 != 0)
  {
    return a[n / 2];
  }
  return (a[(n - 1) / 2] + a[n / 2]) / 2.0;
}

// ===========================================================================
// ============================MOVEMENT SECTION===============================
// ===========================================================================
// Rotate left by given degrees. Using 360 degree as a base line
void rotateLeft(double degree)
{
  double target_tick = 4.3589 * degree;
  //double target_tick = 384;
  double tick_travelled = 0;

  if (target_tick < 0)
    return;

  // Init values
  ticksL = ticksR = 0;               //encoder's ticks (constantly increased when the program is running due to interrupt)
  currentTicksL = currentTicksR = 0; //ticks that we are used to calculate PID. Ticks at the current sampling of PIDController
  oldticksL = oldticksR = 0;
  speedL = rpmTospeedL(-66.25);
  speedR = rpmTospeedR(64.9);

  md.setSpeeds(speedL, speedR);
  tick_travelled = (double)ticksR;

  PIDControlLeft.SetSampleTime(15); //Controller is called every 50ms
  PIDControlLeft.SetMode(AUTOMATIC); //Controller is invoked automatically.

  while (tick_travelled < target_tick)
  {
    // if not reach destination ticks yet
    currentTicksL = ticksL - oldticksL; //calculate the ticks travelled in this sample interval of 50ms
    currentTicksR = ticksR - oldticksR;

    PIDControlLeft.Compute();
    oldticksR += currentTicksR; //update ticks
    oldticksL += currentTicksL;
    tick_travelled += currentTicksR;
  }

  md.setBrakes(400, 400);
  PIDControlLeft.SetMode(MANUAL); //turn off PID
  delay(delayExplore);
}

// Rotate right by given degrees. Using 360 degree as a base line
void rotateRight(double degree)
{
  double target_tick = 4.3589 * degree;
  //double target_tick = 373;
  double tick_travelled = 0;
  
  if (target_tick < 0)
    return;

  // Init values
  ticksL = ticksR = 0;               //encoder's ticks (constantly increased when the program is running due to interrupt)
  currentTicksL = currentTicksR = 0; //ticks that we are used to calculate PID. Ticks at the current sampling of PIDController
  oldticksL = oldticksR = 0;
  speedL = rpmTospeedL(66.25);
  speedR = rpmTospeedR(-64.9);

  md.setSpeeds(speedL, speedR);
  tick_travelled = (double)ticksR;

  PIDControlRight.SetSampleTime(15); //Controller is called every 25ms
  PIDControlRight.SetMode(AUTOMATIC); //Controller is invoked automatically.

  while (tick_travelled < target_tick)
  {
    // if not reach destination ticks yet
    currentTicksL = ticksL - oldticksL; //calculate the ticks travelled in this sample interval of 50ms
    currentTicksR = ticksR - oldticksR;

    PIDControlRight.Compute();
    oldticksR += currentTicksR; //update ticks
    oldticksL += currentTicksL;
    tick_travelled += currentTicksR;
  }

  md.setBrakes(400, 400);
  PIDControlRight.SetMode(MANUAL);

  delay(delayExplore);
}

// Move robot forward by distance (in cm)
void moveForward(float distance)
{
  //at 6.10v to 6.20v
  double rpmL, rpmR;
  double target_tick = 0;

  //target_tick = FORWARD_TARGET_TICKS; //289 // EDITED
  target_tick = 26.85 * distance + FORWARD_TARGET_TICKS;
  double tick_travelled = 0;

  if (target_tick < 0)
    return;

  // Init values
  ticksL = ticksR = 0;               //encoder's ticks (constantly increased when the program is running due to interrupt)
  currentTicksL = currentTicksR = 0; //ticks that we are used to calculate PID. Ticks at the current sampling of PIDController
  oldticksL = oldticksR = 0;

  //Speed in rpm for motor 1 and 2
  rpmL = 68.5;
  rpmR = 65;
  
  speedL = rpmTospeedL(rpmL); //70.75 //74.9  100
  speedR = rpmTospeedR(rpmR); //70.5 //74.5 99.5

  //Set Final ideal speed and accomodate for the ticks we used in acceleration
  md.setSpeeds(speedL, speedR);
  tick_travelled = (double)ticksR;
  
  PIDControlStraight.SetSampleTime(6.5); //Controller is called every 25ms
  PIDControlStraight.SetMode(AUTOMATIC); //Controller is invoked automatically using default value for PID

  while (tick_travelled < target_tick)
  {
    // if not reach destination ticks yet
    currentTicksL = ticksL - oldticksL; //calculate the ticks travelled in this sample interval of 50ms
    currentTicksR = ticksR - oldticksR;

    PIDControlStraight.Compute();

    oldticksR += currentTicksR; //update ticks
    oldticksL += currentTicksL;
    tick_travelled += currentTicksR;
  }

  //md.setBrakes(370,400);
  md.setBrakes(350, 350);
  PIDControlStraight.SetMode(MANUAL);
  delay(delayExplore);
}

// Move robot backward by distance (in cm)
void moveBackward(float distance)
{
  //at 6.10v to 6.20v
  double rpmL, rpmR;
  double target_tick = 0;

  //target_tick = FORWARD_TARGET_TICKS; //289 // EDITED
  target_tick = 26.85 * distance + FORWARD_TARGET_TICKS;
  double tick_travelled = 0;

  if (target_tick < 0)
    return;

  // Init values
  ticksL = ticksR = 0;               //encoder's ticks (constantly increased when the program is running due to interrupt)
  currentTicksL = currentTicksR = 0; //ticks that we are used to calculate PID. Ticks at the current sampling of PIDController
  oldticksL = oldticksR = 0;

  //Speed in rpm for motor 1 and 2
  rpmL = -64.9;
  rpmR = -64.9;
  
  speedL = rpmTospeedL(rpmL); //70.75 //74.9  100
  speedR = rpmTospeedR(rpmR); //70.5 //74.5 99.5

  //Set Final ideal speed and accomodate for the ticks we used in acceleration
  md.setSpeeds(speedL, speedR);
  tick_travelled = (double)ticksR;
  
  PIDControlStraight.SetSampleTime(6.5); //Controller is called every 25ms
  PIDControlStraight.SetMode(AUTOMATIC); //Controller is invoked automatically using default value for PID

  while (tick_travelled < target_tick)
  {
    // if not reach destination ticks yet
    currentTicksL = ticksL - oldticksL; //calculate the ticks travelled in this sample interval of 50ms
    currentTicksR = ticksR - oldticksR;

    PIDControlStraight.Compute();

    oldticksR += currentTicksR; //update ticks
    oldticksL += currentTicksL;
    tick_travelled += currentTicksR;
  }

  //md.setBrakes(370,400);
  md.setBrakes(250, 250);
  PIDControlStraight.SetMode(MANUAL);
  delay(delayExplore);
}

// Increase ticks (left motor)
void E1Pos()
{
  ticksL++;
}

// Increase ticks (right motor)
void E2Pos()
{
  ticksR++;
}

// RPM to speed conversion (left motor)
double rpmTospeedL(double RPM)
{
  if (RPM == 0)
    return 0;
  else
    return 3.6980 * RPM;
}

// RPM to speed conversion (right motor)
double rpmTospeedR(double RPM)
{
  if (RPM == 0)
    return 0;
  else
    return 4.0999 * RPM;
}

// ===========================================================================
// ============================MOVEMENT SECTION===============================
// ===========================================================================

/*
/ https://github.com/HarveyLeo/cz3004-mdp-grp2/blob/master/Arduino/Arduino%20Workspace/MazerunnerWeek12_12V/MazerunnerWeek12_12V.ino
*/

void harveyAlignment()
{
  harveyCalibrateAngle();
  delay(100);
}

void harveyCalibrateAngle()
{
  int offset = 2; // TODO

  int count = 0;
  
  while (1)
  {
    float LFreading = getSensorMedian(FLSPIN, 15);
    float RFreading = getSensorMedian(FRSPIN, 15);
    float error = RFreading - (LFreading - offset); //Higher to the left, Lower to the right

    //Serial.println(error);

    if (error > 1.40) // TODO
      //rotateLeft(0.25);
      {
        //Serial.println("LEFT");
      md.setSpeeds(-150, 150);
      if(error > 2.35) {
        delay(30 * abs(error));
      } else {
        delay(12);
      }
      //delay(abs(error * 20 / 3));
      }
    else if (error < 1.30)
      //rotateRight(0.25);
     { 
      //Serial.println("RIGHT");
      md.setSpeeds(150, -150);
      if(error < -0.12) {
        delay(30 * abs(error));
      } else {
        delay(12);
      }
      //delay(abs(error * 20 / 3));
     }
    else
    {
      md.setBrakes(400,400);
      break;
    }

    md.setBrakes(100,100);

    count++;
    if(count > 10) {
      break;
    }
  }
  
  float LFdistance = getSensorMedian(FLSPIN);
  
  if (LFdistance > 15 || LFdistance < 15) // TODO, should be fine
    harveyCalibrateDistance();
}

void harveyCalibrateDistance()
{
  int count = 0;
  
  while (1)
  {

    float LFdistance = getSensorMedian(FLSPIN, 5);

//    if(LFdistance > 23) {
//      return;
//    }

    if (LFdistance > 13.6)
      moveForward(0.005);
    else if (LFdistance < 13.4)
      moveBackward(0.005);
    else
      break;

    count++;
    if(count > 25) {
      break;
    }
  }

  //Recursive call if angle is misaligned after distance alignment.
  float angleError = getSensorMedian(FLSPIN) - getSensorMedian(FRSPIN);
  if (angleError > 3 || angleError < -3)
    harveyAlignment();
}

void harveyCalibrateDistanceLeft()
{
  while (1)
  {

    float LFdistance = getSensorMedian(FLSPIN);

//    if(LFdistance > 23) {
//      return;
//    }

    if (LFdistance > 13.6)
      moveForward(0.01);
    else if (LFdistance < 13.4)
      moveBackward(0.01);
    else
      break;
  }
}

void harveyCalibrateDistanceMiddle()
{
  while (1)
  {

    float Fdistance = getSensorMedian(FSPIN);

//    if(Fdistance > 17) {
//      return;
//    }

    if (Fdistance > 10.5)
      moveForward(0.01);
    else if (Fdistance < 10.3)
      moveBackward(0.01);
    else
      break;
  }
}

void harveyCalibrateDistanceRight()
{
  while (1)
  {

    float RFdistance = getSensorMedian(FRSPIN);

//    if(RFdistance > 21) {
//      return;
//    }

    if (RFdistance > 12.9)
      moveForward(0.01);
    else if (RFdistance < 12.7)
      moveBackward(0.01);
    else
      break;
  }
}
