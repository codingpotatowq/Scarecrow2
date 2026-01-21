#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

class Ultrasonic {
public:
    Ultrasonic(int trigPin, int echoPin);

    void begin();
    long getDistance(); // returns distance in cm

private:
    int trig;
    int echo;
};

#endif
