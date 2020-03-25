#include "DualVNH5019MotorShield.h"
#include "EnableInterrupt.h"
#include "PID_v1.h"

// Move 1 block
#define FORWARD_TARGET_TICKS 0

// Move 90 degrees
#define LEFT_ROTATE_DEGREES 86.5
#define RIGHT_ROTATE_DEGREES 87.5
//#define LEFT_ROTATE_DEGREES 87
//#define RIGHT_ROTATE_DEGREES 87.6
#define ROTATE_LEFT_180 184.5             

//Move Forward fixed distance
#define FORWARD_DISTANCE 10.5
#define FORWARD_DISTANCE_MULTIPLE 9.6
#define MULTIPLE_FORWARD_FACTOR 4.1 / 3 //4.65 + 4.55(for fastest)

//Move Forward Staight/
#define LEFT_RPM 70
#define RIGHT_RPM 69
//#define LEFT_RPM 100
//#define RIGHT_RPM 95
//#define LEFT_RPM 82.2
//#define RIGHT_RPM 77.95 
#define LEFT_RPM_MULTIPLE 100
#define RIGHT_RPM_MULTIPLE 92


// For communication
char source = 't';

// For motors
DualVNH5019MotorShield md;

// For speed data
byte encoder1A = 3;
byte encoder1B = 5;
byte encoder2A = 11;
byte encoder2B = 13;

double speedL = 300, speedR = 300;// In PWM

// For operation mode
bool FASTEST_PATH = false;
bool DEBUG = true;
byte delayExplore = 2.5;
byte delayFastestPath = 1;

// For PID
volatile int ticksL = 0;
volatile int ticksR = 0;
volatile double  ticksDiff = ticksL - ticksR;
double idealTickDiff = 0;

PID PIDController(&ticksDiff, &speedL, &idealTickDiff, 2, 0, 1, DIRECT);  




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

  // Begin communication
  Serial.begin(9600);

  PIDController.SetMode(AUTOMATIC);
  PIDController.SetOutputLimits(-400, 400);
}




void loop()
{
  //printSensors(4);
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

  source = command_buffer[0];
  command = command_buffer[1];

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
    moveForward(FORWARD_DISTANCE_MULTIPLE * 4 + MULTIPLE_FORWARD_FACTOR * 3);
    sendAck();
    break;
  }
  case '5':
  {
    moveForward(FORWARD_DISTANCE_MULTIPLE * 5 + MULTIPLE_FORWARD_FACTOR * 4);
    sendAck();
    break;
  }
  case '6':
  {
    moveForward(FORWARD_DISTANCE_MULTIPLE * 6 + MULTIPLE_FORWARD_FACTOR * 5);
    sendAck();
    break;
  }
  case '7':
  {
    moveForward(FORWARD_DISTANCE_MULTIPLE * 7 + MULTIPLE_FORWARD_FACTOR * 6);
    sendAck();
    break;
  }
  case '8':
  {
    moveForward(FORWARD_DISTANCE_MULTIPLE * 8 + MULTIPLE_FORWARD_FACTOR * 7);
    sendAck();
    break;
  }
   case '9':
  {
    moveForward(FORWARD_DISTANCE_MULTIPLE * 9 + MULTIPLE_FORWARD_FACTOR * 8);
    sendAck();
    break;
  }
   case 'I':
  {
    moveForward(FORWARD_DISTANCE_MULTIPLE * 10 + MULTIPLE_FORWARD_FACTOR * 9);
    sendAck();
    break;
  }
   case 'O':
  {
    moveForward(FORWARD_DISTANCE_MULTIPLE * 11 + MULTIPLE_FORWARD_FACTOR * 10);
    sendAck();
    break;
  }
   case 'P':
  {
    moveForward(FORWARD_DISTANCE_MULTIPLE * 12 + MULTIPLE_FORWARD_FACTOR * 11);
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
