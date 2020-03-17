void calibrateRightAngle() // ID = 2
{
    int count = 0;
    float RBdistance = sensorRB.distance();
    float RFdistance = sensorRF.distance();
    float error = RBdistance - RFdistance;

    if (RBdistance > 19 || RFdistance > 17)
    {
        return;
    }

    while (1)
    {
        RBdistance = sensorRB.distance();
        RFdistance = sensorRF.distance();
        error = RBdistance - RFdistance;

        if (error > 1.10) // Rotate Left
        {
            rotateLeft(abs(error) / 3000);
        }
        else if (error < 1.01) // Rotate Right
        {
            rotateRight(abs(error) / 3000);
        }
        else
        {
            break;
        }

        count++;
        if (count > 20)
        {
            break;
        }
    }
}

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
                delay(20 * abs(error));
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
                delay(20 * abs(error));
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

        if (error > -0.45) // Rotate Left
        {
            if (error > 0.04)
            {
                rotateLeft(abs(error) / 300);
            }
            else
            {
                rotateLeft(abs(error) / 1000);
            }
        }
        else if (error < -0.55) // Rotate Right
        {
            if (error < -1.20)
            {
                rotateRight(abs(error) / 300);
            }
            else
            {
                rotateRight(abs(error) / 1000);
            }
        }
        else
        {
            break;
        }

        count++;
        if (count > 100)
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
    int count = 0;
    while (1)
    {
        float LFdistance = sensorFL.distance();

        if (LFdistance > 13.17)
        {
            if (LFdistance > 13.70)
            {
                moveForward(LFdistance / 10000);
            }
            else
            {
                moveForward(0.01);
            }
        }
        else if (LFdistance < 13.10)
        {
            if (LFdistance < 12.25)
            {
                moveBackward(LFdistance / 10000);
            }
            else
            {
                moveBackward(0.01);
            }
        }

        else
        {
            LFdistance = sensorFL.distance();
            if (LFdistance <= 13.17 && LFdistance >= 13.10)
            {
                break;
            }
        }

        count++;

        if (count > 100)
        {
            break;
        }
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
    int count = 0;
    float targetDistance = 10.81;
    while (1)
    {
        float Fdistance = sensorF.distance();
        float error = abs(targetDistance - Fdistance);

        if (Fdistance > 10.20)
        {
            if (Fdistance > 11.0)
            {
                moveForward(Fdistance / 10000);
            }
            else
            {
                moveForward(0.01);
            }
        }
        else if (Fdistance < 10.15)
            if (Fdistance < 9.80)
            {
                moveBackward(Fdistance / 10000);
            }
            else
            {
                moveBackward(0.01);
            }
        else
        {
            Fdistance = sensorF.distance();
            if (Fdistance <= 10.20 && Fdistance >= 10.15)
            {
                break;
            }
        }

        count++;

        if (count > 100)
        {
            break;
        }
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
    int count = 0;
    while (1)
    {
        float RFdistance = sensorFR.distance();

        if (RFdistance > 12.35)
        {
            if (RFdistance > 13.10)
            {
                moveForward(RFdistance / 1000);
            }
            else
            {
                moveForward(RFdistance / 2000);
            }
        }
        else if (RFdistance < 12.30)
        {
            if (RFdistance < 11.70)
            {
                moveBackward(RFdistance / 1000);
            }
            else
            {
                moveBackward(RFdistance / 2000);
            }
        }
        else
        {
            RFdistance = sensorFR.distance();
            if (RFdistance <= 12.35 && RFdistance >= 13.30)
            {
                break;
            }
        }

        count++;

        if (count > 100)
        {
            break;
        }
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
