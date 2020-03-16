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

void sensorInit() {
    for(int i = 0; i < 30; i++){
    sensorF.distance();
    sensorFL.distance();
    sensorFR.distance();
    sensorRF.distance();
    sensorRB.distance();
    sensorL.distance();
  }
}

void printSensors(bool grids)
{
    if (grids)
    {
        Serial.print("F: ");
        Serial.print(gridsF());
        Serial.print(", FL: ");
        Serial.print(gridsFL());
        Serial.print(", FR: ");
        Serial.print(gridsFR());
        Serial.print(", RF: ");
        Serial.print(gridsRF());
        Serial.print("RB: ");
        Serial.print(gridsRB());
        Serial.print(", L: ");
        Serial.println(gridsL());
    }
    else
    {
        Serial.print("F: ");
        Serial.print(sensorF.distance());
        Serial.print(", FL: ");
        Serial.print(sensorFL.distance());
        Serial.print(", FR: ");
        Serial.print(sensorFR.distance());
        Serial.print(", RF: ");
        Serial.print(sensorRF.distance());
        Serial.print(", RB: ");
        Serial.print(sensorRB.distance());
        Serial.print(", L: ");
        Serial.println(sensorL.distance());
    }
}

void sendSensors(char source)
{
    Serial.print("@");
    Serial.print(source);
    Serial.print(gridsL());
    Serial.print(":");
    Serial.print(gridsRF());
    Serial.print(":");
    Serial.print(gridsF());
    Serial.print(":");
    Serial.print(gridsFL());
    Serial.print(":");
    Serial.print(gridsFR());
    Serial.println("!");
    Serial.flush();
}

int gridsRB()
{
    int dis = sensorRB.distance();

    if (dis <= 21)
        return 1;

    else if (dis > 21 && dis <= 33)
        return 2;

    else
        return 3;
}

int gridsL()
{
    int dis = sensorL.distance();
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

int gridsRF()
{
    int dis = sensorRF.distance();

    if (dis <= 19)
        return 1;

    else if (dis > 19 && dis <= 29)
        return 2;

    else
        return 3;
}

int gridsF()
{
    int dis = sensorF.distance();

    if (dis <= 15)
        return 1;

    else if (dis > 15 && dis <= 26)
        return 2;

    else
        return 3;
}

int gridsFL()
{
    int dis = sensorFL.distance();

    if (dis <= 18.38)
        return 1;

    else if (dis > 18.38 && dis <= 30.94)
        return 2;

    else
        return 3;
}

int gridsFR()
{
    int dis = sensorFR.distance();

    if (dis <= 18.30)
        return 1;

    else if (dis > 18.30 && dis <= 32)
        return 2;

    else
        return 3;
}
