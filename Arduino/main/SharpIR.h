/*
	SharpIR
	Arduino library for retrieving distance (in cm) from the analog GP2Y0A21Y and GP2Y0A02YK
	From an original version of Dr. Marcal Casas-Cartagena (marcal.casas@gmail.com)     
	
    Version : 1.0 : Guillaume Rico
	https://github.com/guillaume-rico/SharpIR
*/

#ifndef SharpIR_h
#define SharpIR_h

#define NB_SAMPLE 31

#ifdef ARDUINO
#include "Arduino.h"
#elif defined(SPARK)
#include "Particle.h"
#endif

class SharpIR
{
public:
    SharpIR(int irPin, int sensorModel);
    double distance();
    void sort(double a[], int size);

private:
    int _irPin;
    int _model;
};

#endif
