#include "led.h"
#include <Arduino.h>

led::led(uint8_t redPin, uint8_t bluePin)
    : pinR(redPin), pinB(bluePin),
      enabled(false), currentColor(255, 255),
      currentPattern(), lastPattern(), flashDuration(10),
      intensity(100), lastUpdateTime(0), flashState(0),
      pulseValue(0), pulseDirection(true) {}

void led::begin() {
    pinMode(pinR, OUTPUT);
    pinMode(pinB, OUTPUT);
    allOff();
}

void led::update() {
    if (!enabled) {
        allOff();
        return;
    }
    
    executePattern();
}

void led::setColor(const Color& color) {
    currentColor = color;
}

void led::setPattern(Pattern pattern) {
    lastPattern = currentPattern;
    currentPattern = pattern;
    flashState = 0;
    pulseValue = 0;
    pulseDirection = true;
}

void led::setSpeed(uint16_t flashDurationMs) {
    flashDuration = flashDurationMs;
}

void led::setIntensity(uint8_t intensityPercent) {
    intensity = (intensityPercent > 100) ? 100 : intensityPercent;
}

void led::enable(bool state) {
    enabled = state;
    if (!enabled) {
        allOff();
    }
}

void led::setRGB(uint8_t r, uint8_t b) {
    analogWrite(pinR, scaleIntensity(r));
    analogWrite(pinB, scaleIntensity(b));
}

void led::executePattern() {
    switch (currentPattern) {
        case Pattern::SINGLE_FLASH:
            singleFlash();
            break;
        case Pattern::DOUBLE_FLASH:
            doubleFlash();
            break;
        case Pattern::TRIPLE_FLASH:
            tripleFlash();
            break;
        case Pattern::POLICE:
            policeEffect();
            break;
        case Pattern::PARTY:
            partyEffect();
            break;
        case Pattern::RAPID_STROBE:
            rapidStrobe();
            break;
    }
}

void led::singleFlash() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= flashDuration) {
        flashState = !flashState;
        
        if (flashState) {
            setRGB(currentColor.r, currentColor.b);
        } else {
            allOff();
        }
        
        lastUpdateTime = currentTime;
    }
}

void led::doubleFlash() {
    unsigned long currentTime = millis();
    uint16_t interval = flashDuration / 4;
    
    if (currentTime - lastUpdateTime >= interval) {
        flashState++;
        if (flashState > 7) flashState = 0;
        
        // Pattern: ON OFF ON OFF PAUSE PAUSE PAUSE PAUSE
        if (flashState == 0 || flashState == 2) {
            setRGB(currentColor.r, currentColor.b);
        } else {
            allOff();
        }
        
        lastUpdateTime = currentTime;
    }
}

void led::tripleFlash() {
    unsigned long currentTime = millis();
    uint16_t interval = flashDuration / 6;
    
    if (currentTime - lastUpdateTime >= interval) {
        flashState++;
        if (flashState > 11) flashState = 0;
        
        // Pattern: ON OFF ON OFF ON OFF PAUSE PAUSE PAUSE PAUSE PAUSE PAUSE
        if (flashState == 0 || flashState == 2 || flashState == 4) {
            setRGB(currentColor.r, currentColor.b);
        } else {
            allOff();
        }
        
        lastUpdateTime = currentTime;
    }
}


void led::policeEffect() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= flashDuration) {
        flashState = !flashState;
        
        if (flashState) {
            setRGB(255, 0);  // Red
        } else {
            setRGB(0, 255);  // Blue
        }
        
        lastUpdateTime = currentTime;
    }
}

void led::partyEffect() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= flashDuration / 2) {
        uint8_t colorChoice = rand() % 4;
        
        switch (colorChoice) {
            case 0: setRGB(255, 0); break;    // Red
            case 1: setRGB(0, 255); break;    // Blue
            case 2: setRGB(255, 255); break;  // Purple/Magenta
            case 3: setRGB(128, 128); break;  // Mixed
        }
        
        lastUpdateTime = currentTime;
    }
}

void led::rapidStrobe() {
    unsigned long currentTime = millis();
    
    // Very fast toggle - no pause between flashes
    if (currentTime - lastUpdateTime >= flashDuration) {
        flashState = !flashState;
        
        if (flashState) {
            setRGB(currentColor.r, currentColor.b);
        } else {
            allOff();
        }
        
        lastUpdateTime = currentTime;
    }
}

uint8_t led::scaleIntensity(uint8_t value) {
    return (value * intensity) / 100;
}

void led::allOff() {
    analogWrite(pinR, 0);
    analogWrite(pinB, 0);
}

void led:: shufflePattern() {
    Pattern patterns[] = {
        Pattern :: SINGLE_FLASH,
        Pattern :: DOUBLE_FLASH,
        Pattern :: TRIPLE_FLASH,
        Pattern :: POLICE,
        Pattern :: PARTY,
        Pattern :: RAPID_STROBE
    };

    int num = 6;
    Pattern newPattern;

    do{
        int randomIndex = rand()% num;
        newPattern = patterns[randomIndex];
    } while (newPattern == currentPattern);
    setPattern(newPattern);
}
