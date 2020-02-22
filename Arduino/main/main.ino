#include <DualVNH5019MotorShield.h>
#include <ArduinoSort.h>

// Movement variables
#define SPEED 400
#define REVERSE -400
#define BRAKE 400

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

void setup()
{
  pinMode(LSPIN, INPUT);
  pinMode(LLPIN, INPUT);
  pinMode(RSPIN, INPUT);
  pinMode(FLPIN, INPUT);
  pinMode(FLSPIN, INPUT);
  pinMode(FRSPIN, INPUT);

  Serial.begin(9600);

  md.init();
}

void loop()
{
  char command_buffer[10];
  int i = 0, arg = 0, digit = 1;
  char newChar;

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
    //    String debug = "@t" + command;
    //    String finalDebug = debug + "!";
    //    Serial.println(finalDebug);
  case 'W':
  {
    if (arg == 0)
      moveFront();
    else
      moveFront();
    stopMotors();
    sendAck();
    break;
  }
  case 'A':
  {
    if (arg == 0)
      turnLeft();
    else
      turnLeft();
    stopMotors();
    sendAck();
    break;
  }
  case 'D':
  {
    if (arg == 0)
      turnRight();
    else
      turnRight();
    stopMotors();
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
    moveFront();
    sendAck();
    break;
  }
  case 'L':
  {
    turnLeft();
    stopMotors();
    sendAck();
    break;
  }
  case 'R':
  {
    turnRight();
    stopMotors();
    sendAck();
    break;
  }
  case 'F':
  {
    //TODO: MazeRunner_Flag = true;
    stopMotors();
    sendAck();
    break;
  }
  default:
  {
    break;
  }
    memset(command_buffer, 0, sizeof(command_buffer));
  }
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
  Serial.println("|!");
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

void moveFront()
{
  md.setSpeeds(SPEED, REVERSE);
  delay(500);
  stopIfFault();
}

void moveBack()
{
  md.setSpeeds(REVERSE, SPEED);
  delay(500);
  stopIfFault();
}

void turnLeft()
{
  md.setSpeeds(REVERSE, REVERSE);
  delay(500);
  stopIfFault();
}

void turnRight()
{
  md.setSpeeds(SPEED, SPEED);
  delay(500);
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

int findMedian(int a[], int n)
{
  sortArray(a, n);
  if (n % 2 != 0)
  {
    return a[n / 2];
  }
  return (a[(n - 1) / 2] + a[n / 2]) / 2.0;
}
