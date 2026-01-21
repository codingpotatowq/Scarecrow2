#include "pir.h"

Pir :: Pir(int pin1, int pin2, int pin3)
    : _p1(pin1), _p2(pin2), _p3(pin3), pir1State(LOW), pir2State(LOW), pir3State(LOW) {}

//setup
void Pir::begin() {
    pinMode(_p1, INPUT);
    pinMode(_p2, INPUT);
    pinMode(_p3, INPUT);
}

PirClass Pir::update() {
    int pin1Value = digitalRead(_p1);
    int pin2Value = digitalRead(_p2);
    int pin3Value = digitalRead(_p3);

    PirClass e;

    e  = handlePir1(pin1Value, pin2Value, pin3Value);
    if (e != PIR_NONE) return e;
    e = handlePir2(pin1Value, pin2Value, pin3Value);
    if (e != PIR_NONE) return e;
    e= handlePir3(pin1Value, pin2Value, pin3Value);
    if (e != PIR_NONE) return e;

    return PIR_NONE;
}



//left PIR
PirClass Pir::handlePir1(int pin1Value, int pin2Value, int pin3Value) {
    if (pin1Value == HIGH && pir1State == LOW) {

        if (pin2Value == HIGH && pin3Value == HIGH) {
            Serial.println("Motion detected from all sides");
            return PIR_ALL;
        }
        else if (pin2Value == HIGH) {
            Serial.println("Motion detected from left and front");
            return PIR_LEFT_FRONT;
        }
        else if (pin3Value == HIGH) {
            Serial.println("Motion detected from back");
            return PIR_BACK;
        }
        else {
            Serial.println("Motion detected from left");
            return PIR_LEFT;
        }

        pir1State = HIGH;
    }
    else if (pin1Value == LOW && pir1State == HIGH) {
        pir1State = LOW;
    }

    return PIR_NONE;
}

//middle PIR
PirClass Pir::handlePir2(int pin1Value, int pin2Value, int pin3Value) {
    if (pin2Value == HIGH && pir2State == LOW) {

        if (pin1Value == HIGH && pin3Value == HIGH) {
            Serial.println("Motion detected from all sides");
            return PIR_ALL;
        }
        else if (pin3Value == HIGH) {
            Serial.println("Motion detected from front and right");
            return PIR_FRONT_RIGHT;
        }
        else if (pin1Value == HIGH) {
            Serial.println("Motion detected from front and left");
            return PIR_LEFT_FRONT;
        }
        else {
            Serial.println("Motion detected from front");
            return PIR_FRONT;
        }

        pir2State = HIGH;
    }
    else if (pin2Value == LOW && pir2State == HIGH) {
        pir2State = LOW;
    }
    return PIR_NONE;
}

//right PIR
PirClass Pir::handlePir3(int pin1Value, int pin2Value, int pin3Value) {
    if (pin3Value == HIGH && pir3State == LOW) {

        if (pin1Value == HIGH && pin2Value == HIGH) {
            Serial.println("Motion detected from all sides");
            return PIR_ALL;
        }
        else if (pin1Value == HIGH) {
            Serial.println("Motion detected from back");
            return PIR_BACK;
        }
        else if (pin2Value == HIGH) {
            Serial.println("Motion detected from front and right");
            return PIR_FRONT_RIGHT;
        }
        else {            
            Serial.println("Motion detected from right"); 
            return PIR_RIGHT;
            }
        pir3State = HIGH;
    }
    else if (pin3Value == LOW && pir3State == HIGH) {
        pir3State = LOW;
    }
    return PIR_NONE;
}

void Pir::troubleshoot(unsigned long duration) {
    Serial.println("\n========================================");
    Serial.println("PIR TROUBLESHOOTING MODE");
    Serial.println("========================================");
    Serial.print("Testing for ");
    Serial.print(duration / 1000);
    Serial.println(" seconds...");
    Serial.println("Wave your hand in front of each sensor.");
    Serial.println();
    Serial.println("Sensor Layout:");
    Serial.println("  [PIR1-Left]  [PIR2-Front]  [PIR3-Right]");
    Serial.println();
    
    unsigned long startTime = millis();
    unsigned long lastPrintTime = 0;
    const unsigned long printInterval = 500; // Print every 500ms
    
    int pir1Count = 0, pir2Count = 0, pir3Count = 0;
    int pir1LastState = LOW, pir2LastState = LOW, pir3LastState = LOW;
    
    while (millis() - startTime < duration) {
        int pin1Value = digitalRead(_p1);
        int pin2Value = digitalRead(_p2);
        int pin3Value = digitalRead(_p3);
        
        // Count state changes (rising edge detections)
        if (pin1Value == HIGH && pir1LastState == LOW) pir1Count++;
        if (pin2Value == HIGH && pir2LastState == LOW) pir2Count++;
        if (pin3Value == HIGH && pir3LastState == LOW) pir3Count++;
        
        pir1LastState = pin1Value;
        pir2LastState = pin2Value;
        pir3LastState = pin3Value;
        
        // Print status periodically
        if (millis() - lastPrintTime >= printInterval) {
            Serial.print("PIR1(Left): ");
            Serial.print(pin1Value == HIGH ? "ACTIVE " : "idle   ");
            Serial.print(" | PIR2(Front): ");
            Serial.print(pin2Value == HIGH ? "ACTIVE " : "idle   ");
            Serial.print(" | PIR3(Right): ");
            Serial.print(pin3Value == HIGH ? "ACTIVE" : "idle  ");
            Serial.print(" | Triggers: [");
            Serial.print(pir1Count);
            Serial.print(",");
            Serial.print(pir2Count);
            Serial.print(",");
            Serial.print(pir3Count);
            Serial.println("]");
            
            lastPrintTime = millis();
        }
        
        delay(10); // Small delay to prevent overwhelming serial
    }
    
    
}