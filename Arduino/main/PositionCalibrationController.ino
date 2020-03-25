#define DIST_L_LOWER 1120 // Left sensor distance
#define DIST_L_UPPER DIST_L_LOWER + 20

#define DIST_M_LOWER 1055 // Middle sensor distance
#define DIST_M_UPPER DIST_M_LOWER + 20

#define DIST_R_LOWER 1145 // Right sensor distance
#define DIST_R_UPPER DIST_R_LOWER + 20

#define ROT_L_LOWER 60 // Left-Middle rotation
#define ROT_L_UPPER ROT_L_LOWER + 20

#define ROT_M_LOWER 20 // Left-Right rotation
#define ROT_M_UPPER ROT_M_LOWER + 20

#define ROT_R_LOWER -93 // Middle-Right rotation
#define ROT_R_UPPER ROT_R_LOWER + 20

#define ROT_SIDE_LOWER 68 // Right side rotation
#define ROT_SIDE_UPPER ROT_SIDE_LOWER + 20

#define MAX_ROTATION_COUNT_RIGHT 50
#define MAX_ROTATION_COUNT_FRONT 80
#define MAX_DIST_COUNT 100

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
            rotateLeft(0.01);
        }
        else if (error < ROT_SIDE_LOWER) // Rotate Right
        {
            rotateRight(0.01);
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
        int countBuffer = 5;
        int vals[countBuffer];
        for(int i = 0; i < countBuffer; i++) {
          int FLdistance = sensorFL.distance() * 100;
          int Fdistance = sensorF.distance() * 100;
          vals[i] = FLdistance - Fdistance;
        }
        int error = findMedian(vals, countBuffer);

        if (error > ROT_L_UPPER) // Rotate Left
        {
            rotateRight(0.01);
        }
        else if (error < ROT_L_LOWER) // Ro    tate Right
        {
            rotateLeft(0.01);
        }
        else
        {
          int FLdistance = sensorFL.distance() * 100;
          int Fdistance = sensorF.distance() * 100;
          int error = FLdistance - Fdistance;
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
        int countBuffer = 5;
        int vals[countBuffer];
        for(int i = 0; i < countBuffer; i++) {
          int FLdistance = sensorFL.distance() * 100;
          int FRdistance = sensorFR.distance() * 100;
          vals[i] = FRdistance - FLdistance;
        }
        int error = findMedian(vals, countBuffer);

        if (error > ROT_M_UPPER) // Rotate Left
        {
            rotateLeft(0.01);
        }
        else if (error < ROT_M_LOWER) // Rotate Right
        {
            rotateRight(0.01);
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

    float FRdistance = sensorFL.distance();

    if (calibrateDistance && (FRdistance > 15 || FRdistance < 15)) // TODO
        calibrateDistanceR(2, false);
}

void calibrateFrontAngleMR(bool calibrateDistance) // ID = 3
{
    int count = 0;

    while (1)
    {
        int countBuffer = 5;
        int vals[countBuffer];
        for(int i = 0; i < countBuffer; i++) {
          int Fdistance = sensorF.distance() * 100;
          int FRdistance = sensorFR.distance() * 100;
          vals[i] = Fdistance - FRdistance;
        }
        int error = findMedian(vals, countBuffer);

        if (error > ROT_R_UPPER) // Rotate Left
        {
            rotateRight(0.01);
        }
        else if (error < ROT_R_LOWER) // Rotate Right
        {
            rotateLeft(0.01);
        }
        else
        {
            int Fdistance = sensorF.distance() * 100;
            int FRdistance = sensorFR.distance() * 100;
            int error = Fdistance - FRdistance;
            if (error > ROT_R_LOWER && error < ROT_R_UPPER)
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
            moveForward(0.001);
        }
        else if (LFdistance < DIST_L_LOWER)
        {
            moveBackward(0.001);
        }

        else
        {
          int LFdistance = sensorFL.distance() * 100;
          if(LFdistance > DIST_L_LOWER && LFdistance < DIST_L_UPPER){
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
        int Fdistance = sensorF.distance() * 100;

        if (Fdistance > DIST_M_UPPER)
        {
            moveForward(0.001);
        }
        else if (Fdistance < DIST_M_LOWER)
        {
            moveBackward(0.001);
        }
        else
        {
            int Fdistance = sensorF.distance() * 100;
            if(Fdistance > DIST_M_LOWER && Fdistance < DIST_M_UPPER){
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
        int RFdistance = sensorFR.distance() * 100;

        if (RFdistance > DIST_R_UPPER)
        {
            moveForward(0.001);
        }
        else if (RFdistance < DIST_R_LOWER)
        {
            moveBackward(0.001);
        }
        else
        {
            int RFdistance = sensorFR.distance() * 100;
            if(RFdistance > DIST_R_LOWER && RFdistance < DIST_R_UPPER){
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
