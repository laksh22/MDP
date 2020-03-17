#define DIST_L_LOWER 13.10
#define DIST_L_UPPER 13.17
#define DIST_M_LOWER 10.20
#define DIST_M_UPPER 10.15
#define DIST_R_LOWER 12.35
#define DIST_R_UPPER 12.30
#define ROT_L_UPPER -2.15
#define ROT_L_LOWER -2.05
#define ROT_M_UPPER -0.45
#define ROT_M_LOWER -0.55
#define ROT_R_UPPER -2.24
#define ROT_R_LOWER -2.50
#define ROT_SIDE_LOWER 1.01
#define ROT_SIDE_UPPER 1.10
#define MAX_ROTATION_COUNT_RIGHT 20
#define MAX_ROTATION_COUNT_FRONT 100
#define MAX_DIST_COUNT 100

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

        if (error > ROT_SIDE_UPPER) // Rotate Left
        {
            rotateLeft(abs(error) / 5000);
        }
        else if (error < ROT_SIDE_LOWER) // Rotate Right
        {
            rotateRight(abs(error) / 5000);
        }
        else
        {
            break;
        }

        count++;
        if (count > MAX_ROTATION_COUNT_RIGHT)
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

        if (error > ROT_L_UPPER) // Rotate Left
        {
            if (error > ROT_L_UPPER + 0.50)
            {
                rotateRightCalib(abs(error) / 500);
            }
            else
            {
                rotateRightCalib(abs(error) / 2000);
            }
        }
        else if (error < ROT_L_LOWER) // Rotate Right
        {
            if (error < ROT_L_LOWER - 0.50)
            {
                rotateLeftCalib(abs(error) / 500);
            }
            else
            {
                rotateLeftCalib(abs(error) / 2000);
            }
        }
        else
        {
            break;
        }

        count++;
        if (count > MAX_ROTATION_COUNT_FRONT)
        {
            break;
        }
    }

    float Fdistance = sensorF.distance();

    if (calibrateDistance && (Fdistance > 15 || Fdistance < 15)) // TODO
        calibrateDistanceM(1, true);
}

void calibrateFrontAngleLR(bool calibrateDistance) // ID = 2
{
    int count = 0;

    while (1)
    {
        float FLdistance = sensorFL.distance();
        float FRdistance = sensorFR.distance();
        float error = FRdistance - FLdistance;

        if (error > ROT_M_UPPER) // Rotate Left
        {
            if (error > ROT_M_UPPER + 0.50)
            {
                rotateLeftCalib(abs(error) / 500);
            }
            else
            {
                rotateLeftCalib(abs(error) / 2000);
            }
        }
        else if (error < ROT_M_LOWER) // Rotate Right
        {
            if (error < ROT_M_LOWER - 0.50)
            {
                rotateRightCalib(abs(error) / 500);
            }
            else
            {
                rotateRightCalib(abs(error) / 2000);
            }
        }
        else
        {
            break;
        }

        count++;
        if (count > MAX_ROTATION_COUNT_FRONT)
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

        if (error > ROT_R_UPPER) // Rotate Left
        {
            if (error > ROT_R_UPPER + 0.50)
            {
                rotateRightCalib(abs(error) / 500);
            }
            else
            {
                rotateRightCalib(abs(error) / 2000);
            }
        }
        else if (error < ROT_R_LOWER) // Rotate Right
        {
            if (error < ROT_R_LOWER - 0.50)
            {
                rotateLeftCalib(abs(error) / 500);
            }
            else
            {
                rotateLeftCalib(abs(error) / 2000);
            }
        }
        else
        {
            break;
        }

        count++;
        if (count > MAX_ROTATION_COUNT_FRONT)
        {
            break;
        }
    }

    float Fdistance = sensorF.distance();

    if (calibrateDistance && (Fdistance > 15 || Fdistance < 15)) // TODO
        calibrateDistanceM(3, true);
}

void calibrateDistanceL(int id, bool calibrateAngle)
{
    int count = 0;
    while (1)
    {
        float LFdistance = sensorFL.distance();

        if (LFdistance > DIST_L_UPPER)
        {
            moveForwardCalib(0.001);
        }
        else if (LFdistance < DIST_L_LOWER)
        {
            moveBackwardCalib(0.001);
        }

        else
        {
            LFdistance = sensorFL.distance();
            if (LFdistance <= DIST_L_UPPER && LFdistance >= DIST_L_LOWER)
            {
                break;
            }
        }

        count++;

        if (count > MAX_DIST_COUNT)
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
    while (1)
    {
        float Fdistance = sensorF.distance();

        if (Fdistance > DIST_M_UPPER)
        {
            moveForwardCalib(0.001);
        }
        else if (Fdistance < DIST_M_LOWER)
        {
            moveBackwardCalib(0.001);
        }
        else
        {
            Fdistance = sensorF.distance();
            if (Fdistance <= DIST_M_UPPER && Fdistance >= DIST_M_LOWER)
            {
                break;
            }
        }

        count++;

        if (count > MAX_DIST_COUNT)
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

        if (RFdistance > DIST_R_UPPER)
        {
            moveForwardCalib(0.001);
        }
        else if (RFdistance < DIST_R_LOWER)
        {
            moveBackwardCalib(0.001);
        }
        else
        {
            RFdistance = sensorFR.distance();
            if (RFdistance <= DIST_R_UPPER && RFdistance >= DIST_R_LOWER)
            {
                break;
            }
        }

        count++;

        if (count > MAX_DIST_COUNT)
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
