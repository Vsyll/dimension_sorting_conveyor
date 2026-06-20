/*
 * Konveyor Penyortir Barang — 8 kategori (kombinasi besar/kecil P,L,T)
 * Board: ESP32 (DevKit). Struktur modular: tiap modul di file .h sendiri.
 *   Konfigurasi.h     : pin, kalibrasi, tabel sudut & nama kategori, VP Blynk
 *   Tampilan.h        : LCD I2C 16x2
 *   PengukurDimensi.h : baca 4 IR -> P/L/T -> kategori 0..7
 *   PengarahChute.h   : servo 180 deg (chute pengarah)
 *   PenggerakBelt.h   : motor belt via PWM/MOSFET
 *   PengamanApi.h     : KY-026 + relay (emergency cut-off)
 * index kategori = (Pjg?4:0)+(Lbr?2:0)+(Tgi?1:0) -> K1..K8
 */

// ---- Kredensial Blynk WAJIB sebelum include Blynk ----
#define BLYNK_TEMPLATE_ID   "Tubes INSTRU"
#define BLYNK_TEMPLATE_NAME "TUBES"
#define BLYNK_AUTH_TOKEN    "c1quVSxtfWCxQF4IEdBvp48NHHedn5CZ"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>

#include "Konfigurasi.h"
#include "Tampilan.h"
#include "PengukurDimensi.h"
#include "PengarahChute.h"
#include "PenggerakBelt.h"
#include "PengamanApi.h"

char ssid[] = "mufid";
char pass[] = "mufidfamily";

// ---- Objek tiap modul ----
Tampilan        lcd(0x27, 16, 2);
PengukurDimensi sensor(PIN_IR_L1, PIN_IR_L2, PIN_IR_T2, IR_ON,
                       BELT_CM_PER_S, PANJANG_AMBANG_MM, DIM_MAKS);
PengarahChute   chute(PIN_SERVO);
PenggerakBelt   belt(PIN_MOTOR);
PengamanApi     pengaman(PIN_FLAME, PIN_RELAY, FLAME_AMBANG,
                         RELAY_AKTIF, RELAY_PUTUS);

bool startOn     = false;   // status tombol START dari Blynk
long totalBarang = 0;

// dipanggil PengukurDimensi & loop agar emergency selalu dicek
bool cekApi() {
  if (pengaman.cek()) {
    belt.jalan(false, 0);
    lcd.dua("!! EMERGENCY !!", "API TERDETEKSI");
    Blynk.virtualWrite(VP_STATUS, "EMERGENCY: API!");
    Blynk.virtualWrite(VP_EMERG, 255);
    Blynk.virtualWrite(VP_LED, 0);
    Blynk.logEvent("api_terdeteksi");
    return true;
  }
  return false;
}

// ---- Handler widget Blynk ----
BLYNK_WRITE(VP_START) { startOn = param.asInt(); }
BLYNK_WRITE(VP_SPEED) { motorDuty = constrain(param.asInt(), 0, 255); }
BLYNK_WRITE(VP_RESET) {
  if (param.asInt() == 1) {          // lepas kunci emergency
    pengaman.reset();
    Blynk.virtualWrite(VP_EMERG, 0);
  }
}
BLYNK_CONNECTED() { Blynk.virtualWrite(VP_TOTAL, totalBarang); }

void prosesBenda() {
  HasilUkur h = sensor.ukur(cekApi);
  if (pengaman.sedangEmergency()) return;

  totalBarang++;
  chute.keSudut(SUDUT_SLOT[h.kategori]);   // arahkan ke sekat kategori

  char katStr[28];
  snprintf(katStr, sizeof(katStr), "%s (Kategori %d)",
           NAMA_KAT[h.kategori], h.kategori + 1);

  char b1[17], b2[17];
  snprintf(b1, sizeof(b1), "K%d %s", h.kategori + 1, NAMA_KAT[h.kategori]);
  snprintf(b2, sizeof(b2), "P=%2.0fmm L%d T%d", h.P, h.lebar ? 1 : 0, h.tinggi ? 1 : 0);
  lcd.dua(b1, b2);

  Blynk.virtualWrite(VP_STATUS, katStr);
  Blynk.virtualWrite(VP_BARANG, katStr);
  Blynk.virtualWrite(VP_TOTAL, totalBarang);
  Serial.printf("%s | P=%.0fmm lebar=%d tinggi=%d (total %ld)\n",
                katStr, h.P, h.lebar, h.tinggi, totalBarang);

  // tunggu benda sampai ujung & jatuh, sambil cek emergency
  unsigned long tw = millis();
  while (millis() - tw < WAKTU_KE_UJUNG_MS) {
    Blynk.run();
    if (cekApi()) return;
    delay(5);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);              // I2C: SDA=21, SCL=22
  lcd.mulai();
  lcd.dua("Konveyor Sortir", "Memulai...");

  pinMode(PIN_SWITCH, INPUT_PULLUP);
  sensor.mulai();
  belt.mulai();
  pengaman.mulai();
  chute.mulai();
  chute.keSudut(SUDUT_SLOT[0]);    // posisi awal -> sekat K1

  lcd.dua("Hubungkan WiFi", "mohon tunggu");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Blynk.virtualWrite(VP_STATUS, "IDLE - menunggu");
  lcd.dua("Sistem siap", "Tekan START");
}

void loop() {
  Blynk.run();
  if (pengaman.sedangEmergency()) return;   // terkunci; tekan RESET board/Blynk
  if (cekApi()) return;

  bool jalan = startOn || (digitalRead(PIN_SWITCH) == LOW);
  Blynk.virtualWrite(VP_LED, jalan ? 255 : 0);

  if (!jalan) {
    belt.jalan(false, 0);
    lcd.dua("Sistem IDLE", "Tekan START");
    Blynk.virtualWrite(VP_STATUS, "IDLE - menunggu");
    return;
  }

  belt.jalan(true, motorDuty);     // belt jalan
  if (sensor.adaBenda()) prosesBenda();
}