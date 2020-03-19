#include "SharpIR.h"
#include "ArduinoSort.h"

#define RFPIN A0 // PS1
#define FPIN A1  // PS2
#define RBPIN A2 // PS3
#define FLPIN A3  // PS4
#define FRPIN A4 // PS5
#define LPIN A5 // PS6

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

void printSensors(int type)
{
    if (type == 1)
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
    else if (type == 2)
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
    } else if (type==3) 
    {
        int f = sensorF.distance() * 100;
        int fl = sensorFL.distance() * 100;
        int fr = sensorFR.distance() * 100;
        int rf = sensorRF.distance() * 100;
        int rb = sensorRB.distance() * 100;
        int l = sensorL.distance() * 100;
        Serial.print("F: ");
        Serial.print(f);
        Serial.print(", FL: ");
        Serial.print(fl);
        Serial.print(", FR: ");
        Serial.print(fr);
        Serial.print(", RF: ");
        Serial.print(rf);
        Serial.print(", RB: ");
        Serial.print(rb);
        Serial.print(", L: ");
        Serial.println(l);
    } else 
    {
        int RBdistance = sensorRB.distance() * 100;
        int RFdistance = sensorRF.distance() * 100;
        int error = RBdistance - RFdistance;
        Serial.println(error);
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

    if (dis <= 20)
        return 1;

    else if (dis > 20 && dis <= 34)
        return 2;

    else
        return 3;
}

int gridsL()
{
    int dis = sensorL.distance();
    if (dis <= 19)
        return 1;

    else if (dis > 19 && dis <= 26)
        return 2;

    else if (dis > 26 && dis <= 36)
        return 3;

    else if (dis > 36 && dis <= 46)
        return 4;

    else if (dis > 46 && dis <= 59)
        return 5;

    else
        return 6;
}

int gridsRF()
{
    int dis = sensorRF.distance();

    if (dis <= 18)
        return 1;

    else if (dis > 18 && dis <= 29)
        return 2;

    else
        return 3;
}

int gridsF()
{
    int buffer = 10;
    int sensorVals[buffer];
    for(int i = 0; i < buffer; i++){
      sensorVals[i] = sensorF.distance();
    }
    int dis = findMedian(sensorVals, buffer);

    if (dis <= 16)
        return 1;

    else if (dis > 16 && dis <= 28)
        return 2;

    else
        return 3;
}

int gridsFL()
{
    int dis = sensorFL.distance();

    if (dis <= 19)
        return 1;

    else if (dis > 19 && dis <= 30)
        return 2;

    else
        return 3;
}

int gridsFR()
{
    float dis = sensorFR.distance();

    if (dis <= 17)
        return 1;

    else if (dis > 17 && dis <= 27)
        return 2;

    else
        return 3;
}

int findMedian(int a[], int n)
{
  sortArray(a, n);
  if (n % 2 != 0)
  {
    return a[n / 2];
  }
  return (a[(n - 1) / 2] + a[n / 2]) / 2.0;
}
