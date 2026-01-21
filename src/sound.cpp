#include "sound.h"

LM386::LM386(int pin, int loudThreshold) {
    micPin = pin;
    loudThresholdDb = loudThreshold; 
}

void LM386::begin() {
    // nothing needed for now
}

int LM386::getLevel() {
    unsigned long startMillis = millis();
    unsigned int signalMax = 0;
    unsigned int signalMin = 1023;
    const int sampleWindow = 50; // ms duration

    while (millis() - startMillis < sampleWindow) {
        int sample = analogRead(micPin);

        if (sample > signalMax && sample < 1023) {
            signalMax = sample;
        }

        if (sample < signalMin && sample > 0) {
            signalMin = sample;
        }
    }

    int peakToPeak = signalMax - signalMin;
    return peakToPeak;
}

float LM386::getDecibels() {
    int peakToPeak = getLevel();
    
    // Convert peak-to-peak to voltage (assuming 5V reference)!!CHANGE
    float voltage = (peakToPeak * 5.0) / 1024.0;
    
    // Convert to dB SPL (approximate calibration) !!CHANGE
    // This formula may need adjustment based on your specific mic/amp setup
    // Typical range: 40-90 dB for common environments
    float db = 20.0 * log10(voltage / 0.00631) + 50.0;  // 0.00631V reference
    
    // Clamp to reasonable range !!CHANGE
    if (db < 30.0) db = 30.0;
    if (db > 120.0) db = 120.0;
    
    return db;
}

bool LM386::detect() {
    float currentDb = getDecibels();
    return (currentDb >= loudThresholdDb);
}

bool LM386::isLoud() {
    return detect();
}

float LM386::getFrequency() {
    const int sampleSize = 128;  // Power of 2 for better performance
    const int sampleRate = 10000; // 10 kHz sampling rate
    int samples[sampleSize];
    
    // Collect samples as fast as possible
    unsigned long startMicros = micros();
    for (int i = 0; i < sampleSize; i++) {
        samples[i] = analogRead(micPin);
        delayMicroseconds(1000000 / sampleRate);  // Control sample rate
    }
    unsigned long endMicros = micros();
    
    // Calculate actual sample rate
    float actualSampleRate = (sampleSize * 1000000.0) / (endMicros - startMicros);
    
    // Remove DC offset
    long sum = 0;
    for (int i = 0; i < sampleSize; i++) {
        sum += samples[i];
    }
    int dcOffset = sum / sampleSize;
    
    for (int i = 0; i < sampleSize; i++) {
        samples[i] -= dcOffset;
    }
    
    // Zero crossing detection method (simple and effective)
    int zeroCrossings = 0;
    for (int i = 1; i < sampleSize; i++) {
        if ((samples[i-1] < 0 && samples[i] >= 0) || 
            (samples[i-1] >= 0 && samples[i] < 0)) {
            zeroCrossings++;
        }
    }
    
    // Calculate frequency
    // Each complete cycle has 2 zero crossings
    float frequency = (zeroCrossings * actualSampleRate) / (2.0 * sampleSize);
    
    // Filter out unrealistic frequencies
    if (frequency < 20.0 || frequency > 5000.0) {
        return 0.0;  // Outside typical audio range for this setup
    }
    
    return frequency;
}

FrequencyRange LM386::getFrequencyRange() {
    float freq = getFrequency();
    
    if (freq == 0.0) { // !!CHANGE
        return FREQ_NONE;
    } else if (freq < 150.0) {
        return FREQ_LOW;       // Bass/low rumble (20-150 Hz)
    } else if (freq < 500.0) {
        return FREQ_MID_LOW;   // Male voice range (150-500 Hz)
    } else if (freq < 2000.0) {
        return FREQ_MID;       // Female voice/general (500-2000 Hz)
    } else if (freq < 4000.0) {
        return FREQ_MID_HIGH;  // High voice/whistle (2000-4000 Hz)
    } else {
        return FREQ_HIGH;      // Very high pitch (4000+ Hz)
    }
}