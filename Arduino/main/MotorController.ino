// Rotate left by given degrees
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

// Rotate right by given degrees
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
    double target_tick = 26.85 * distance;
    double tick_travelled = 0;

    if (target_tick < 0)
        return;

    // Init values
    ticksL = ticksR = 0;
    currentTicksR = 0;
    oldTicksR = 0;

    speedL = rpmToSpeedL(LEFT_RPM);
    speedR = rpmToSpeedR(RIGHT_RPM);

    md.setSpeeds(speedL, speedR);
    tick_travelled = (double)ticksR;

    while (tick_travelled < target_tick)
    {
        currentTicksR = ticksR - oldTicksR;
        oldTicksR += currentTicksR;
        tick_travelled += currentTicksR;
    }

    md.setBrakes(350, 350);
    delay(delayExplore);
}

// Move robot forward by distance (in cm)
void moveForwardMultiple(float distance)
{
    //at 6.10v to 6.20v
    double targetTicks = 26.85 * distance;
    ticksL = ticksR = 0;

    if (targetTicks < 0)
        return;

    PIDController.SetSampleTime(6.5);
    md.setSpeeds(speedL, speedR);

    ticksDiff = 0;

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        if (shouldCal)
        {
            ticksDiff = ticksL - ticksR;
            PIDController.Compute();
            md.setM1Speed(speedL);
        }
        else
        {
            md.setM1Speed(speedL);
        }
        ticksDiff = ticksL - ticksR;
    }

    md.setBrakes(350, 350);
    delay(delayExplore);
}

// Move robot backward by distance (in cm)
void moveBackward(float distance)
{
    //at 6.10v to 6.20v
    double targetTicks = 26.85 * distance;
    ticksL = ticksR = 0;

    if (targetTicks < 0)
        return;

    PIDController.SetSampleTime(6.5);
    md.setSpeeds(-speedL, -speedR);

    ticksDiff = 0;

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        ticksDiff = ticksL - ticksR;
        PIDController.Compute();
        md.setM1Speed(-speedL);
        ticksDiff = ticksL - ticksR;
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
