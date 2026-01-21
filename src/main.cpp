#include <Arduino.h>
#include <pir.h>
#include <soundfft.h>
#include <distance.h>
#include <classifier.h>
#include <servoctr.h>
#include <buzzer.h>
#include <hawksound.h>
#include <led.h>
// INPUT
Pir pir(2, 3, 4);
LM386fft mic(A5);
Ultrasonic frontSensor(6, 5);  // trig = red, echo = yellow

// OUTPUT
DualServoController servo(A3, A4);  //left = 2, right = 1
buzzer buzzer1(7);
buzzer buzzer2(8);
led led1(9, 11); //red = , blue = 
led led2(10, 12);


SoundClassifier classifier(
  frontSensor,
  mic,
  pir, pir, pir);

const char* eventToString(SoundEvent event) {
    switch(event) { 
        case NONE: return "NONE";

        case TOO_FAR: return "TOO_FAR";

        case HUMAN_ALL: return "HUMAN_ALL";
        case CROW_ALL: return "CROW_ALL";

        
        case HUMAN_FRONT: return "HUMAN_FRONT";
        case CROW_FRONT: return "CROW_FRONT";


        case HUMAN_BACK: return "HUMAN_BACK";
        case CROW_BACK: return "CROW_BACK";


        case HUMAN_LEFT: return "HUMAN_LEFT";
        case CROW_LEFT: return "CROW_LEFT";


        case HUMAN_RIGHT: return "HUMAN_RIGHT";
        case CROW_RIGHT: return "CROW_RIGHT";

        
        case BACKGROUND: return "BACKGROUND";
        default: return "UNKNOWN";
    }
}

void setup() {
  Serial.begin(9600);
  // INPUT SENSORS
    pir.begin();
    mic.begin();
    frontSensor.begin();
    
    // OUTPUT SENSORS
    buzzer1.begin();
    buzzer2.begin();

    led1.begin();
    led1.setPattern(led::Pattern::POLICE);
    led1.setSpeed(20); 
    led1.setIntensity(100);  
    led1.enable(true);

    led2.begin();
    led2.setPattern(led::Pattern::POLICE);
    led2.setSpeed(20); 
    led2.setIntensity(100); 
    led2.enable(true);
    
    servo.begin();

    Serial.println("Sound Classifier Initialized");
    Serial.println("-------------------------------------------------------------");
}


void loop() {
  
  // ------- TROUBLESHOOTING ---------------------------------
  //mic.classifySound();

  /*SoundEvent event = classifier.evaluate();
  if (event != NONE) {
    Serial.print("Event detected: ");
    Serial.println(eventToString(event));}*/

  //pir.troubleshoot(10000);

  
  // ------- CLASSIFIER --------------------------------------
  /*SoundEvent event = classifier.evaluate();
  if (event != NONE) {
    Serial.print("Event detected: ");
    Serial.println(eventToString(event));
    if (event == CROW_FRONT || CROW_BACK){
    
    static unsigned long lastPatternChange = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastPatternChange >= 1000) {
        buzzer1.shuffleSoundCrow(); 
        buzzer2.shuffleSoundCrow(); 
        led1.shufflePattern(); 
        led2.shufflePattern(); 
        
        lastPatternChange = currentTime;
      }
      buzzer1.update();
      buzzer2.update();
      led1.update();
      led2.update();
  
    unsigned long now = millis();
    servo.activateLeft(now);
    servo.activateRight(now);
    }
  } */

const unsigned long REACTION_DURATION_MS = 5000;

static unsigned long reactionEndTime = 0;

static int activeServoMask = 0;
const int SERVO_LEFT = 1;
const int SERVO_RIGHT = 2;
const int SERVO_ALL = 3; // 1 | 2

SoundEvent event = classifier.evaluate();
unsigned long currentTime = millis(); // Get time once per loop

if (event != NONE) {
  Serial.print("Event detected: ");
  Serial.println(eventToString(event));
  
  if (event == CROW_FRONT || event == CROW_BACK || event == CROW_ALL || event == CROW_LEFT || event == CROW_RIGHT) {
    reactionEndTime = currentTime + REACTION_DURATION_MS;
  }

  if (event == CROW_FRONT || event == CROW_BACK || event == CROW_ALL) {
    activeServoMask = SERVO_ALL;
  } else if (event == CROW_LEFT) {
    activeServoMask = SERVO_LEFT;
  } else if (event == CROW_RIGHT) {
    activeServoMask = SERVO_RIGHT;
  }
}

if (currentTime < reactionEndTime) {
  
  static unsigned long lastPatternChange = 0;
  
  if (currentTime - lastPatternChange >= 1000) {
    buzzer1.shuffleSoundCrow(); 
    buzzer2.shuffleSoundCrow(); 
    led1.shufflePattern(); 
    led2.shufflePattern(); 
    
    lastPatternChange = currentTime;
  }

  buzzer1.update();
  buzzer2.update();
  led1.update();
  led2.update();

  if (activeServoMask & SERVO_LEFT) {
    servo.activateLeft(currentTime);
  }
  if (activeServoMask & SERVO_RIGHT) {
    servo.activateRight(currentTime);
  }
  
} else {
  // Stop all action once the reaction is finished
  // (You might need custom functions like stop() if updates don't handle idle)
  // activeServoMask = 0; // Reset the state
}

  // RIGHT
  /*if (event == CROW_RIGHT){
    static unsigned long lastPatternChange = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastPatternChange >= 1000) {
        buzzer1.shuffleSoundCrow(); 
        buzzer2.shuffleSoundCrow(); 
        led1.shufflePattern(); 
        led2.shufflePattern(); 
        
        lastPatternChange = currentTime;
      }
      buzzer1.update();
      buzzer2.update();
      led1.update();
      led2.update();
  
    unsigned long now = millis();
    servo.activateRight(now);
  }

  // LEFT
  if (event == CROW_LEFT){
    static unsigned long lastPatternChange = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastPatternChange >= 1000) {
        buzzer1.shuffleSoundCrow(); 
        buzzer2.shuffleSoundCrow(); 
        led1.shufflePattern(); 
        led2.shufflePattern(); 
        
        lastPatternChange = currentTime;
      }
      buzzer1.update();
      buzzer2.update();
      led1.update();
      led2.update();
  
    unsigned long now = millis();
    servo.activateLeft(now);
    servo.activateRight(now);
  }

  // BOTHfe
  if (event == CROW_FRONT || CROW_BACK){
    static unsigned long lastPatternChange = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastPatternChange >= 1000) {
        buzzer1.shuffleSoundCrow(); 
        buzzer2.shuffleSoundCrow(); 
        led1.shufflePattern(); 
        led2.shufflePattern(); 
        
        lastPatternChange = currentTime;
      }
      buzzer1.update();
      buzzer2.update();
      led1.update();
      led2.update();
  
    unsigned long now = millis();
    servo.activateLeft(now);
    servo.activateRight(now);
  }


  // -------- OUTPUT --------
  static unsigned long lastPatternChange = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastPatternChange >= 1000) {
        buzzer1.shuffleSoundCrow(); 
        buzzer2.shuffleSoundCrow(); 
        led1.shufflePattern(); 
        led2.shufflePattern(); 
        
        lastPatternChange = currentTime;
    }
    buzzer1.update();
    buzzer2.update();
    led1.update();
    led2.update();
  
  unsigned long now = millis();
  servo.activateLeft(now);
  servo.activateRight(now);*/

  delay(100);
}

