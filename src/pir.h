#pragma once
#include <Arduino.h>

enum PirClass {
    PIR_NONE,
    PIR_LEFT,
    PIR_LEFT_FRONT,
    PIR_FRONT,
    PIR_FRONT_RIGHT,
    PIR_RIGHT,
    PIR_BACK,
    PIR_ALL
};

// Returned by update()
struct PirResult {
    PirClass left;
    PirClass front;
    PirClass right;
};

class Pir {
public:
    Pir(int pin1, int pin2, int pin3);

    void begin();
    PirClass update();

    void troubleshoot(unsigned long duration = 10000);  // Run for 10 seconds by default
    void printRawValues();                              // Print current pin states
    bool testIndividualSensor(int sensorNum);  

private:
    int _p1, _p2, _p3;

    int pir1State;
    int pir2State;
    int pir3State;

    PirClass handlePir1(int pin1Value, int pin2Value, int pin3Value);
    PirClass handlePir2(int pin1Value, int pin2Value, int pin3Value);
    PirClass handlePir3(int pin1Value, int pin2Value, int pin3Value);

    
};
