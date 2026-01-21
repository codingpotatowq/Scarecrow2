#ifndef SOUNDFFT_H
#define SOUNDFFT_H

#include <Arduino.h>
#include <arduinoFFT.h>

// Enums for classification
enum SoundType {
    SOUND_NONE,      // No significant sound detected
    SOUND_HUMAN,     // Human voice detected
    SOUND_CROW,      // Crow
    SOUND_UNKNOWN    // Sound detected but cannot classify
};

// Structure to hold all audio features
struct AudioFeatures {
    float rmsEnergy;           // Root Mean Square energy (loudness)
    float zcr;                 // Zero Crossing Rate (0-1, normalized)
    float peakToPeak;          // Peak-to-peak amplitude (0-1023)
    float dominantFreq;        // Dominant frequency in Hz
    float spectralCentroid;    // Spectral centroid in Hz (brightness)
    float spectralBandwidth;   // Spectral bandwidth in Hz (spread)
    float harmonicity;         // Harmonicity ratio 0-1 (periodicity)
};

class LM386fft {
private:
    int micPin;
    
    // FFT configuration
    static const int samples = 64;        // Must be power of 2
    static const int samplingFreq = 8000;  // 8 kHz sampling rate
    
    // FFT data arrays
    float vReal[samples];
    float vImag[samples];
    
    // ArduinoFFT object
    ArduinoFFT<float> fft;
    
    // Private helper methods
    void collectSamples();
    void performFFT();

    struct NoiseProfile {
        float rmsMin, rmsMax;
        float zcrMin, zcrMax;
        float p2pMin, p2pMax;
        float centroidMin, centroidMax;
        float bwMin, bwMax;
        float harmMin, harmMax;
    };
    
    NoiseProfile noiseProfile;
    bool noiseProfileInitialized;
    
public:
    // Constructor
    LM386fft(int pin, int loudThreshold = 60);
    
    // Initialization
    void begin();
    
    // Basic measurements
    int getLevel();                    // Get peak-to-peak level (0-1023)
    float getDecibels();               // Get approximate decibel level
    bool detect();                     // Detect if sound exceeds threshold
    bool isLoud();                     // Alias for detect()
    
    // Legacy methods (for backward compatibility)
    float getFrequency();              // Get dominant frequency

    // Advanced feature extraction methods
    float getRMSEnergy();              // Calculate RMS energy
    float getZeroCrossingRate();       // Calculate zero crossing rate
    float getDominantFrequency();      // Get dominant frequency (FFT peak)
    float getSpectralCentroid();       // Calculate spectral centroid
    float getSpectralBandwidth();      // Calculate spectral bandwidth
    float getHarmonicity();            // Calculate harmonicity ratio
    
    // Complete analysis methods
    AudioFeatures getAudioFeatures();  // Get all features at once
    SoundType classifySound();         // Classify the sound type
    void initializeNoiseProfile();
    bool isBackgroundNoise(const AudioFeatures& f);
    float getNoiseConfidence(const AudioFeatures& f);

};

#endif // SOUND_H