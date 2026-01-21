#include <Arduino.h>
#include <classifier.h>

SoundClassifier::SoundClassifier(Ultrasonic& frontDist,
                                 LM386fft& mic,
                                 Pir& Pir_1,
                                 Pir& Pir_2,
                                 Pir& Pir_3)
    : frontUS(frontDist),
      mainMic(mic),
      pir_1(Pir_1),
      pir_2(Pir_2),
      pir_3(Pir_3)

{
}

SoundEvent SoundClassifier::evaluate() {
    SoundType soundDetected = mainMic.classifySound();
    
    if (soundDetected == SOUND_NONE) {
        return BACKGROUND;
    }

    if (soundDetected == SOUND_HUMAN) return HUMAN_FRONT;

    if (soundDetected == SOUND_CROW) return CROW_FRONT;

    // --- DISTANCE MEASUREMENTS ---
    float dFront = frontUS.getDistance();   

    bool frontClose = (dFront < 0.5);
    bool frontFar   = (dFront >= 0.5);

    if (frontFar) { 
        return TOO_FAR;
        }

    // --- SOUND MEASUREMENTS ---

    // --- PIR ACTIVATION ---
    PirClass handle_1 = pir_1.update();
    PirClass handle_2 = pir_2.update();
    PirClass handle_3 = pir_3.update();
    
    // PIR CLASSIFICATION
    // FRONT
    if (handle_2 == PIR_FRONT || handle_2 == PIR_LEFT_FRONT || handle_2 == PIR_FRONT_RIGHT) {
        if (soundDetected == SOUND_HUMAN && frontClose) return HUMAN_FRONT;
        if (soundDetected == SOUND_CROW && frontClose) return CROW_FRONT;
        }

    // BACK
    if (handle_3 == PIR_BACK || handle_1 == PIR_BACK) {
        if (soundDetected == SOUND_HUMAN) return HUMAN_BACK;
        if (soundDetected == SOUND_CROW) return CROW_BACK;
        }

    // LEFT
    if (handle_1 == PIR_LEFT || handle_1 == PIR_LEFT_FRONT) {
        if (soundDetected == SOUND_HUMAN && frontClose) return HUMAN_LEFT;
        if (soundDetected == SOUND_CROW && frontClose) return CROW_LEFT;
        }

    // RIGHT
    if (handle_3 == PIR_RIGHT || handle_3 == PIR_FRONT_RIGHT) {
        if (soundDetected == SOUND_HUMAN && frontClose) return HUMAN_RIGHT;
        if (soundDetected == SOUND_CROW && frontClose) return CROW_RIGHT;
        }

    if (handle_1 == PIR_ALL || handle_2 == PIR_ALL || handle_3 == PIR_ALL) {
        if (soundDetected == SOUND_HUMAN && frontClose) return HUMAN_ALL;
        if (soundDetected == SOUND_CROW && frontClose) return CROW_ALL;
    }
    return NONE;
}
