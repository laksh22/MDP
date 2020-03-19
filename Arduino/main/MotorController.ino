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
//    speedL = rpmTospeedL(-66.25);
//    speedR = rpmTospeedR(64.9);
    speedL = rpmTospeedL(-100);
    speedR = rpmTospeedR(98);
    
    md.setSpeeds(speedL, speedR);
    tick_travelled = (double)ticksR;

    PIDControlLeft.SetSampleTime(15);  //Controller is called every 50ms
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
//    speedL = rpmTospeedL(66.25);
//    speedR = rpmTospeedR(-64.9);
    speedL = rpmTospeedL(100);
    speedR = rpmTospeedR(-98);

    md.setSpeeds(speedL, speedR);
    tick_travelled = (double)ticksR;

    PIDControlRight.SetSampleTime(15);  //Controller is called every 25ms
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

    speedL = rpmTospeedL(LEFT_RPM);  //70.75 //74.9  100
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

    speedL = rpmTospeedL(-LEFT_RPM); //70.75 //74.9  100
    speedR = rpmTospeedR(-RIGHT_RPM); //70.5 //74.5 99.5

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

// Move robot forward by distance (in cm)
void moveForwardCalib(float distance)
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

    speedL = rpmTospeedL(69/2);  //70.75 //74.9  100
    speedR = rpmTospeedR(73/2); //70.5 //74.5 99.5

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
void moveBackwardCalib(float distance)
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

    speedL = rpmTospeedL(-69/2); //70.75 //74.9  100
    speedR = rpmTospeedR(-69/2); //70.5 //74.5 99.5

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

// Rotate left by given degrees. Using 360 degree as a base line
void rotateLeftCalib(double degree)
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
    speedL = rpmTospeedL(-66.25/2);
    speedR = rpmTospeedR(64.9/2);

    md.setSpeeds(speedL, speedR);
    tick_travelled = (double)ticksR;

    PIDControlLeft.SetSampleTime(15);  //Controller is called every 50ms
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
void rotateRightCalib(double degree)
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
    speedL = rpmTospeedL(66.25/2);
    speedR = rpmTospeedR(-64.9/2);

    md.setSpeeds(speedL, speedR);
    tick_travelled = (double)ticksR;

    PIDControlRight.SetSampleTime(15);  //Controller is called every 25ms
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
