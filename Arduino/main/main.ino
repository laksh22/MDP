#include "DualVNH5019MotorShield.h"
#include "EnableInterrupt.h"
#include "PID_v1.h"

// Move 1 block
#define FORWARD_TARGET_TICKS 0

// Move 90 degrees
#define LEFT_ROTATE_DEGREES 90
#define RIGHT_ROTATE_DEGREES 93
#define ROTATE_LEFT_180 184.5

//Move Forward fixed distance
#define FORWARD_DISTANCE 10
#define MULTIPLE_FORWARD_FACTOR 4.1/3

//Move Forward Staight
#define LEFT_RPM 68.5
#define RIGHT_RPM 65

// For communication
char source = 't';

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
  sensorInit();
  
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
    sendSensors(source);
    break;
  }
  case 'C':
  {
    calibrateFrontAngleLR(true);
    sendAck();
    break;
  }
  case 'L':
  {
    calibrateDistanceL(-1, false);
    sendAck();
    break;
  }
  case 'M':
  {
    calibrateDistanceM(-1, false);
    sendAck();
    break;
  }
  case 'R':
  {
    calibrateDistanceR(-1, false);
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

  speedL = rpmTospeedL(LEFT_RPM); //70.75 //74.9  100
  speedR = rpmTospeedR(RIGHT_RPM); //70.5 //74.5 99.5

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
  double target_tick = 0;
  double rpmL, rpmR;

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
