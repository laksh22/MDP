#include <DualVNH5019MotorShield.h>
#include <ArduinoSort.h>
#include <PinChangeInt.h>
#include <PID_v1.h>

// Movement constants
#define LEFT_SPEED 300
#define LEFT_REVERSE -278
#define RIGHT_SPEED 297
#define RIGHT_REVERSE -262
#define BRAKE 400

// Encoder constants
#define leftEncoderPinA 3
#define leftEncoderPinB 5
#define rightEncoderPinA 11
#define rightEncoderPinB 13
#define MOVE_DISTANCE 10 // In cm

// Rotation constants
#define wheelDistance 17
#define ROTATE_DISTANCE_RIGHT (3.14 * wheelDistance / 4) + 0.3 - 0.648 
#define ROTATE_DISTANCE_LEFT (3.14 * wheelDistance / 4) - 0.22 + 0.75 

// Sensor pins
#define LSPIN A0  // PS1
#define LLPIN A1  // PS2
#define RSPIN A2  // PS3
#define FLPIN A3  // PS4
#define FLSPIN A4 // PS5
#define FRSPIN A5 // PS6

// Sensor types
#define SR -1000
#define LR -1001

// Buffer for sensor values
#define BUFFER 30

// Sensor calculation parameters
#define LCONST 60.374
#define SCONST 29.988
#define L_EXP -1.16
#define S_EXP -1.173

// Motor shield | M1 = left, M2 = right
DualVNH5019MotorShield md;

//Encoder variables
unsigned long ticksL = 0; //number of times we've seen rising/falling edge in encoder output
volatile long encoderPosL = 0;
unsigned long ticksR = 0; //number of times we've seen rising/falling edge in encoder output
volatile long encoderPosR = 0;

int distanceToMove = 0;
bool leftDone = false;
bool rightDone = false;

// Motor variables
int leftOutputSpeed = LEFT_SPEED;
int rightOutputSpeed = RIGHT_SPEED;
int leftSetSpeed = LEFT_SPEED;
int rightSetSpeed = RIGHT_SPEED;

//PID
// double leftKp = 2, leftKi = 5, leftKd = 1;
// double rightKp = 2, rightKi = 5, rightKd = 1;
// PID leftPID(&Input, &leftOutputSpeed, &leftSetSpeed, leftKp, leftKi, leftKd, DIRECT);
// PID rightPID(&Input, &rightOutputSpeed, &rightSetSpeed, rightKp, rightKi, rightKd, DIRECT);

void setup()
{
  // IR Sensors
  pinMode(LSPIN, INPUT);
  pinMode(LLPIN, INPUT);
  pinMode(RSPIN, INPUT);
  pinMode(FLPIN, INPUT);
  pinMode(FLSPIN, INPUT);
  pinMode(FRSPIN, INPUT);

  // Rotary encoders
  pinMode(leftEncoderPinA, INPUT_PULLUP);
  pinMode(leftEncoderPinB, INPUT_PULLUP);
  pinMode(rightEncoderPinA, INPUT_PULLUP);
  pinMode(rightEncoderPinB, INPUT_PULLUP);

  attachPinChangeInterrupt(rightEncoderPinA, countTicksCalcPosR, RISING);
  attachPinChangeInterrupt(leftEncoderPinA, countTicksCalcPosL, RISING);

  // PID
  //  leftPID.SetMode(AUTOMATIC);
  //  rightPID.SetMode(AUTOMATIC);

  // Serial communication
  Serial.begin(9600);
  Serial.flush();

  // Motor
  md.init();
}

void loop()
{
  leftDone = false;
  rightDone = false;
  char command_buffer[10];
  int i = 0, arg = 0, digit = 1;
  char newChar;
  attachPinChangeInterrupt(rightEncoderPinA, countTicksCalcPosR, RISING);
  attachPinChangeInterrupt(leftEncoderPinA, countTicksCalcPosL, RISING);

  /*---------------------------------------------------------------------------------------------------
                                 Establishing Serial Connection with RPi
  ---------------------------------------------------------------------------------------------------*/
  while (1)
  {
    if (Serial.available())
    {
      newChar = Serial.read();
      command_buffer[i] = newChar;
      i++;
      if (newChar == '|')
      {
        i = 1;
        break;
      }
    }
  }

  //First character in array is the command
  char command = command_buffer[0];

  //Converts subsequent characters in the array into an integer
  while (command_buffer[i] != '|')
  {
    arg = arg + (digit * (command_buffer[i] - 48));
    digit *= 10;
    i++;
  }

  //  command = 'W';
  //  arg = 0;
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
    distanceToMove = MOVE_DISTANCE;
    moveFront();
    move();
    break;
  }
  case 'A':
  {
    distanceToMove = ROTATE_DISTANCE_LEFT;
    turnLeft();
    move();
    break;
  }
  case 'D':
  {
    distanceToMove = ROTATE_DISTANCE_RIGHT;
    turnRight();
    move();
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
    turnLeft();
    break;
  }
  case 'R':
  {
    turnRight();
    break;
  }
  case 'F':
  {
    //TODO: MazeRunner_Flag = true;
    stopMotors();
    break;
  }
  default:
  {
    break;
  }
    memset(command_buffer, 0, sizeof(command_buffer));
  }
}

void countTicksCalcPosL()
{
  if(leftDone == true) {return;}
  ticksL++;

  int maxDistance = distanceToMove * 10 * 11.93 / 4;

  noInterrupts();
  if (ticksL > maxDistance && ticksR > maxDistance)
  {
    stopMotors();
    ticksL = 0;
    leftDone = true;
    sendAck();
    detachPinChangeInterrupt(rightEncoderPinA);
  } else {
    move();
  }
  interrupts(); 
}

void countTicksCalcPosR()
{
  if(rightDone == true) {return;}
  ticksR++;

  int maxDistance = distanceToMove * 10 * 11.93 / 4;

  noInterrupts();
  if (ticksL > maxDistance && ticksR > maxDistance)
  {
    stopMotors();
    ticksR = 0;
    rightDone = true;
    sendAck();
    detachPinChangeInterrupt(leftEncoderPinA);
  } else {
    move();
  }
  interrupts();
}

void sendAck()
{
  Serial.println("@tY!");
}

//This function sends the sensor data to the RPi
void sendSensors()
{
  int LSDistance = getShortIRDistance(LSPIN);
  int LLDistance = getLongIRDistance(LLPIN);
  int RSDistance = getShortIRDistance(RSPIN);
  int FLDistance = getLongIRDistance(FLPIN);
  int FLSDistance = getLongIRDistance(FLSPIN);
  int FRSDistance = getLongIRDistance(FRSPIN);

  Serial.flush();
  Serial.print("@t");
  if (LSDistance <= 40)
  {
    Serial.print(distanceInGrids(LSDistance, SR));
    Serial.print(":");
  }
  else
  {
    Serial.print(distanceInGrids(LLDistance, LR));
    Serial.print(":");
  }
  Serial.print(distanceInGrids(RSDistance, SR));
  Serial.print(":");
  Serial.print(distanceInGrids(FLDistance, LR));
  Serial.print(":");
  Serial.print(distanceInGrids(FLSDistance, SR));
  Serial.print(":");
  Serial.print(distanceInGrids(FRSDistance, SR));
  Serial.println("!");
}

//This function converts the cm readings into grids based on sensor type
int distanceInGrids(int dis, int sensorType)
{
  int grids;
  if (sensorType == SR)
  { //Short range effective up to 2 grids away
    if (dis > 28)
      grids = 3;
    else if (dis >= 10 && dis <= 19)
      grids = 1;
    else if (dis >= 20 && dis <= 28)
      grids = 2;
    //else if (dis >= 32 && dis <= 38) grids = 3;
    else
      grids = -1;
  }
  else if (sensorType == LR)
  { //Long range effective up to 5 grids away
    if (dis > 58)
      grids = 6;
    else if (dis >= 12 && dis <= 22)
      grids = 1;
    else if (dis > 22 && dis <= 27)
      grids = 2;
    else if (dis >= 30 && dis <= 37)
      grids = 3;
    else if (dis >= 39 && dis <= 48)
      grids = 4;
    else if (dis >= 49 && dis <= 58)
      grids = 5;
    else
      grids = -1;
  }

  return grids;
}

void printSensors()
{
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

int getLongIRDistance(int pin)
{
  int sensorValues[BUFFER];
  for (int i = 0; i < BUFFER; i++)
  {
    int sensorValue = analogRead(pin);
    float voltage = sensorValue * (5.0 / 1023.0);
    int distance = LCONST * pow(voltage, L_EXP);
    sensorValues[i] = distance;
  }
  sortArray(sensorValues, BUFFER);
  return findMedian(sensorValues, BUFFER);
}

int getShortIRDistance(int pin)
{
  int sensorValues[BUFFER];
  for (int i = 0; i < BUFFER; i++)
  {
    int sensorValue = analogRead(pin);
    float voltage = sensorValue * (5.0 / 1023.0);
    int distance = SCONST * pow(voltage, S_EXP);
    sensorValues[i] = distance;
  }
  sortArray(sensorValues, BUFFER);
  return findMedian(sensorValues, BUFFER);
}

void move()
{
  md.setSpeeds(leftOutputSpeed, rightOutputSpeed);
  stopIfFault();
}

void moveFront()
{
  leftSetSpeed = LEFT_SPEED;
  rightSetSpeed = RIGHT_SPEED;
  leftOutputSpeed = LEFT_SPEED;
  rightOutputSpeed = RIGHT_SPEED;
}

void moveBack()
{
  leftSetSpeed = LEFT_REVERSE;
  rightSetSpeed = RIGHT_REVERSE;
  leftOutputSpeed = LEFT_REVERSE;
  rightOutputSpeed = RIGHT_REVERSE;
}

void turnLeft()
{
  leftSetSpeed = LEFT_REVERSE;
  rightSetSpeed = RIGHT_SPEED;
  leftOutputSpeed = LEFT_REVERSE;
  rightOutputSpeed = RIGHT_SPEED;
}

void turnRight()
{
  leftSetSpeed = LEFT_SPEED;
  rightSetSpeed = RIGHT_REVERSE;
  leftOutputSpeed = LEFT_SPEED;
  rightOutputSpeed = RIGHT_REVERSE;
}

void stopMotors()
{
  md.setBrakes(BRAKE, BRAKE);
}

void stopIfFault()
{
  if (md.getM1Fault())
  {
    while (1)
      ;
  }
  if (md.getM2Fault())
  {
    while (1)
      ;
  }
}

int findMedian(int a[], int n)
{
  sortArray(a, n);
  if (n % 2 != 0)
  {
    return a[n / 2];
  }
  return (a[(n - 1) / 2] + a[n / 2]) / 2.0;
}