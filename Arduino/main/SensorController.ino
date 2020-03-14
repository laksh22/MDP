#include "SharpIR.h"
#include "ArduinoSort.h"

#define RBPIN A0 // PS1
#define LPIN A1  // PS2
#define RFPIN A2 // PS3
#define FPIN A3  // PS4
#define FLPIN A4 // PS5
#define FRPIN A5 // PS6

#define SRmodel 1080
#define LRmodel 20150

SharpIR sensorRB = SharpIR(RBPIN, SRmodel);
SharpIR sensorL = SharpIR(LPIN, LRmodel);
SharpIR sensorRF = SharpIR(RFPIN, SRmodel);
SharpIR sensorF = SharpIR(FPIN, SRmodel);
SharpIR sensorFL = SharpIR(FLPIN, SRmodel);
SharpIR sensorFR = SharpIR(FRPIN, SRmodel);

void printSensors(bool grids, int buffer)
{
    if (grids)
    {
        Serial.print("F: ");
        Serial.print(gridsF(buffer));
        Serial.print(", FL: ");
        Serial.print(gridsFL(buffer));
        Serial.print(", FR: ");
        Serial.print(gridsFR(buffer));
        Serial.print(", RF: ");
        Serial.print(gridsRF(buffer));
        Serial.print("RB: ");
        Serial.print(gridsRB(buffer));
        Serial.print(", L: ");
        Serial.println(gridsL(buffer));
    }
    else
    {
        Serial.print("F: ");
        Serial.print(getSensorF(buffer));
        Serial.print(", FL: ");
        Serial.print(getSensorFL(buffer));
        Serial.print(", FR: ");
        Serial.print(getSensorFR(buffer));
        Serial.print(", RF: ");
        Serial.print(getSensorRF(buffer));
        Serial.print(", RB: ");
        Serial.print(getSensorRB(buffer));
        Serial.print(", L: ");
        Serial.println(getSensorL(buffer));
    }
}

void sendSensors(char source, int buffer)
{
    Serial.print("@");
    Serial.print(source);
    Serial.print(gridsL(buffer));
    Serial.print(":");
    Serial.print(gridsRF(buffer));
    Serial.print(":");
    Serial.print(gridsF(buffer));
    Serial.print(":");
    Serial.print(gridsFL(buffer));
    Serial.print(":");
    Serial.print(gridsFR(buffer));
    Serial.println("!");
    Serial.flush();
}

float findMedian(float a[], int n)
{
    sortArray(a, n);
    if (n % 2 != 0)
    {
        return a[n / 2];
    }
    return (a[(n - 1) / 2] + a[n / 2]) / 2.0;
}

float getSensorRB(int buffer)
{
    float sensorValues[buffer];
    for (int i = 0; i < buffer; i++)
    {
        sensorValues[i] = sensorRB.distance();
    }
    return findMedian(sensorValues, buffer);
}

float getSensorL(int buffer)
{
    float sensorValues[buffer];
    for (int i = 0; i < buffer; i++)
    {
        sensorValues[i] = sensorL.distance();
    }
    return findMedian(sensorValues, buffer);
}

float getSensorRF(int buffer)
{
    float sensorValues[buffer];
    for (int i = 0; i < buffer; i++)
    {
        sensorValues[i] = sensorRF.distance();
    }
    return findMedian(sensorValues, buffer);
}

float getSensorF(int buffer)
{
    float sensorValues[buffer];
    for (int i = 0; i < buffer; i++)
    {
        sensorValues[i] = sensorF.distance();
    }
    return findMedian(sensorValues, buffer);
}

float getSensorFL(int buffer)
{
    float sensorValues[buffer];
    for (int i = 0; i < buffer; i++)
    {
        sensorValues[i] = sensorFL.distance();
    }
    return findMedian(sensorValues, buffer);
}

float getSensorFR(int buffer)
{
    float sensorValues[buffer];
    for (int i = 0; i < buffer; i++)
    {
        sensorValues[i] = sensorFR.distance();
    }
    return findMedian(sensorValues, buffer);
}

int gridsRB(int buffer)
{
    int dis = getSensorRB(buffer);

    if (dis <= 21)
        return 1;

    else if (dis > 21 && dis <= 33)
        return 2;

    else
        return 3;
}

int gridsL(int buffer)
{
    int dis = getSensorL(buffer);
    if (dis <= 19)
        return 1;

    else if (dis > 19 && dis <= 25)
        return 2;

    else if (dis > 25 && dis <= 34)
        return 3;

    else if (dis > 34 && dis <= 44)
        return 4;

    else if (dis > 44 && dis <= 57)
        return 5;

    else
        return 6;
}

int gridsRF(int buffer)
{
    int dis = getSensorRF(buffer);

    if (dis <= 19)
        return 1;

    else if (dis > 19 && dis <= 29)
        return 2;

    else
        return 3;
}

int gridsF(int buffer)
{
    int dis = getSensorF(buffer);

    if (dis <= 15)
        return 1;

    else if (dis > 15 && dis <= 26)
        return 2;

    else
        return 3;
}

int gridsFL(int buffer)
{
    int dis = getSensorFL(buffer);

    if (dis <= 18.38)
        return 1;

    else if (dis > 18.38 && dis <= 30.94)
        return 2;

    else
        return 3;
}

int gridsFR(int buffer)
{
    int dis = getSensorFR(buffer);

    if (dis <= 18.30)
        return 1;

    else if (dis > 18.30 && dis <= 32)
        return 2;

    else
        return 3;
}
