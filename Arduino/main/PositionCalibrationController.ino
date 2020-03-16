void calibrateFrontAngleLM(bool calibrateDistance) // ID = 1
{
    int count = 0;

    while (1)
    {
        float FLdistance = sensorF.distance();
        float Fdistance = sensorFR.distance();
        float error = FLdistance - Fdistance;

        if (error > 1.40) // Rotate Left
        {
            md.setSpeeds(-150, 150);
            if (error > 2.35)
            {
                delay(30 * abs(error));
            }
            else
            {
                delay(12);
            }
        }
        else if (error < 1.30) // Rotate Right
        {
            md.setSpeeds(150, -150);
            if (error < -0.12)
            {
                delay(30 * abs(error));
            }
            else
            {
                delay(12);
            }
        }
        else
        {
            md.setBrakes(400, 400);
            break;
        }

        md.setBrakes(100, 100);

        count++;
        if (count > 10)
        {
            break;
        }
    }

    float Fdistance = sensorF.distance();

    if (calibrateDistance && (Fdistance > 15 || Fdistance < 15)) // TODO
        calibrateDistanceM(1, false);
}

void calibrateFrontAngleLR(bool calibrateDistance) // ID = 2
{
    int count = 0;

    while (1)
    {
        float FLdistance = sensorFL.distance();
        float FRdistance = sensorFR.distance();
        float error = FRdistance - FLdistance;

        if (error > 1.40) // Rotate Left
        {
            md.setSpeeds(-150, 150);
            if (error > 2.35)
            {
                delay(30 * abs(error));
            }
            else
            {
                delay(12);
            }
        }
        else if (error < 1.30) // Rotate Right
        {
            md.setSpeeds(150, -150);
            if (error < -0.12)
            {
                delay(30 * abs(error));
            }
            else
            {
                delay(12);
            }
        }
        else
        {
            md.setBrakes(400, 400);
            break;
        }

        md.setBrakes(100, 100);

        count++;
        if (count > 10)
        {
            break;
        }
    }

    float FLdistance = sensorFL.distance();

    if (calibrateDistance && (FLdistance > 15 || FLdistance < 15)) // TODO
        calibrateDistanceL(2, false);
}

void calibrateFrontAngleMR(bool calibrateDistance) // ID = 3
{
    int count = 0;

    while (1)
    {
        float Fdistance = sensorF.distance();
        float FRdistance = sensorFR.distance();
        float error = Fdistance - FRdistance;

        if (error > 1.40) // Rotate Left
        {
            md.setSpeeds(-150, 150);
            if (error > 2.35)
            {
                delay(30 * abs(error));
            }
            else
            {
                delay(12);
            }
        }
        else if (error < 1.30) // Rotate Right
        {
            md.setSpeeds(150, -150);
            if (error < -0.12)
            {
                delay(30 * abs(error));
            }
            else
            {
                delay(12);
            }
        }
        else
        {
            md.setBrakes(400, 400);
            break;
        }

        md.setBrakes(100, 100);

        count++;
        if (count > 10)
        {
            break;
        }
    }

    float Fdistance = sensorF.distance();

    if (calibrateDistance && (Fdistance > 15 || Fdistance < 15)) // TODO
        calibrateDistanceM(3, false);
}

void calibrateDistanceL(int id, bool calibrateAngle)
{
    while (1)
    {
        float LFdistance = sensorFL.distance();

        if (LFdistance > 13.6)
            moveForward(0.01);
        else if (LFdistance < 13.4)
            moveBackward(0.01);
        else
            break;
    }

    float angleError = 0;

    switch (id)
    {
    case 1:
        angleError = sensorFL.distance() - sensorF.distance();
        if (calibrateAngle && (angleError > 3 || angleError < -3))
            calibrateFrontAngleLM(false);
        return;
    case 2:
        angleError = sensorFL.distance() - sensorFR.distance();
        if (calibrateAngle && (angleError > 3 || angleError < -3))
            calibrateFrontAngleLR(false);
        return;
    default:
        return;
    }
}

void calibrateDistanceM(int id, bool calibrateAngle)
{
    while (1)
    {
        float Fdistance = sensorF.distance();

        if (Fdistance > 10.5)
            moveForward(0.01);
        else if (Fdistance < 10.3)
            moveBackward(0.01);
        else
            break;
    }

    float angleError = 0;

    switch (id)
    {
    case 1:
        angleError = sensorFL.distance() - sensorF.distance();
        if (calibrateAngle && (angleError > 3 || angleError < -3))
            calibrateFrontAngleLM(false);
        return;
    case 3:
        angleError = sensorF.distance() - sensorFR.distance();
        if (calibrateAngle && (angleError > 3 || angleError < -3))
            calibrateFrontAngleMR(false);
        return;
    default:
        return;
    }
}

void calibrateDistanceR(int id, bool calibrateAngle)
{
    while (1)
    {
        float RFdistance = sensorFR.distance();

        if (RFdistance > 12.9)
            moveForward(0.01);
        else if (RFdistance < 12.7)
            moveBackward(0.01);
        else
            break;
    }

    float angleError = 0;

    switch (id)
    {
    case 2:
        angleError = sensorFL.distance() - sensorFR.distance();
        if (calibrateAngle && (angleError > 3 || angleError < -3))
            calibrateFrontAngleLR(false);
        return;
    case 3:
        angleError = sensorF.distance() - sensorFR.distance();
        if (calibrateAngle && (angleError > 3 || angleError < -3))
            calibrateFrontAngleMR(false);
        return;
    default:
        return;
    }
}