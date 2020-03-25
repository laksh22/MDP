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

// Move robot forward by distance (in cm)
void moveForward(float distance)
{
    //at 6.10v to 6.20v
    double targetTicks = 26.85 * distance + FORWARD_TARGET_TICKS;
    ticksL = ticksR = 0;  

    if (targetTicks < 0)
        return;      

    speedL = speedR;

    PIDController.SetSampleTime(6.5);
    md.setSpeeds(speedL, speedR);

    ticksDiff = 0;

    while (ticksL < targetTicks && ticksR < targetTicks)
    {
        ticksDiff = ticksL - ticksR;
        PIDController.Compute();
        md.setM1Speed(speedL);
//        Serial.print("ticksL: "); Serial.println(ticksL);
//        Serial.print("ticksR: "); Serial.println(ticksR);
//        Serial.print("diff: "); Serial.println(ticksDiff);
//        Serial.print("speedL: "); Serial.println(speedL);
//        Serial.print("speedR: "); Serial.println(speedR);
//        Serial.println();
//        Serial.println();    
    }

    md.setBrakes(350, 350);
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
