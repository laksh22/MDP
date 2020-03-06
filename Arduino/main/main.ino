#include "DualVNH5019MotorShield.h"
#include "SharpIR.h"
#include "EnableInterrupt.h"
#include "PID_v1.h"
#include "ArduinoSort.h"

// For calibrating movement
// Change to make robot go straight
#define FORWARD_RPM_LEFT 71
#define FORWARD_RPM_RIGHT 83
// Change to make robot stay in the same position while rotating
#define ROTATE_RPM_LEFT 51.582
#define ROTATE_RPM_RIGHT 83
// Change to make robot move exactly one block
#define FORWARD_TARGET_TICKS 264
// Change to make robot move exactly 90 degrees
#define LEFT_TARGET_TICKS 2
#define RIGHT_TARGET_TICKS -9

// For communication
char source = 't';

// For Sensor data
#define BUFFER 20

#define LSPIN A0  // PS1
#define LLPIN A1  // PS2
#define RSPIN A2  // PS3
#define FSPIN A3  // PS4
#define FLSPIN A4 // PS5
#define FRSPIN A5 // PS6

#define SRmodel 1080
#define LRmodel 20150

SharpIR sr1 = SharpIR(LSPIN, SRmodel);
SharpIR sr2 = SharpIR(LLPIN, LRmodel);
SharpIR sr3 = SharpIR(RSPIN, SRmodel);
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

double distance_cm;
double speed1, speed2; // In PWM

// For operation mode
bool FASTEST_PATH = false;
bool DEBUG = false;
byte delayExplore = 2.5;
byte delayFastestPath = 1;

// For PID
volatile word tick1 = 0;
volatile word tick2 = 0;
word ticks_moved = 0;
double currentTick1, currentTick2, oldTick1, oldTick2;

PID PIDControlStraight(&currentTick1, &speed1, &currentTick2, 3.5, 0, 0.75, DIRECT);
PID PIDControlLeft(&currentTick1, &speed1, &currentTick2, 3, 0, 0.5, DIRECT);
PID PIDControlRight(&currentTick1, &speed1, &currentTick2, 3, 0, 0.5, DIRECT);

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
  currentTick1 = currentTick2 = oldTick1 = oldTick2 = 0;

  // Begin communication
  Serial.begin(9600);
}

void loop()
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
    moveForward(10);
    sendAck();
    break;
  }
  case 'A':
  {
    rotateLeft(90);
    sendAck();
    break;
  }
  case 'D':
  {
    rotateRight(90);
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
    //TODO: frontAlignment();
    sendAck();
    break;
  }
  case 'L':
  {
    rotateLeft(90);
    sendAck();
    break;
  }
  case 'R':
  {
    rotateLeft(10);
    sendAck();
    break;
  }
  case 'F':
  {
    //TODO: MazeRunner_Flag = true;
    moveForward(1);
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
  Serial.print("@");
  Serial.print(source);
  Serial.println("Y!");
  Serial.flush();
}

// Send the sensor data to the RPi
void sendSensors()
{
  int LSDistance = getSensorMedian(LSPIN);
  int RSDistance = getSensorMedian(RSPIN);
  int FSDistance = getSensorMedian(FSPIN);
  int FLSDistance = getSensorMedian(FLSPIN);
  int FRSDistance = getSensorMedian(FRSPIN);
  Serial.print("@");
  Serial.print(source);
  if (LSDistance < 40)
  {
    Serial.print(leftShortToGrids(LSDistance));
    Serial.print(":");
  }
  else
  {
    int LLDistance = getSensorMedian(LLPIN);
    Serial.print(leftLongToGrids(LLDistance));
    Serial.print(":");
  }
  Serial.print(rightShortToGrids(RSDistance));
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
int rightShortToGrids(int dis)
{
  if (dis <= 19)
    return 1;

  else if (dis > 19 && dis <= 29)
    return 2;

  else
    return 3;
}

int leftShortToGrids(int dis)
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
  if (dis <= 47)
    return 4;

  else if (dis > 47 && dis <= 57)
    return 5;

  else
    return 6;
}

int frontToGrids(int dis)
{
  if (dis <= 14)
    return 1;

  else if (dis > 14 && dis <= 25)
    return 2;

  else
    return 3;
}

int frontRightToGrids(int dis)
{
  if (dis <= 17)
    return 1;

  else if (dis > 17 && dis <= 28)
    return 2;

  else
    return 3;
}

int frontLeftToGrids(int dis)
{

  if (dis <= 20)
    return 1;

  else if (dis > 20 && dis <= 32)
    return 2;

  else
    return 3;
}

// Find the median distance of a sensor
int getSensorMedian(int pin)
{
  int sensorValues[BUFFER];
  switch (pin)
  {
  case LSPIN:
    for (int i = 0; i < BUFFER; i++)
    {
      int sensorValue = sr1.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case LLPIN:
    for (int i = 0; i < BUFFER; i++)
    {
      int sensorValue = sr2.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case RSPIN:
    for (int i = 0; i < BUFFER; i++)
    {
      int sensorValue = sr3.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case FSPIN:
    for (int i = 0; i < BUFFER; i++)
    {
      int sensorValue = sr4.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case FLSPIN:
    for (int i = 0; i < BUFFER; i++)
    {
      int sensorValue = sr5.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  case FRSPIN:
    for (int i = 0; i < BUFFER; i++)
    {
      int sensorValue = sr6.distance();
      sensorValues[i] = sensorValue;
    }
    break;
  }
  return findMedian(sensorValues, BUFFER);
}

// Find the median value out of an array
int findMedian(int a[], int n)
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
  double target_tick = 4.3589 * degree + LEFT_TARGET_TICKS;
  //double target_tick = 384;

  if (FASTEST_PATH)
  {
    //target_tick = 378;
  }
  //target_tick = 4.1533*degree;
  double tick_travelled = 0;

  if (target_tick < 0)
    return;

  // Init values
  tick1 = tick2 = 0;               //encoder's ticks (constantly increased when the program is running due to interrupt)
  currentTick1 = currentTick2 = 0; //ticks that we are used to calculate PID. Ticks at the current sampling of PIDController
  oldTick1 = oldTick2 = 0;
  speed1 = rpmToSpeed1(-70);
  speed2 = rpmToSpeed2(69);
  //speed1 = -210;
  //speed2 = 210;

  md.setSpeeds(speed1, speed2);
  tick_travelled = (double)tick2;

  PIDControlLeft.SetSampleTime(15); //Controller is called every 50ms
  if (FASTEST_PATH)
  {
    PIDControlLeft.SetTunings(5, 0, 0.5);
    PIDControlLeft.SetSampleTime(15);
  }
  PIDControlLeft.SetMode(AUTOMATIC); //Controller is invoked automatically.

  while (tick_travelled < target_tick)
  {
    // if not reach destination ticks yet
    currentTick1 = tick1 - oldTick1; //calculate the ticks travelled in this sample interval of 50ms
    currentTick2 = tick2 - oldTick2;

    PIDControlLeft.Compute();
    oldTick2 += currentTick2; //update ticks
    oldTick1 += currentTick1;
    tick_travelled += currentTick2;
  }

  md.setBrakes(400, 400);
  PIDControlLeft.SetMode(MANUAL); //turn off PID
  delay(delayExplore);
  if (FASTEST_PATH)
    delay(delayFastestPath);
}

// Rotate right by given degrees. Using 360 degree as a base line
void rotateRight(double degree)
{
  double target_tick = 4.3589 * degree + RIGHT_TARGET_TICKS;
  //double target_tick = 373;

  if (FASTEST_PATH)
  {
    //target_tick = 380;
  }
  //0.2319*degree + 6.4492;
  double tick_travelled = 0;
  if (target_tick < 0)
    return;

  // Init values
  tick1 = tick2 = 0;               //encoder's ticks (constantly increased when the program is running due to interrupt)
  currentTick1 = currentTick2 = 0; //ticks that we are used to calculate PID. Ticks at the current sampling of PIDController
  oldTick1 = oldTick2 = 0;
  speed1 = rpmToSpeed1(70);
  speed2 = rpmToSpeed2(-69);

  md.setSpeeds(speed1, speed2);
  tick_travelled = (double)tick2;

  PIDControlRight.SetSampleTime(15); //Controller is called every 25ms
  if (FASTEST_PATH)
  {
    PIDControlRight.SetTunings(4, 0, 0.5);
    PIDControlRight.SetSampleTime(15); // less aggressive
  }
  PIDControlRight.SetMode(AUTOMATIC); //Controller is invoked automatically.

  while (tick_travelled < target_tick)
  {
    // if not reach destination ticks yet
    currentTick1 = tick1 - oldTick1; //calculate the ticks travelled in this sample interval of 50ms
    currentTick2 = tick2 - oldTick2;

    PIDControlRight.Compute();
    oldTick2 += currentTick2; //update ticks
    oldTick1 += currentTick1;
    tick_travelled += currentTick2;
  }

  md.setBrakes(400, 400);
  PIDControlRight.SetMode(MANUAL);

  delay(delayExplore);
  if (FASTEST_PATH)
    delay(delayFastestPath);
}

// Move robot forward by distance (in cm)
void moveForward(byte distance)
{
  //at 6.10v to 6.20v
  double rpm1, rpm2;
  double target_tick = 0;

  target_tick = FORWARD_TARGET_TICKS; //289 // EDITED
  //target_tick = 26.85 * distance + 407.53;
  double tick_travelled = 0;

  if (target_tick < 0)
    return;

  // Init values
  tick1 = tick2 = 0;               //encoder's ticks (constantly increased when the program is running due to interrupt)
  currentTick1 = currentTick2 = 0; //ticks that we are used to calculate PID. Ticks at the current sampling of PIDController
  oldTick1 = oldTick2 = 0;

  //Speed in rpm for motor 1 and 2
  if (FASTEST_PATH)
  {
    rpm1 = 100.5;
    // rpm1 = 99.8;
    rpm2 = 100;
  }
  else
  {
    rpm1 = 84.5;
    rpm2 = 84.5;
  }
  speed1 = rpmToSpeed1Forward(rpm1); //70.75 //74.9  100
  speed2 = rpmToSpeed2Forward(rpm2); //70.5 //74.5 99.5

  //Set Final ideal speed and accomodate for the ticks we used in acceleration
  md.setSpeeds(speed1, speed2);
  tick_travelled = (double)tick2;
  PIDControlStraight.SetSampleTime(6.5); //Controller is called every 25ms

  if (FASTEST_PATH)
  { //turn on PID tuning if fastest path
    PIDControlStraight.SetTunings(10, 0, 1);
    PIDControlStraight.SetSampleTime(8);
  }
  PIDControlStraight.SetMode(AUTOMATIC); //Controller is invoked automatically using default value for PID

  while (tick_travelled < target_tick)
  {
    // if not reach destination ticks yet
    currentTick1 = tick1 - oldTick1; //calculate the ticks travelled in this sample interval of 50ms
    currentTick2 = tick2 - oldTick2;

    PIDControlStraight.Compute();

    oldTick2 += currentTick2; //update ticks
    oldTick1 += currentTick1;
    tick_travelled += currentTick2;
  }

  //md.setBrakes(370,400);
  md.setBrakes(350, 350);
  PIDControlStraight.SetMode(MANUAL);
  delay(delayExplore);
  if (FASTEST_PATH)
    delay(delayFastestPath);
}

// Increase ticks (left motor)
void E1Pos()
{
  tick1++;
}

// Increase ticks (right motor)
void E2Pos()
{
  tick2++;
}

// RPM to speed conversion when going forward (left motor)
double rpmToSpeed1Forward(double RPM)
{
  if (RPM > 0)
    return 2.815 * RPM + FORWARD_RPM_LEFT;
  else
    return 0;
  //  else
  //    return -2.9117 * (-1) * RPM - 45.197;
}

// RPM to speed conversion when going forward (right motor)
double rpmToSpeed2Forward(double RPM)
{
  if (RPM > 0)
    return 2.7845 * RPM + FORWARD_RPM_RIGHT;
  else
    return 0;
  //  else
  //    return -2.8109 * (-1) * RPM - 54.221;
}

// RPM to speed conversion when rotating (left motor)
double rpmToSpeed1(double RPM)
{
  if (RPM > 0)
    return 2.8598 * RPM + ROTATE_RPM_LEFT;
  else if (RPM == 0)
    return 0;
  else
    return -2.9117 * (-1) * RPM - 45.197;
}

// RPM to speed conversion when rotating (right motor)
double rpmToSpeed2(double RPM)
{
  if (RPM > 0)
    return 2.7845 * RPM + ROTATE_RPM_RIGHT;
  else if (RPM == 0)
    return 0;
  else
    return -2.8109 * (-1) * RPM - 54.221;
}
