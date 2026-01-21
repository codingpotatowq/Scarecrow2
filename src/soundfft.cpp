#include "soundfft.h"
#include <math.h>

LM386fft::LM386fft(int pin, int loudThreshold) 
    : micPin(pin), fft(vReal, vImag, samples, samplingFreq), 
      noiseProfileInitialized(false) {}

void LM386fft::begin() {
    // Initialize FFT arrays
    for (int i = 0; i < samples; i++) {
        vReal[i] = 0.0;
        vImag[i] = 0.0;
    }
}

int LM386fft::getLevel() {
    unsigned long startMillis = millis();
    unsigned int signalMax = 0;
    unsigned int signalMin = 1023;
    const int sampleWindow = 50;

    while (millis() - startMillis < sampleWindow) {
        int sample = analogRead(micPin);
        if (sample > signalMax && sample < 1023) signalMax = sample;
        if (sample < signalMin && sample > 0) signalMin = sample;
    }

    return signalMax - signalMin;
}


// Collect audio samples with DC offset removal
void LM386fft::collectSamples() {
    // Collect raw samples
    for (int i = 0; i < samples; i++) {
        vReal[i] = analogRead(micPin);
        vImag[i] = 0.0;
        delayMicroseconds(1000000 / samplingFreq);
    }
    
    // Remove DC offset
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += vReal[i];
    }
    float dcOffset = sum / samples;
    
    for (int i = 0; i < samples; i++) {
        vReal[i] -= dcOffset;
    }
}

// Calculate RMS energy
float LM386fft::getRMSEnergy() {
    collectSamples();
    
    float sumSquares = 0;
    for (int i = 0; i < samples; i++) {
        sumSquares += vReal[i] * vReal[i];
    }
    
    return sqrt(sumSquares / samples);
}

// Calculate Zero Crossing Rate
float LM386fft::getZeroCrossingRate() {
    int zeroCrossings = 0;
    
    for (int i = 1; i < samples; i++) {
        if ((vReal[i-1] < 0 && vReal[i] >= 0) || 
            (vReal[i-1] >= 0 && vReal[i] < 0)) {
            zeroCrossings++;
        }
    }
    
    // Normalize by number of samples
    return (float)zeroCrossings / samples;
}

// Perform FFT and get frequency domain dataf
void LM386fft::performFFT() {
    // Apply Hamming window to reduce spectral leakage
    fft.windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    
    // Compute FFT
    fft.compute(vReal, vImag, samples, FFT_FORWARD);
    
    // Convert to magnitude
    fft.complexToMagnitude(vReal, vImag, samples);
}

// Get dominant frequency (peak of FFT)
float LM386fft::getDominantFrequency() {
    collectSamples();
    performFFT();
    
    // Find peak in FFT (ignore DC component at index 0)
    int peakIndex = 1;
    float peakValue = vReal[1];
    
    for (int i = 2; i < samples / 2; i++) {
        if (vReal[i] > peakValue) {
            peakValue = vReal[i];
            peakIndex = i;
        }
    }
    
    // Convert bin index to frequency
    return (peakIndex * samplingFreq) / (float)samples;
}

// Calculate spectral centroid (center of mass of spectrum)
float LM386fft::getSpectralCentroid() {
    float weightedSum = 0;
    float magnitudeSum = 0;
    
    for (int i = 1; i < samples / 2; i++) {
        float frequency = (i * samplingFreq) / (float)samples;
        weightedSum += frequency * vReal[i];
        magnitudeSum += vReal[i];
    }
    
    if (magnitudeSum < 0.001) return 0.0;
    
    return weightedSum / magnitudeSum;
}

// Calculate spectral bandwidth (spread around centroid)
float LM386fft::getSpectralBandwidth() {
    float centroid = getSpectralCentroid();
    
    float weightedVariance = 0;
    float magnitudeSum = 0;
    
    for (int i = 1; i < samples / 2; i++) {
        float frequency = (i * samplingFreq) / (float)samples;
        float diff = frequency - centroid;
        weightedVariance += (diff * diff) * vReal[i];
        magnitudeSum += vReal[i];
    }
    
    if (magnitudeSum < 0.001) return 0.0;
    
    return sqrt(weightedVariance / magnitudeSum);
}

// Calculate harmonicity (ratio of harmonic peaks to noise)
float LM386fft::getHarmonicity() {
    float fundamental = getDominantFrequency();
    
    if (fundamental < 50.0) return 0.0;
    
    float harmonicEnergy = 0;
    float totalEnergy = 0;
    
    // Check for harmonic peaks (2f, 3f, 4f, 5f)
    for (int harmonic = 1; harmonic <= 5; harmonic++) {
        float targetFreq = fundamental * harmonic;
        int targetBin = (targetFreq * samples) / samplingFreq;
        
        if (targetBin >= samples / 2) break;
        
        // Look in a window around the expected harmonic
        int window = 3;
        float localMax = 0;
        
        for (int j = -window; j <= window; j++) {
            int bin = targetBin + j;
            if (bin > 0 && bin < samples / 2) {
                if (vReal[bin] > localMax) {
                    localMax = vReal[bin];
                }
            }
        }
        
        harmonicEnergy += localMax;
    }
    
    // Total spectral energy
    for (int i = 1; i < samples / 2; i++) {
        totalEnergy += vReal[i];
    }
    
    if (totalEnergy < 0.001) return 0.0;
    
    return harmonicEnergy / totalEnergy;
}

// Get all audio features at once
AudioFeatures LM386fft::getAudioFeatures() {
    AudioFeatures features;
    
    collectSamples();
    
    // Time domain features
    features.rmsEnergy = getRMSEnergy();
    features.zcr = getZeroCrossingRate();
    features.peakToPeak = getLevel();
    
    // Frequency domain features
    performFFT();
    features.dominantFreq = getDominantFrequency();
    features.spectralCentroid = getSpectralCentroid();
    features.spectralBandwidth = getSpectralBandwidth();
    features.harmonicity = getHarmonicity();
    
    return features;
}

void printAudioFeatures(const AudioFeatures& f) {
    Serial.print("RMS="); Serial.print(f.rmsEnergy);
    Serial.print(" ZCR="); Serial.print(f.zcr);
    Serial.print(" P2P="); Serial.print(f.peakToPeak);
    Serial.print(" DomFreq="); Serial.print(f.dominantFreq);
    Serial.print(" Centroid="); Serial.print(f.spectralCentroid);
    Serial.print(" BW="); Serial.print(f.spectralBandwidth);
    Serial.print(" Harm="); Serial.println(f.harmonicity);
}

void LM386fft::initializeNoiseProfile() {
    // Based on actual background noise data analysis
    // Key differentiator: ZCR is very low for background noise (0.02-0.11)
    // while CROW has high ZCR (0.50-0.70)
    
    noiseProfile.rmsMin = 0.0;
    noiseProfile.rmsMax = 200.0;  // Most background is 100-185, margin for safety
    
    noiseProfile.zcrMin = 0.0;
    noiseProfile.zcrMax = 0.30;   // Background max ~0.25, well below CROW min (0.50)
    
    noiseProfile.p2pMin = 740.0;  // Background is consistently 743-762
    noiseProfile.p2pMax = 765.0;  // Slightly wider range for safety
    
    noiseProfile.centroidMin = 0.0;
    noiseProfile.centroidMax = 280.0;  // Background max ~155, below HUMAN min (300)
    
    noiseProfile.bwMin = 150.0;   // Background is mostly 175-320
    noiseProfile.bwMax = 320.0;   
    
    noiseProfile.harmMin = 0.0;
    noiseProfile.harmMax = 2.5;   // Background shows wide harmonicity range
    
    noiseProfileInitialized = true;
}

bool LM386fft::isBackgroundNoise(const AudioFeatures& f) {
    if (!noiseProfileInitialized) {
        initializeNoiseProfile();
    }
    
    // === PRIMARY DISTINGUISHING FEATURES ===
    
    // 1. ZCR is THE KEY DIFFERENTIATOR
    // Background: 0.02-0.11 (very stable, low)
    // Human voice: 0.14-0.25 (elevated, dynamic)
    if (f.zcr > 0.13) {
        return false;  // Human voice or other active sound
    }
    
    // 2. P2P at sensor limit suggests strong signal (voice/animal)
    // Background: 754-756 (very tight range), Human: 767-771 (at sensor limit)
    if (f.peakToPeak > 757) {
        return false;  // Strong signal approaching sensor limit
    }
    
    // 3. Negative dominant frequency indicates clipping/distortion (loud voice)
    if (f.dominantFreq < 0) {
        return false;  // Signal clipping, definitely not background
    }
    
    // 4. High RMS indicates strong sound energy
    // Background: 20-205, Human: 220-376
    if (f.rmsEnergy > 210) {
        return false;  // High energy signal
    }
    
    // === POSITIVE BACKGROUND NOISE IDENTIFICATION ===
    // All of these must match for confident background classification
    
    bool zcrMatch = (f.zcr >= 0.02 && f.zcr <= 0.12);      // Very low ZCR
    bool p2pMatch = (f.peakToPeak >= 754 && f.peakToPeak <= 756);  // Tight range
    bool rmsMatch = (f.rmsEnergy >= 15 && f.rmsEnergy <= 205);     // Low-moderate energy
    bool domFreqPositive = (f.dominantFreq > 0 && f.dominantFreq <= 500); // Stable, positive
    
    // Require all criteria for background noise
    return (zcrMatch && p2pMatch && rmsMatch && domFreqPositive);
}

float LM386fft::getNoiseConfidence(const AudioFeatures& f) {
    if (!noiseProfileInitialized) {
        initializeNoiseProfile();
    }
    
    // Calculate a confidence score (0.0 = definitely not noise, 1.0 = definitely noise)
    float confidence = 0.0;
    int matches = 0;
    int total = 0;
    
    // ZCR check (most important)
    total++;
    if (f.zcr >= noiseProfile.zcrMin && f.zcr <= noiseProfile.zcrMax) {
        matches++;
        confidence += 0.3; // ZCR is weighted heavily
    }
    
    // P2P check
    total++;
    if (f.peakToPeak >= noiseProfile.p2pMin && f.peakToPeak <= noiseProfile.p2pMax) {
        matches++;
        confidence += 0.25;
    }
    
    // Centroid check
    total++;
    if (f.spectralCentroid >= noiseProfile.centroidMin && 
        f.spectralCentroid <= noiseProfile.centroidMax) {
        matches++;
        confidence += 0.25;
    }
    
    // RMS check
    total++;
    if (f.rmsEnergy >= noiseProfile.rmsMin && f.rmsEnergy <= noiseProfile.rmsMax) {
        matches++;
        confidence += 0.2;
    }
    
    // Cap at 1.0
    if (confidence > 1.0) confidence = 1.0;
    
    return confidence;
}


// Classify sound type based on features
SoundType LM386fft::classifySound() {
    AudioFeatures f = getAudioFeatures();
    printAudioFeatures(f);
    
    if (f.rmsEnergy < 10.0) {
        return SOUND_NONE;
    }
    
    if (isBackgroundNoise(f)) {
        float confidence = getNoiseConfidence(f);
        Serial.print(" [NOISE confidence=");
        Serial.print(confidence);
        Serial.println("]");
        return SOUND_NONE;  // Treat background noise as no sound
    }
    
    
    // HUMAN characteristics:
    if (f.zcr >= 0.14 && f.zcr <= 0.28 &&               // Moderate ZCR range
        f.peakToPeak > 740 &&                            // Has signal
        f.rmsEnergy > 60) {                              // Reasonable energy
        Serial.println(" [HUMAN]");
        return SOUND_HUMAN;
    }
    
    // CROW characteristics: 
    if (f.zcr > 0.28 && f.zcr < 0.65 &&                 // Very high ZCR (crow caw)
        f.dominantFreq > 100 && f.dominantFreq < 500 &&  // Crow frequency range
        f.peakToPeak > 740) {                            // Has reasonable signal
        Serial.println(" [CROW]");
        return SOUND_CROW;
    }   

    // Default to unknown
    return SOUND_UNKNOWN;
}

// Legacy compatibility methods
float LM386fft::getFrequency() {
    return getDominantFrequency();
}

