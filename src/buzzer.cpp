#include <buzzer.h>
#include <hawksound.h>

buzzer::buzzer(int pin)
    : _pin(pin) {}


void buzzer::begin() {
    pinMode(_pin, OUTPUT);
}
void buzzer::update(){
    step = 0;
    active = true;
    lastTime = millis();
    execute();
}


void buzzer::stop() {
    noTone(_pin);
    active = false;
}


void buzzer :: setPattern(Pattern pattern) {
    lastPattern = currentPattern;
    currentPattern = pattern;
}

void buzzer::execute() {
    switch (currentPattern) {
    case Pattern::ALARM_FIXED_1:
        alarmPattern1(now);
        break;
    case Pattern::ALARM_FIXED_2:
        alarmPattern2(now);
        break;
    case Pattern::ALARM_FIXED_3:
        alarmPattern3(now);
        break;
    case Pattern::ALARM_CROW:
        alarmPattern4(now);
        break;
    case Pattern::ALARM_UNIVERSAL:
        alarmPattern7(now);
        break;
    }
}

void buzzer::beep(int frequency, int duration) {
    tone(_pin, frequency, duration);
    delay(duration);
    noTone(_pin);
}


void buzzer::alarmPattern1(unsigned long now) {
    if (now - lastTime >= delayAmount) {
        lastTime = now;

        if (step >= 10) { stop(); return; }

        if (step % 2 == 0) {
            tone(_pin, (step % 4 == 0) ? 2800 : 3200, 200);
            delayAmount = 200;
        } else {
            noTone(_pin);
            delayAmount = 50;
        }

        step++;
    }
}

void buzzer::alarmPattern2(unsigned long now){
    if (now - lastTime >= delayAmount) {
        lastTime = now;

        if (step >= 10) { stop(); return; }

        if (step % 2 == 0) {
            tone(_pin, (step % 4 == 0) ? 400 : 1000, 50);
            delayAmount = 50;
        } else {
            noTone(_pin);
            delayAmount = 5;
        }

        step++;
    }
}

void buzzer::alarmPattern3(unsigned long now){
    if (now - lastTime >= delayAmount) {
        lastTime = now;

        if (step >= 10) { stop(); return; }

        if (step % 2 == 0) {
            tone(_pin, (step % 4 == 0) ? 1000 : 2200, 250);
            delayAmount = 250;
        } else {
            noTone(_pin);
            delayAmount = 20;
        }

        step++;
    }
}

void buzzer::alarmPattern4(unsigned long now){
    if (now - lastTime >= delayAmount) {
        lastTime = now;

        if (step >= 10) { stop(); return; }

        if (step % 2 == 0) {
            tone(_pin, random(2000, 3000), 300);
            delayAmount = 400;
        } else {
            noTone(_pin);
            delayAmount = random(100, 300);
        }

        step++;
    }

}

void buzzer::alarmPattern5(unsigned long now) {
    // Pigeon alarm call pattern
    tone(_pin, 1800, 400);
    delay(400);
    tone(_pin, 2200, 600);
    delay(600);
    tone(_pin, 2500, 400);
    delay(400);
    noTone(_pin);
}

void buzzer::alarmPattern6(unsigned long now) {
    // Blackbird alarm frequencies
    for (int i = 0; i < 6; i++) {
        tone(_pin, random(2000, 3500), 250);
        delay(300);
        noTone(_pin);
        delay(random(100, 400));
    }
}

void buzzer::alarmPattern7(unsigned long now) {
    // Universal bird deterrent
    if (now - lastTime >= delayAmount) {
        lastTime = now;

        if (step >= 10) { stop(); return; }

        if (step % 2 == 0) {
            tone(_pin, random(-500, 500), 400);
            delayAmount = 400;
        } else {
            noTone(_pin);
            delayAmount = random(200, 600);
        }

        step++;
    }
}

void buzzer::playMelody(const int* melody, const int* durations, int length) {
    for (int i = 0; i < length; i++) {
        int note = pgm_read_word(&melody[i]);
        int dur  = pgm_read_word(&durations[i]);

        if (note > 0) tone(_pin, note, dur);
        noTone(_pin);
    }
}

void buzzer:: shuffleSoundCrow(){
    Pattern patterns[] = {
        Pattern :: ALARM_FIXED_1,
        Pattern :: ALARM_FIXED_2,
        Pattern :: ALARM_FIXED_3,
        Pattern :: ALARM_CROW,
        Pattern :: ALARM_UNIVERSAL,
        /*Pattern :: ALARM_PIGEON,
        Pattern :: ALARM_BLACKBIRD,*/
    };

    int num = 5;
    Pattern newPattern;

    do{
        int randomIndex = rand()% num;
        newPattern = patterns[randomIndex];
    } while (newPattern == currentPattern);
    setPattern(newPattern);

}

