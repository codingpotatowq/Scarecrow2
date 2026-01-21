#include "servoctr.h"

DualServoController::DualServoController(int leftPin, int rightPin)
    : leftPin(leftPin), rightPin(rightPin) {}

void DualServoController::begin() {
    leftServo.attach(leftPin);
    rightServo.attach(rightPin);
}

void DualServoController::activateLeft(unsigned long now) {
    isActive = true;
    startCycle(leftState);
    moveRandom(leftServo, leftState, now);
    
}

void DualServoController::activateRight(unsigned long now) {
    isActive = true;
    startCycle(rightState);
    moveRandom(rightServo, rightState, now);
}

void DualServoController::activateBoth() {
    isActive = true;
    startCycle(leftState);
    startCycle(rightState);
}

void DualServoController::stop() {
    isActive = false;
    leftServo.write(0);
    rightServo.write(0);
}

void DualServoController::startCycle(ServoState &state) {
    state.angle        = random(0, 45);
    state.activeDelay  = random(50, 500);
    state.neutralDelay = random(50, 500);
    state.firstRun = true;
}


void DualServoController::moveRandom(Servo &servo, ServoState &state, unsigned long now) {

    if (state.firstRun) {
        servo.write(state.angle);
        state.lastTime = now;
        state.firstRun = false;
        state.inActivePhase = true;
        return;
    }

    if (state.inActivePhase) {
        if (now - state.lastTime >= state.activeDelay) {
            servo.write(0);
            state.inActivePhase = false;
            state.lastTime = now;
        }
    } 
    else {
        if (now - state.lastTime >= state.neutralDelay) {
            startCycle(state);
            servo.write(state.angle);
            state.inActivePhase = true;
            state.lastTime = now;
        }
    }
}
