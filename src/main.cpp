#define BLYNK_TEMPLATE_ID   "Tubes INSTRU"
#define BLYNK_TEMPLATE_NAME "TUBES"
#define BLYNK_AUTH_TOKEN    "c1quVSxtfWCxQF4IEdBvp48NHHedn5CZ"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#include "config.h"
#include "lcd_display.h"
#include "dimensionDetect.h"
#include "sorter.h"
#include "belt.h"
#include "fireDetect.h"

char ssid[] = "mufid";
char pass[] = "mufidfamily";

LcdDisplay lcd;
PengukurDimensi detektor;
PengarahChute   sorter;
Belt            belt;
PengamanApi     pengaman;

bool blynkEmergencyTrigger = false;
bool sistemBerjalan = false;  // ← tambahan
long totalBarang = 0;

int kecepatanMotorSekarang = INITIAL_MOTOR_DUTY;
unsigned long waktuTravelAktif = WAKTU_KE_UJUNG_MS;

bool butuhSorting = false;
unsigned long waktuSelesaiUkur = 0;
int kategoriTertunda = 0;

bool cekEmergency() {
    if (pengaman.cek()) {
        lcd.displayEmergency();
        Blynk.virtualWrite(VP_STATUS, "EMERGENCY: API!");
        Blynk.virtualWrite(VP_EMERG, 255);
        return true;
    }
    if (blynkEmergencyTrigger) {
        lcd.displayEmergency();
        Blynk.virtualWrite(VP_STATUS, "EMERGENCY: BLYNK STOP!");
        return true;
    }
    return false;
}

// ← tambahan handler start/stop
BLYNK_WRITE(VP_START) {
    sistemBerjalan = param.asInt();
    if (sistemBerjalan) {
        belt.setSpeed(kecepatanMotorSekarang);
        lcd.displayStandby();
        Blynk.virtualWrite(VP_STATUS, "CONVEYOR RUNNING");
    } else {
        belt.setSpeed(0);
        lcd.displayStandby();
        Blynk.virtualWrite(VP_STATUS, "CONVEYOR STOP");
    }
}

BLYNK_WRITE(VP_EMERG) {
    if (param.asInt() > 0) {
        blynkEmergencyTrigger = true;
    }
}

BLYNK_WRITE(VP_RESET) {
    if (param.asInt() == 1) {
        pengaman.reset();
        blynkEmergencyTrigger = false;
        totalBarang = 0;
        butuhSorting = false;                   // ← reset flag sorting
        sorter.keKategori(0);                   // ← balik servo ke posisi awal
        Blynk.virtualWrite(VP_EMERG, 0);
        Blynk.virtualWrite(VP_TOTAL, 0);
        Blynk.virtualWrite(VP_STATUS, "SISTEM NORMAL");
        lcd.displayStandby();
        if (sistemBerjalan) {
            belt.setSpeed(kecepatanMotorSekarang);
        }
    }
}

BLYNK_CONNECTED() {
    Blynk.virtualWrite(VP_TOTAL, totalBarang);
    Blynk.virtualWrite(VP_START, 0);  // pastikan tombol start OFF saat reconnect
}

void setup() {
    Serial.begin(115200);

    lcd.begin();
    lcd.displayBooting();
    lcd.displayStandby();

    detektor.begin();
    belt.begin();
    belt.setSpeed(0);  // mulai dalam kondisi stop
    pengaman.begin();
    sorter.begin();

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Blynk.virtualWrite(VP_STATUS, "SISTEM SIAP");
    Blynk.virtualWrite(VP_START, 0);
}

void loop() {
    Blynk.run();
    sorter.update();

    if (cekEmergency()) return;
    if (!sistemBerjalan) return;  // ← stop semua jika belum di-start

    if (!butuhSorting) {
        HasilUkur dataBenda;

        if (detektor.update(dataBenda, cekEmergency)) {
            totalBarang++;

            sorter.keKategori(dataBenda.kategori);

            lcd.displayHasil(dataBenda.kategori + 1, NAMA_KAT[dataBenda.kategori],
                             dataBenda.P, dataBenda.lebar, dataBenda.tinggi);
            detektor.debug(dataBenda, totalBarang);

            char statusStr[32];
            snprintf(statusStr, sizeof(statusStr), "%s (Slot %d)",
                     NAMA_KAT[dataBenda.kategori], dataBenda.kategori + 1);
            Blynk.virtualWrite(VP_STATUS, statusStr);
            Blynk.virtualWrite(VP_BARANG, NAMA_KAT[dataBenda.kategori]);
            Blynk.virtualWrite(VP_TOTAL, totalBarang);
            Blynk.virtualWrite(VP_LED, 255);

            butuhSorting = true;
            waktuSelesaiUkur = millis();
            kategoriTertunda = dataBenda.kategori;
        }
    }

    if (butuhSorting && (millis() - waktuSelesaiUkur >= waktuTravelAktif)) {
        butuhSorting = false;
        Blynk.virtualWrite(VP_STATUS, "CONVEYOR RUNNING");
    }
}