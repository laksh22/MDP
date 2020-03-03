#include "DualVNH5019MotorShield.h"
#include "SharpIR.h"
#include <EnableInterrupt.h>
#include <PID_v1.h>

String source = "t";

//Define Encoder Pins
byte encoder1A = 3;
byte encoder1B = 5;
byte encoder2A = 11;
byte encoder2B = 13;

double distance_cm;    //distance in cm that the robot need to move.
double speed1, speed2; //speed (in PWM) to feed to the motorshield.

//ticks parameters for PID
volatile word tick1 = 0;
volatile word tick2 = 0;
word ticks_moved = 0;
double currentTick1, currentTick2, oldTick1, oldTick2;

//Operating states
bool FASTEST_PATH = false;
bool DEBUG = false;
byte delayExplore = 2.5;
byte delayFastestPath = 1;

//constructors
DualVNH5019MotorShield md;

// Sensor pins
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

//Refer to end of program for explanation on PID
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
  Serial.begin(9600);
  
  md.init();

  //Attach interrupts to counts ticks
  pinMode(encoder1A, INPUT);
  enableInterrupt(encoder1A, E1_Pos, RISING);
  pinMode(encoder2A, INPUT);
  enableInterrupt(encoder2A, E2_Pos, RISING);

  //init values
  currentTick1 = currentTick2 = oldTick1 = oldTick2 = 0;
}

void loop()
{
  char command_buffer[10];
  int i = 0, arg = 0, digit = 1;
  char newChar;

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

  //First character is the source
  source = command_buffer[0];
  //Second character in array is the command
  char command = command_buffer[1];

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
    move_forward(10);
    sendAck();
    break;
  }
  case 'A':
  {
    rotate_left(90);
    sendAck();
    break;
  }
  case 'D':
  {
    rotate_right(90);
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
    rotate_left(90);
    sendAck();
    break;
  }
  case 'R':
  {
    rotate_left(10);
    sendAck();
    break;
  }
  case 'F':
  {
    //TODO: MazeRunner_Flag = true;
    move_forward(1);
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

//This function sends the sensor data to the RPi
void sendSensors()
{
  int LSDistance = sr1.distance();
  int LLDistance = sr2.distance();
  int RSDistance = sr3.distance();
  int FSDistance = sr4.distance();
  int FLSDistance = sr5.distance();
  int FRSDistance = sr6.distance();
  Serial.flush();
  Serial.print("@");
  Serial.print(source);
  if (LSDistance < 40)
  {
    Serial.print(leftShortToGrids(LSDistance));
    Serial.print(":");
  }
  else
  {
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

int rightShortToGrids(int dis){
  int grids = -1;
  if(dis <= 12) {
    grids = 1;
  } else if (grids > 12 && grids < 23) {
    grids = 2;
  } else if (grids >= 23 && grids < 36){
    grids = 3;
  } else {
    grids = 3;
  }
  return grids;
}

int leftShortToGrids(int dis){
  int grids = -1;
  if(dis <= 13) {
    grids = 1;
  } else if (grids > 13 && grids < 24) {
    grids = 2;
  } else if (grids >= 24 && grids < 35){
    grids = 3;
  } else {
    grids = 3;
  }

  return grids;
}

int leftLongToGrids(int dis){
  int grids = -1;
  if(dis >=28 && grids < 39) {
    grids = 4;
  } else if (grids >= 39 && grids < 51) {
    grids = 5;
  } else if (grids >= 51){
    grids = 6;
  }

  return grids;
}

int frontToGrids(int dis){
  int grids = -1;
  if(dis <= 8) {
    grids = 1;
  } else if (grids > 8 && grids < 18) {
    grids = 2;
  } else if (grids >= 18 && grids < 29){
    grids = 3;
  } else {
    grids = 3;
  }

  return grids;
}

int frontRightToGrids(int dis){
  int grids = -1;
  if(dis <= 13) {
    grids = 1;
  } else if (grids > 13 && grids < 26) {
    grids = 2;
  } else if (grids >= 26 && grids < 40){
    grids = 3;
  } else {
    grids = 3;
  }

  return grids;
}

int frontLeftToGrids(int dis){
  int grids = -1;
  if(dis < 11) {
    grids = 1;
  } else if (grids >= 11 && grids < 22) {
    grids = 2;
  } else if (grids >= 22 && grids < 34){
    grids = 3;
  } else {
    grids = 3;
  }
  return grids;
}

//This function converts the cm readings into grids based on sensor type
//int distanceInGrids(int dis, int sensorType)
//{  
//  int grids;
//  if (sensorType == SRmodel)
//  { //Short range effective up to 2 grids away
//    if (dis > 28)
//      grids = 3;
//    else if (dis >= 10 && dis <= 19)
//      grids = 1;
//    else if (dis >= 20 && dis <= 28)
//      grids = 2;
//    //else if (dis >= 32 && dis <= 38) grids = 3;
//    else
//      grids = -1;
//  }
//  else if (sensorType == LRmodel)
//  { //Long range effective up to 5 grids away
//    if (dis > 58)
//      grids = 6;
//    else if (dis >= 12 && dis <= 22)
//      grids = 1;
//    else if (dis > 22 && dis <= 27)
//      grids = 2;
//    else if (dis >= 30 && dis <= 37)
//      grids = 3;
//    else if (dis >= 39 && dis <= 48)
//      grids = 4;
//    else if (dis >= 49 && dis <= 58)
//      grids = 5;
//    else
//      grids = -1;
//  }
//
//  return grids;
//}

void sendAck()
{
  Serial.print("@");
  Serial.print(source);
  Serial.println("Y!");
}

/*
 * =======================================================
 * Motion Controls
 * Methods to move the robot straight, left, right, etc...
 * =======================================================
 */

//A method to rotate robot to the right by a degree. Using 360 degree as a base line
void rotate_right(double degree)
{
  double target_tick = 0;
  target_tick = 4.3589 * degree - 60;
  //target_tick = 373;

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
  speed1 = rpm_to_speed_1(70);
  speed2 = rpm_to_speed_2(-69);

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

  //     for (word i = 0; i <= 400; i+=200){
  //     md.setBrakes(i,i);
  //     delay(1);
  //}
  md.setBrakes(400, 400);
  PIDControlRight.SetMode(MANUAL);

  delay(delayExplore);
  if (FASTEST_PATH)
    delay(delayFastestPath);
}

//A method to rotate robot to the left by a degree. Using 360 degree as a base line
void rotate_left(double degree)
{
  double target_tick = 0;
  target_tick = 4.3589 * degree + 8;
  //target_tick = 384;

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
  speed1 = rpm_to_speed_1(-70);
  speed2 = rpm_to_speed_2(69);
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

  //     for (word i = 0; i <= 400; i+=200){
  //     md.setBrakes(i,i);
  //     delay(1);
  //   }
  md.setBrakes(400, 400);
  PIDControlLeft.SetMode(MANUAL); //turn off PID
  delay(delayExplore);
  if (FASTEST_PATH)
    delay(delayFastestPath);
}

//A method to move robot forward by distance/unit of square
void move_forward(byte distance)
{
  //at 6.10v to 6.20v
  double rpm1, rpm2;
  double target_tick = 0;
  if (FASTEST_PATH)
  {
    switch (distance)
    {
    case 10:
      target_tick = 296;
      break;
    case 20:
      target_tick = 604;
      break;
    case 30:
      target_tick = 926;
      break;
    case 40:
      target_tick = 1237;
      break;
    case 50:
      target_tick = 1547;
      break;
    case 60:
      target_tick = 1860;
      break;
    case 70:
      target_tick = 2170;
      break;
    case 80:
      target_tick = 2487;
      break;
    case 90:
      target_tick = 2810;
      break;
    default:
      target_tick = 31.36 * distance - 19.4;
    }
  }

  else
    target_tick = 284; //289 // EDITED
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
  speed1 = rpm_to_speed_1_forward(rpm1); //70.75 //74.9  100
  speed2 = rpm_to_speed_2_forward(rpm2); //70.5 //74.5 99.5

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

void E1_Pos()
{
  tick1++;
}

void E2_Pos()
{
  tick2++;
}


double rpm_to_speed_1_forward(double RPM)
{
  if (RPM > 0)
    return 2.815 * RPM + 64.15;
  else
    return 0;
//  else
//    return -2.9117 * (-1) * RPM - 45.197;
}

double rpm_to_speed_2_forward(double RPM)
{
  if (RPM > 0)
    return 2.7845 * RPM + 83;
  else
    return 0;
//  else
//    return -2.8109 * (-1) * RPM - 54.221;
}

double rpm_to_speed_1(double RPM)
{
  if (RPM > 0)
    return 2.8598 * RPM + 51.582;
  else if (RPM == 0)
    return 0;
  else
    return -2.9117 * (-1) * RPM - 45.197;
}

double rpm_to_speed_2(double RPM)
{
  if (RPM > 0)
    return 2.7845 * RPM + 83;
  else if (RPM == 0)
    return 0;
  else
    return -2.8109 * (-1) * RPM - 54.221;
}
