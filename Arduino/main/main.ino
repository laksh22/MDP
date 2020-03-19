#include "DualVNH5019MotorShield.h"
#include "EnableInterrupt.h"
#include "PID_v1.h"

// Move 1 block
#define FORWARD_TARGET_TICKS 0

// Move 90 degrees
//#define LEFT_ROTATE_DEGREES 90
//#define RIGHT_ROTATE_DEGREES 91
#define LEFT_ROTATE_DEGREES 87
#define RIGHT_ROTATE_DEGREES 87.5
#define ROTATE_LEFT_180 184.5             

//Move Forward fixed distance
#define FORWARD_DISTANCE 9.85
#define FORWARD_DISTANCE_MULTIPLE 9.6
#define MULTIPLE_FORWARD_FACTOR 4.65 / 3

//Move Forward Staight/
//#define LEFT_RPM 69
//#define RIGHT_RPM 67.5
//#define LEFT_RPM 100
//#define RIGHT_RPM 95
#define LEFT_RPM 85
#define RIGHT_RPM 78.5   
#define LEFT_RPM_MULTIPLE 100
#define RIGHT_RPM_MULTIPLE 93.5


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
byte delayExplore = 10;
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
  //printSensors(2);
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
  case 'Z':
  {
    calibrateRightAngle();
    sendAck();
    break;
  }
  case 'C':
  {
    calibrateFrontAngleLR(true);
    sendAck();
    break;
  }
  case 'T':
  {
    calibrateFrontAngleLM(true);
    sendAck();
    break;
  }
  case 'Y':
  {
    calibrateFrontAngleMR(true);
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
    moveForward(FORWARD_DISTANCE * 2 + MULTIPLE_FORWARD_FACTOR);
    sendAck();
    break;
  }
  case '3':
  {
    moveForward(FORWARD_DISTANCE * 3 + MULTIPLE_FORWARD_FACTOR * 2);
    sendAck();
    break;
  }
  case '4':
  {
    moveForwardMultiple(FORWARD_DISTANCE_MULTIPLE * 4 + MULTIPLE_FORWARD_FACTOR * 3);
    sendAck();
    break;
  }
  case '5':
  {
    moveForwardMultiple(FORWARD_DISTANCE_MULTIPLE * 5 + MULTIPLE_FORWARD_FACTOR * 4);
    sendAck();
    break;
  }
  case '6':
  {
    moveForwardMultiple(FORWARD_DISTANCE_MULTIPLE * 6 + MULTIPLE_FORWARD_FACTOR * 5);
    sendAck();
    break;
  }
  case '7':
  {
    moveForwardMultiple(FORWARD_DISTANCE_MULTIPLE * 7 + MULTIPLE_FORWARD_FACTOR * 6);
    sendAck();
    break;
  }
  case '8':
  {
    moveForwardMultiple(FORWARD_DISTANCE_MULTIPLE * 8 + MULTIPLE_FORWARD_FACTOR * 7);
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

// Send an acknowledgement after every command is executed
void sendAck()
{
  delay(50);
  Serial.print("@");
  Serial.print(source);
  Serial.println("Y!");
  Serial.flush();
}
