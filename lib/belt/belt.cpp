#include "Belt.h"

Belt::Belt() {}

void Belt::begin() {
    pinMode(PIN_MOTOR, OUTPUT);
    analogWriteFrequency(MOTOR_PWM_FREQ);
    analogWrite(PIN_MOTOR, 0); 
}

// Menjalankan motor
void Belt::run(bool aktif, int duty) {
    if (aktif) {
        int saveDuty = constrain(duty, 0, 255);
        analogWrite(PIN_MOTOR, saveDuty);
    } else {
        analogWrite(PIN_MOTOR, 0);
    }
}