// Rotate left by given degrees. Using 360 degree as a base line
void rotateLeft(double degree)
{
    //at 6.10v to 6.20v
    double targetTicks = 4.3589 * degree;
    ticksL = ticksR = 0;  

    if (targetTicks < 0)
        return;             

    PIDController.SetSampleTime(6.5);
    md.setSpeeds(-speedL, speedR);

    ticksDiff = 0;

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        ticksDiff = ticksL - ticksR;
        PIDController.Compute();
        md.setM1Speed(-speedL);
    }

    md.setBrakes(350, 350);
    delay(delayExplore);
}

// Rotate right by given degrees. Using 360 degree as a base line
void rotateRight(double degree)
{
    //at 6.10v to 6.20v
    double targetTicks = 4.3589 * degree;
    ticksL = ticksR = 0;  

    if (targetTicks < 0)
        return;             

    PIDController.SetSampleTime(6.5);
    md.setSpeeds(speedL, -speedR);

    ticksDiff = 0;

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        ticksDiff = ticksL - ticksR;
        PIDController.Compute();
        md.setM1Speed(speedL);
    }

    md.setBrakes(350, 350);
    delay(delayExplore);
}


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
  oldTicksL = oldTicksR = 0;

  speedL = rpmToSpeedL(LEFT_RPM); //70.75 //74.9  100
  speedR = rpmToSpeedR(RIGHT_RPM); //70.5 //74.5 99.5

  //Set Final ideal speed and accomodate for the ticks we used in acceleration
  md.setSpeeds(speedL, speedR);
  tick_travelled = (double)ticksR;

  while (tick_travelled < target_tick)
  {
    // if not reach destination ticks yet
    currentTicksL = ticksL - oldTicksL; //calculate the ticks travelled in this sample interval of 50ms
    currentTicksR = ticksR - oldTicksR;

    oldTicksR += currentTicksR; //update ticks
    oldTicksL += currentTicksL;
    tick_travelled += currentTicksR;
  }

  //md.setBrakes(370,400);
  md.setBrakes(350, 350);
  delay(delayExplore);
  
  if (FASTEST_PATH)
    delay(delayFastestPath);
   
  md.setBrakes(350, 350);
  delay(delayExplore);
}

// Move robot forward by distance (in cm)
void moveForwardMultiple(float distance)
{
    //at 6.10v to 6.20v
    double targetTicks = 26.85 * distance + FORWARD_TARGET_TICKS;
    ticksL = ticksR = 0;  

    if (targetTicks < 0)
        return;      

    PIDController.SetSampleTime(6.5);
    md.setSpeeds(speedL, speedR);

    ticksDiff = 0;
    PIDController.SetMode(AUTOMATIC);


    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        if(shouldCal) {
          ticksDiff = ticksL - ticksR;
          PIDController.Compute();
          md.setM1Speed(speedL);
        } else {
          md.setM1Speed(speedL);
        }
        ticksDiff = ticksL - ticksR;
//        Serial.print("diff: "); Serial.println(ticksDiff);
//        Serial.print("speedL: "); Serial.println(speedL);
//        Serial.print("speedR: "); Serial.println(speedR);
//        Serial.println();
    }

    md.setBrakes(350, 350);
    PIDController.SetMode(MANUAL);    
    delay(delayExplore);
}

// Move robot backward by distance (in cm)
void moveBackward(float distance)
{
    //at 6.10v to 6.20v
    double targetTicks = 26.85 * distance + FORWARD_TARGET_TICKS;
    ticksL = ticksR = 0;  

    if (targetTicks < 0)
        return;             

    speedL = speedR;
    md.setSpeeds(-speedL, -speedR);

    ticksDiff = 0;

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        md.setM1Speed(-speedL);
    }

    md.setBrakes(350, 350);
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
double rpmToSpeedL(double RPM)
{
    if (RPM == 0)
        return 0;
    else
        return 3.6980 * RPM;
}

// RPM to speed conversion (right motor)
double rpmToSpeedR(double RPM)
{
    if (RPM == 0)
        return 0;
    else
        return 4.0999 * RPM;
}
