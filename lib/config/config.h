#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===== Pin Servo & Relay =====
#define PIN_SERVO           13
inline constexpr int   SERVO_STEP_DEG      = 7;
inline constexpr int   SERVO_STEP_INTERVAL = 5;

#define PIN_RELAY           27

// ===== Motor Config (L298N IN1) =====
#define PIN_MOTOR           14
#define INITIAL_MOTOR_DUTY  230
#define MOTOR_PWM_FREQ      25000

// ====== Pin Sensor ======
#define PIN_IR_PANJANG      32
#define PIN_IR_LEBAR_L      33
#define PIN_IR_LEBAR_R      25
#define PIN_IR_TINGGI       26
#define PIN_FLAME           34

// ===== Logika Sensor / Aktuator =====
#define IR_ON               LOW
#define RELAY_AKTIF         LOW
#define RELAY_PUTUS         HIGH

// ===== Virtual Pin Blynk =====
#define VP_START            V0
#define VP_RESET            V1
#define VP_SPEED            V2
#define VP_LED              V3
#define VP_STATUS           V4
#define VP_TOTAL            V5
#define VP_EMERG            V6
#define VP_BARANG           V7

// ===== Kalibrasi =====
inline constexpr float         BELT_CM_PER_S     = 22.651;
inline constexpr int           FLAME_AMBANG      = 1600;
inline constexpr unsigned long WAKTU_KE_UJUNG_MS = 57;
inline constexpr float         PANJANG_AMBANG_MM = 35.0;
inline constexpr float         DIM_MAKS          = 70.0;

inline constexpr int SUDUT_SLOT[8] = {101, 146, 34, 124, 79, 169, 56, 11};

inline const char* const NAMA_KAT[8] = {
    "K-C-R", "K-C-T", "K-L-R", "K-L-T",
    "P-C-R", "P-C-T", "P-L-R", "P-L-T"
};

#endif