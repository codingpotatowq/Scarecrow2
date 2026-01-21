#ifndef SOUNDCLASSIFIER_H
#define SOUNDCLASSIFIER_H

#include <distance.h>
#include <pir.h>
#include <soundfft.h>

enum SoundEvent {
    NONE,

    HUMAN_FRONT,
    HUMAN_BACK,
    HUMAN_LEFT,
    HUMAN_RIGHT,
    HUMAN_ALL, 

    // BIRD CLASSIFICATION
    CROW_FRONT,
    CROW_BACK,
    CROW_LEFT,
    CROW_RIGHT,
    CROW_ALL,

    TOO_FAR,
    BACKGROUND,
};

class SoundClassifier {
public:
    SoundClassifier(Ultrasonic& frontUS,
                    LM386fft&  mainMic,
                    Pir& pir_1, Pir& pir_2, Pir& pir_3
                    );  
    SoundEvent evaluate(); 


private:
    Ultrasonic& frontUS;
    LM386fft& mainMic ;
    Pir& pir_1;
    Pir& pir_2;
    Pir& pir_3;
};

#endif
