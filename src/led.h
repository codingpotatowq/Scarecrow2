#ifndef LED_H
#define LED_H

#include <stdint.h>

class led {
public:
    // RGB color structure
    struct Color {
        uint8_t r;
        uint8_t b;
        
        Color(uint8_t red = 0, uint8_t blue = 0) 
            : r(red), b(blue) {}
    };

    // Strobe pattern modes
    enum class Pattern {
        SINGLE_FLASH,      // Single flash on/off
        DOUBLE_FLASH,      // Two quick flashes
        TRIPLE_FLASH,      // Three quick flashes
        POLICE,            // Red/Blue alternating
        PARTY,
        RAPID_STROBE              // Multi-color rapid changes
    };

    // Constructor
    led(uint8_t redPin, uint8_t bluePin);
    
    // Core functions
    void begin();
    void update();  // Call this in main loop
    
    // Configuration
    void setColor(const Color& color);
    void setPattern(Pattern pattern);
    void setSpeed(uint16_t flashDurationMs);
    void setIntensity(uint8_t intensity);  // 0-100%
    void enable(bool state);
    void shufflePattern();
    
    // Getters
    bool isEnabled() const { return enabled; }
    Pattern getPattern() const { return currentPattern; }
    Color getCurrentColor() const { return currentColor; }

private:
    // Pin assignments
    uint8_t pinR, pinB;
    
    // State variables
    bool enabled;
    Color currentColor;
    Pattern currentPattern;
    Pattern lastPattern;
    uint16_t flashDuration;
    uint8_t intensity;
    
    // Timing variables
    unsigned long lastUpdateTime;
    uint8_t flashState;
    uint8_t pulseValue;
    bool pulseDirection;
    
    // Internal methods
    void setRGB(uint8_t r, uint8_t b);
    void executePattern();
    void singleFlash();
    void doubleFlash();
    void tripleFlash();
    void policeEffect();
    void partyEffect();
    void rapidStrobe();
    
    // Helper functions
    uint8_t scaleIntensity(uint8_t value);
    void allOff();
};

#endif // STROBE_LIGHT_H