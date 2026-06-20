#include "DimensionDetect.h"

PengukurDimensi::PengukurDimensi() {}

void PengukurDimensi::debug(HasilUkur h, long total) {
    Serial.printf("[DEBUG DIMENSI] Slot: %d (%s) | P: %.1f mm | L: %d | T: %d | Total Benda: %ld\n",
                  h.kategori + 1, NAMA_KAT[h.kategori], h.P, h.lebar, h.tinggi, total);
}

void PengukurDimensi::begin() {
    pinMode(PIN_IR_PANJANG, INPUT);
    pinMode(PIN_IR_LEBAR_L, INPUT);
    pinMode(PIN_IR_LEBAR_R, INPUT);
    pinMode(PIN_IR_TINGGI,  INPUT);
}

bool PengukurDimensi::adaBenda() { 
    return _terhalang(PIN_IR_PANJANG); 
}

// Mengukur dimensi selama benda melewati sensor panjang
HasilUkur PengukurDimensi::ukur(bool (*adaApi)()) {
    // 1. Catat waktu awal saat benda mulai memotong sensor panjang
    unsigned long waktuMasuk = millis(); 
    
    bool statusLebar = false;
    bool statusTinggi = false;

    // 2. Selama sensor panjang masih terhalang oleh benda (LOW)
    while (_terhalang(PIN_IR_PANJANG)) {
        
        if (_terhalang(PIN_IR_LEBAR_L) && _terhalang(PIN_IR_LEBAR_R)) {
            statusLebar = true;
        }
        
        // Sensor tinggi
        if (_terhalang(PIN_IR_TINGGI)) {
            statusTinggi = true;
        }
        
        // Interupsi darurat jika api terdeteksi
        if (adaApi && adaApi()) {
            break;   
        }
        delay(2); 
    }
    
    // 3. Catat waktu saat benda lepas dari sensor panjang
    unsigned long waktuLepas = millis();
    unsigned long durasiObjekLewat = waktuLepas - waktuMasuk;

    HasilUkur h;
    // Hitung panjang: v (cm/s) * t (s) * 10 (ke mm)
    // Durasi dibagi 1000.0 agar menjadi satuan detik
    h.P = BELT_CM_PER_S * (durasiObjekLewat / 1000.0) * 10.0; 
    
    // Batasi hasil maksimal berdasarkan config.h
    if (h.P > DIM_MAKS) {
        h.P = DIM_MAKS;
    }
    
    // Seleksi Kategori Panjang berdasarkan ambang batas di config.h
    h.panjang  = (h.P >= PANJANG_AMBANG_MM);
    h.lebar    = statusLebar;
    h.tinggi   = statusTinggi;
    
    // Pemetaan bitwise untuk slot sorting (0 s.d 7)
    h.kategori = (h.panjang ? 4 : 0) + (h.lebar ? 2 : 0) + (h.tinggi ? 1 : 0);
    
    return h;
}

// Fungsi internal pembacaan logika IR_ON (LOW)
bool PengukurDimensi::_terhalang(uint8_t pin) { 
    return digitalRead(pin) == IR_ON; 
}