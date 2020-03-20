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
        Serial.print(rb);
        Serial.print(", L: ");
        Serial.println(l);
    } else 
    {
        int LVals[5], MVals[5], RVals[5];
        for(int i = 0; i < 5; i++){
          int FLdistance = sensorFL.distance() * 100;
          int Fdistance = sensorF.distance() * 100;
          LVals[i] = FLdistance - Fdistance;

          FLdistance = sensorFL.distance() * 100;
          int FRdistance = sensorFR.distance() * 100;
          MVals[i] = FRdistance - FLdistance;

          Fdistance = sensorF.distance() * 100;
          FRdistance = sensorFR.distance() * 100;
          RVals[i] = Fdistance - FRdistance;
        }
        
        int error1 = findMedian(LVals, 5);
        int error2 = findMedian(MVals, 5);
        int error3 = findMedian(RVals, 5);

        Serial.print("LM: ");
        Serial.print(error1);
        Serial.print(", LR: ");
        Serial.print(error2);
        Serial.print(", MR: ");
        Serial.println(error3);
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
    int buffer = 15;
    int sensorVals[buffer];
    for(int i = 0; i < buffer; i++){
      sensorVals[i] = sensorRB.distance();
    }
    int dis = findMedian(sensorVals, buffer);

    if (dis <= 20)
        return 1;

    else if (dis > 20 && dis <= 34)
        return 2;

    else
        return 3;
}

int gridsL()
{
    int vals[5];
    for(int i = 0; i < 5; i++){
      vals[i] = sensorL.distance();
    }
    int dis = findMedian(vals, 5);
    
    if (dis <= 19)
        return 1;

    else if (dis > 19 && dis <= 25)
        return 2;

    else if (dis > 25 && dis <= 36)
        return 3;

    else if (dis > 36 && dis <= 47)
        return 4;

    else if (dis > 47 && dis <= 59)
        return 5;

    else
        return 6;
}

int gridsRF()
{
    int vals[5];
    for(int i = 0; i < 5; i++){
      vals[i] = sensorRF.distance();
    }
    int dis = findMedian(vals, 5);

    if (dis <= 18)
        return 1;

    else if (dis > 18 && dis <= 30)
        return 2;

    else
        return 3;
}

int gridsF()
{
    int vals[5];
    for(int i = 0; i < 5; i++){
      vals[i] = sensorF.distance();
    }
    int dis = findMedian(vals, 5);

    if (dis <= 16)
        return 1;

    else if (dis > 16 && dis <= 29)
        return 2;

    else
        return 3;
}

int gridsFL()
{
    int vals[5];
    for(int i = 0; i < 5; i++){
      vals[i] = sensorFL.distance();
    }
    int dis = findMedian(vals, 5);

    if (dis <= 17)
        return 1;

    else if (dis > 17 && dis <= 30)
        return 2;

    else
        return 3;
}

int gridsFR()
{
    int vals[5];
    for(int i = 0; i < 5; i++){
      vals[i] = sensorFR.distance();
    }
    int dis = findMedian(vals, 5);

    if (dis <= 15)
        return 1;

    else if (dis > 15 && dis <= 27)
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
