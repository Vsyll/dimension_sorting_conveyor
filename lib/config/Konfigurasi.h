#ifndef KONFIGURASI_H
#define KONFIGURASI_H

// ===== Pin ESP32 =====
#define PIN_SERVO   13   // servo 180 deg posisional (chute pengarah)
#define PIN_MOTOR   14   // PWM -> gate MOSFET (belt Tamiya)
#define PIN_RELAY   27   // relay cut-off (emergency)
#define PIN_FLAME   34   // KY-026 AO (ADC, input-only)
#define PIN_IR_L1   32   // presence + timer panjang
#define PIN_IR_L2   33   // lebar luar
#define PIN_IR_T1   25   // tinggi bawah (cadangan)
#define PIN_IR_T2   26   // tinggi atas (ambang tinggi)
#define PIN_SWITCH  18   // switch START fisik (INPUT_PULLUP, ON=LOW)

// ===== Logika sensor/aktuator =====
#define IR_ON        LOW
#define RELAY_AKTIF  LOW
#define RELAY_PUTUS  HIGH

// ===== Virtual Pin Blynk =====
#define VP_START   V0
#define VP_RESET   V1
#define VP_SPEED   V2
#define VP_LED     V3
#define VP_STATUS  V4
#define VP_TOTAL   V5
#define VP_EMERG   V6
#define VP_BARANG  V7

// ===== Kalibrasi (sesuaikan di lapangan) =====
const float         BELT_CM_PER_S     = 8.0;    // kecepatan belt (cm/detik)
const int           FLAME_AMBANG      = 1600;   // ADC 12-bit; < ambang = ADA API
const unsigned long WAKTU_KE_UJUNG_MS = 3000;   // ~24 cm sesudah IR @ 8 cm/s
const float         PANJANG_AMBANG_MM = 50;     // >= 50 mm (5 cm) = "panjang"
const float         DIM_MAKS          = 100;    // batas atas hasil ukur (mm)
int                 motorDuty         = 140;    // 0-255; rel 5V -> jaga <=150 (~3V) utk Tamiya

// 8 sudut chute -> tengah tiap sekat (tray 1/2 lingkaran, 8 sekat @ 22,5 deg)
const int   SUDUT_SLOT[8] = {11, 34, 56, 79, 101, 124, 146, 169};
// index kategori = (Pjg?4:0)+(Lbr?2:0)+(Tgi?1:0) -> K1..K8
const char* NAMA_KAT[8]   = {
  "Pdk-Ciut-Rdh", "Pdk-Ciut-Tgi", "Pdk-Lbr-Rdh", "Pdk-Lbr-Tgi",
  "Pjg-Ciut-Rdh", "Pjg-Ciut-Tgi", "Pjg-Lbr-Rdh", "Pjg-Lbr-Tgi"
};

#endif