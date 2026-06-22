#include "Belt.h"

Belt::Belt() {}

void Belt::begin() {
    pinMode(PIN_MOTOR, OUTPUT);
    analogWriteFrequency(MOTOR_PWM_FREQ);
    analogWrite(PIN_MOTOR, INITIAL_MOTOR_DUTY); 
}

void Belt::setSpeed(int duty) {
    int saveDuty = constrain(duty, 0, 255);
    analogWrite(PIN_MOTOR, saveDuty);
}