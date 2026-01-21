#include "distance.h"

Ultrasonic::Ultrasonic(int trigPin, int echoPin) {
    trig = trigPin;
    echo = echoPin;
}

void Ultrasonic::begin() {
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
}

long Ultrasonic::getDistance() {
    // Send trigger pulse
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    // Read echo time
    long duration = pulseIn(echo, HIGH, 30000); // timeout 30ms (~5m)

    // Convert to cm
    long distance = duration * 0.00034 / 2;

    return distance;
}
