#define DIST_L_LOWER 1307 // Left sensor distance
#define DIST_L_UPPER DIST_L_LOWER + 20
#define DIST_M_LOWER 1073 // Middle sensor distance
#define DIST_M_UPPER DIST_M_LOWER + 20
#define DIST_R_LOWER 1179 // Right sensor distance
#define DIST_R_UPPER DIST_R_LOWER + 20
#define ROT_L_LOWER 235 // Left-Middle rotation
#define ROT_L_UPPER ROT_L_LOWER + 20
#define ROT_M_LOWER -128 // Left-Right rotation
#define ROT_M_UPPER ROT_M_LOWER + 20
#define ROT_R_LOWER -106 // Middle-Right rotation
#define ROT_R_UPPER ROT_R_LOWER + 20
#define ROT_SIDE_LOWER 106 // Right side rotation
#define ROT_SIDE_UPPER ROT_SIDE_LOWER + 20
#define MAX_ROTATION_COUNT_RIGHT 40
#define MAX_ROTATION_COUNT_FRONT 50
#define MAX_DIST_COUNT 70

void calibrateRightAngle() // ID = 2
{
    int count = 0;

    while (1)
    {
        int RBdistance = sensorRB.distance() * 100;
        int RFdistance = sensorRF.distance() * 100;
        int error = RBdistance - RFdistance;

        if (error > ROT_SIDE_UPPER) // Rotate Left
        {
            rotateLeftCalib(0.01);
        }
        else if (error < ROT_SIDE_LOWER) // Rotate Right
        {
            rotateRightCalib(0.01);
        }
        else
        {
            if (error > ROT_SIDE_LOWER && error < ROT_SIDE_UPPER)
            {
                break;
            }
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
        int FLdistance = sensorFL.distance() * 100;
        int Fdistance = sensorF.distance() * 100;
        int error = FLdistance - Fdistance;

        if (error > ROT_L_UPPER) // Rotate Left
        {
            rotateRightCalib(0.01);
        }
        else if (error < ROT_L_LOWER) // Rotate Right
        {
            rotateLeftCalib(0.01);
        }
        else
        {
            if (error > ROT_L_LOWER && error < ROT_L_UPPER)
            {
                break;
            }
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
        int FLdistance = sensorFL.distance() * 100;
        int FRdistance = sensorFR.distance() * 100;
        int error = FRdistance - FLdistance;

        if (error > ROT_M_UPPER) // Rotate Left
        {
            rotateLeftCalib(0.01);
        }
        else if (error < ROT_M_LOWER) // Rotate Right
        {
            rotateRightCalib(0.01);
        }
        else
        {
            int FLdistance = sensorFL.distance() * 100;
            int FRdistance = sensorFR.distance() * 100;
            int error = FRdistance - FLdistance;
            if (error > ROT_M_LOWER && error < ROT_M_UPPER)
            {
                break;
            }
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
        int Fdistance = sensorF.distance() * 100;
        int FRdistance = sensorFR.distance() * 100;
        int error = Fdistance - FRdistance;

        if (error > ROT_R_UPPER) // Rotate Left
        {
            rotateRightCalib(0.01);
        }
        else if (error < ROT_R_LOWER) // Rotate Right
        {
            rotateLeftCalib(0.01);
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
        int LFdistance = sensorFL.distance() * 100;

        if (LFdistance > DIST_L_UPPER)
        {
            moveForwardCalib(0.01);
        }
        else if (LFdistance < DIST_L_LOWER)
        {
            moveBackwardCalib(0.01);
        }

        else
        {
            break;
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
        int Fdistance = sensorF.distance() * 100;

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
            break;
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
        int RFdistance = sensorFR.distance() * 100;

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
            break;
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
