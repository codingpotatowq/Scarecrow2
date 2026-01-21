#define BUZZER_H
#ifdef BUZZER_H

#include <Arduino.h>
#include <avr/pgmspace.h>

class buzzer {
    public:
        buzzer(int pin);

        void begin();

        void update();

        void stop();

        void beep(int frequency, int duration);

        void playMelody(const int* melody, const int* durations, int length);

        void shuffleSoundCrow();

        void shuffleSoundPigeon();

        void shuffleSoundBlackbird();


        enum class Pattern {
        ALARM_FIXED_1,      
        ALARM_FIXED_2,      
        ALARM_FIXED_3,      
        ALARM_CROW,          
        /*ALARM_PIGEON,
        ALARM_BLACKBIRD,*/
        ALARM_UNIVERSAL
        };

        void setPattern(Pattern pattern);

    private:
        int _pin;

        void alarmPattern1(unsigned long now);

        void alarmPattern2(unsigned long now);

        void alarmPattern3(unsigned long now);

        void alarmPattern4(unsigned long now);

        void alarmPattern5(unsigned long now);

        void alarmPattern6(unsigned long now);

        void alarmPattern7(unsigned long now);

        void execute();

        Pattern lastPattern;

        Pattern currentPattern;

        uint8_t pattern = 0;

        uint8_t step = 0;

        bool active = false;

        unsigned long lastTime = 0;
        
        unsigned long delayAmount = 0;

        unsigned long now;

};

#endif 