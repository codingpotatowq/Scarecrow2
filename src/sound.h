#ifndef SOUND_H
#define SOUND_H

#include <Arduino.h>

// Frequency range classification
enum FrequencyRange {
    FREQ_NONE,      
    FREQ_LOW,       
    FREQ_MID_LOW,   
    FREQ_MID,       
    FREQ_MID_HIGH,  
    FREQ_HIGH      
};

class LM386 {
public:
    LM386(int pin, int loudThreshold = 70);  // CHANGE DB
    void begin();
    
    // Get raw peak-to-peak amplitude (0-1023)
    int getLevel();
    
    // Get sound level in decibels (approximate)
    float getDecibels();
    
    // Check if sound exceeds the loud threshold
    bool detect();
    bool isLoud();  // Alias for detect()
    
    // Get dominant frequency in Hz
    float getFrequency();
    
    // Get frequency range classification
    FrequencyRange getFrequencyRange();

private:
    int micPin;
    int loudThresholdDb;  // Threshold in dB for "loud" detection
};

#endif