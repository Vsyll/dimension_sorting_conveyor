#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===== Pin Servo & Relay =====
#define PIN_SERVO           13
inline constexpr int   SERVO_STEP_DEG      = 3;
inline constexpr int   SERVO_STEP_INTERVAL = 20;

#define PIN_RELAY           27
#define PIN_LED_RELAY       5    // LED indikator relay terputus

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
inline constexpr float         BELT_CM_PER_S        = 16.778;
inline constexpr int           FLAME_AMBANG         = 1600;
inline constexpr uint16_t      TIMEOUT_HOUSING_MS   = 700;
inline constexpr float         PANJANG_AMBANG_MM    = 43.0;
inline constexpr float         DIM_MAKS             = 78.0;
inline constexpr unsigned long WAKTU_KE_UJUNG_MS    = 1810;

inline constexpr int SUDUT_SLOT[8] = {101, 155, 34, 110, 79, 175, 56, 11};

inline const char* const NAMA_KAT[8] = {
    "K-C-R", "K-C-T", "K-L-R", "K-L-T",
    "P-C-R", "P-C-T", "P-L-R", "P-L-T"
};

#endif