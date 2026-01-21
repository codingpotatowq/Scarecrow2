#ifndef SERVOCTR_H
#define SERVOCTR_H

#include <Arduino.h>
#include <Servo.h>

class DualServoController {
public:
    DualServoController(int leftPin, int rightPin);

    void begin();

    void activateRight(unsigned long now);

    void activateLeft(unsigned long now);

    void activateBoth();

    void stop();

private:
    struct ServoState {
        int angle = 0;
        int activeDelay = 0;
        int neutralDelay = 0;
        unsigned long lastTime = 0;
        bool inActivePhase = true;
        bool firstRun = true;
    };

    void startCycle(ServoState &state);
    void moveRandom(Servo &servo, ServoState &state, unsigned long now);

    Servo leftServo, rightServo;
    ServoState leftState, rightState;

    int leftPin, rightPin;
    bool isActive = false;

};

#endif
