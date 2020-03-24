// Rotate left by given degrees
void rotateLeft(double degree)
{
    double targetTicks = 4.3589 * degree;

    if (targetTicks < 0)
        return;

    ticksL = ticksR = 0;

    md.setSpeeds(-speedL, speedR);

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        ticksDiff = ticksL - ticksR;
        PIDControlStraight.Compute();
        md.setM1Speed(-speedL); 
    }

    md.setBrakes(400, 400);
    delay(delayExplore);
}

// Rotate right by given degrees
void rotateRight(double degree)
{
    double targetTicks = 4.3589 * degree;

    if (targetTicks < 0)
        return;

    ticksL = ticksR = 0;

    md.setSpeeds(speedL, -speedR);

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        ticksDiff = ticksL - ticksR;
        PIDControlStraight.Compute();
        md.setM1Speed(speedL); 
    }

    md.setBrakes(400, 400);
    delay(delayExplore);
}

// Move robot forward by distance (in cm)
void moveForward(float distance)
{
    //at 6.10v to 6.20v
    double targetTicks = 26.85 * distance + FORWARD_TARGET_TICKS;
    ticksL = ticksR = 0;

    if (targetTicks < 0)
        return;

    PIDControlStraight.SetSampleTime(6.5);
    md.setSpeeds(speedL, speedR); 

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        ticksDiff = ticksL - ticksR;
        PIDControlStraight.Compute();
        md.setM1Speed(speedL);
        Serial.println("SDF");
    }

    md.setBrakes(350, 350);
    delay(delayExplore);
}

// Move robot backward by distance (in cm)
void moveBackward(float distance)
{
    //at 6.10v to 6.20v
    double targetTicks = 0;
    ticksL = ticksR = 0;

    targetTicks = 26.85 * distance + FORWARD_TARGET_TICKS;

    if (targetTicks < 0)
        return;

    PIDControlStraight.SetSampleTime(6.5);
    md.setSpeeds(-speedL, -speedR); 

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        ticksDiff = ticksL - ticksR;
        PIDControlStraight.Compute();
        md.setM1Speed(-speedL);
    }

    md.setBrakes(350, 350);
    delay(delayExplore);
}

// Increase ticks (left motor)
void ELPos()
{
    ticksL++;    
    Serial.println(ticksL);    
}

// Increase ticks (right motor)
void ERPos()
{
    ticksR++;
}
