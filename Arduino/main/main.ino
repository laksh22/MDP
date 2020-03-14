#include "DualVNH5019MotorShield.h"
#include "EnableInterrupt.h"
#include "PID_v1.h"

// Move 1 block
#define FORWARD_TARGET_TICKS 0

// Move 90 degrees
#define LEFT_ROTATE_DEGREES 90
#define RIGHT_ROTATE_DEGREES 93
#define ROTATE_LEFT_180 184.5

//Move Forward
#define FORWARD_DISTANCE 10

// For communication
char source = 't';

// For sensors
#define E_BUFFER 10
#define CALIBRATE_BUFFER 5

// For motors
DualVNH5019MotorShield md;

// For speed data
byte encoder1A = 3;
byte encoder1B = 5;
byte encoder2A = 11;
byte encoder2B = 13;

double speed1, speed2; // In PWM

// For operation mode
bool FASTEST_PATH = false;
bool DEBUG = true;
byte delayExplore = 2.5;
byte delayFastestPath = 1;

// For PID
volatile word tick1 = 0;
volatile word tick2 = 0;
word ticks_moved = 0;
double currentTick1, currentTick2, oldTick1, oldTick2;
double a = 0;

//PID PIDControlStraight(&currentTick1, &speed1, &currentTick2, 3.5, 0, 0.75, DIRECT);
//PID PIDControlLeft(&currentTick1, &speed1, &currentTick2, 3, 0, 0.5, DIRECT);
//PID PIDControlRight(&currentTick1, &speed1, &currentTick2, 3, 0, 0.5, DIRECT);

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
  currentTick1 = currentTick2 = oldTick1 = oldTick2 = 0;

  // Begin communication
  Serial.begin(9600);
}

void loop()
{
  // printSensors(true, CALIBRATE_BUFFER);

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
    sendSensors(source, E_BUFFER);
    break;
  }
  case 'C':
  {
    frontAlignment();
    sendAck();
    break;
  }
  case 'L':
  {
    calibrateDistanceLeft();
    sendAck();
    break;
  }
  case 'M':
  {
    calibrateDistanceMiddle();
    sendAck();
    break;
  }
  case 'R':
  {
    calibrateDistanceRight();
    sendAck();
    break;
  }
  case 'O':
  {
    rotateLeft(ROTATE_LEFT_180);
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

// ===========================================================================
// ============================MOVEMENT SECTION===============================
// ===========================================================================
// Rotate left by given degrees. Using 360 degree as a base line
void rotateLeft(double degree)
{
  double target_tick = 4.3589 * degree;
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
  speed1 = rpmToSpeed1(-66.25);
  speed2 = rpmToSpeed2(64.9);
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
  double target_tick = 4.3589 * degree;
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
  speed1 = rpmToSpeed1(66.25);
  speed2 = rpmToSpeed2(-64.9);

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
void moveForward(float distance)
{
  //at 6.10v to 6.20v
  double rpm1, rpm2;
  double target_tick = 0;

  //target_tick = FORWARD_TARGET_TICKS; //289 // EDITED
  target_tick = 26.85 * distance + FORWARD_TARGET_TICKS;
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
    rpm1 = 101;
    // rpm1 = 99.8;
    rpm2 = 100;
  }
  else
  {
    rpm1 = 68.5;
    rpm2 = 65.1;
  }
  speed1 = rpmToSpeed1(rpm1); //70.75 //74.9  100
  speed2 = rpmToSpeed2(rpm2); //70.5 //74.5 99.5

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

// Move robot backward by distance (in cm)
void moveBackward(float distance)
{
  //at 6.10v to 6.20v
  double rpm1, rpm2;
  double target_tick = 0;

  //target_tick = FORWARD_TARGET_TICKS; //289 // EDITED
  target_tick = 26.85 * distance + FORWARD_TARGET_TICKS;
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
    rpm1 = -64.9;
    rpm2 = -64.9;
  }
  speed1 = rpmToSpeed1(rpm1); //70.75 //74.9  100
  speed2 = rpmToSpeed2(rpm2); //70.5 //74.5 99.5

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
  md.setBrakes(250, 250);
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

// RPM to speed conversion (left motor)
double rpmToSpeed1(double RPM)
{
  if (RPM == 0)
    return 0;
  else
    return 3.6980 * RPM;
}

// RPM to speed conversion (right motor)
double rpmToSpeed2(double RPM)
{
  if (RPM == 0)
    return 0;
  else
    return 4.0999 * RPM;
}

// ===========================================================================
// ============================MOVEMENT SECTION===============================
// ===========================================================================

void frontAlignment()
{
  calibrateFrontAngle();
  delay(100);
}

void calibrateFrontAngle()
{
  int offset = 2; // TODO

  int count = 0;

  while (1)
  {
    float LFreading = getSensorFL(CALIBRATE_BUFFER);
    float RFreading = getSensorFR(CALIBRATE_BUFFER);
    float error = RFreading - (LFreading - offset); //Higher to the left, Lower to the right

    //Serial.println(error);

    if (error > 1.40) // TODO
    //rotateLeft(0.25);
    {
      //Serial.println("LEFT");
      md.setSpeeds(-150, 150);
      if (error > 2.35)
      {
        delay(30 * abs(error));
      }
      else
      {
        delay(12);
      }
      //delay(abs(error * 20 / 3));
    }
    else if (error < 1.30)
    //rotateRight(0.25);
    {
      //Serial.println("RIGHT");
      md.setSpeeds(150, -150);
      if (error < -0.12)
      {
        delay(30 * abs(error));
      }
      else
      {
        delay(12);
      }
      //delay(abs(error * 20 / 3));
    }
    else
    {
      md.setBrakes(400, 400);
      break;
    }

    md.setBrakes(100, 100);

    count++;
    if (count > 10)
    {
      break;
    }
  }

  float LFdistance = getSensorFL(CALIBRATE_BUFFER);

  if (LFdistance > 15 || LFdistance < 15) // TODO, should be fine
    calibrateFrontDistance();
}

void calibrateFrontDistance()
{
  int count = 0;

  while (1)
  {

    float LFdistance = getSensorFL(CALIBRATE_BUFFER);

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
    if (count > 25)
    {
      break;
    }
  }

  //Recursive call if angle is misaligned after distance alignment.
  float angleError = getSensorFL(CALIBRATE_BUFFER) - getSensorFR(CALIBRATE_BUFFER);
  if (angleError > 3 || angleError < -3)
    calibrateFrontAngle();
}

void calibrateDistanceLeft()
{
  while (1)
  {

    float LFdistance = getSensorFL(CALIBRATE_BUFFER);

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

void calibrateDistanceMiddle()
{
  while (1)
  {

    float Fdistance = getSensorF(CALIBRATE_BUFFER);

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

void calibrateDistanceRight()
{
  while (1)
  {

    float RFdistance = getSensorFR(CALIBRATE_BUFFER);

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
