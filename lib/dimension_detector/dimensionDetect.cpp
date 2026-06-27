#include "DimensionDetect.h"

PengukurDimensi::PengukurDimensi() {
    stateSekarang = IDLE;
}

void PengukurDimensi::debug(HasilUkur h, long total) {
    Serial.printf("[DEBUG DIMENSI] Slot: %d (%s) | P: %.1f mm | L: %d | T: %d | Total Benda: %ld\n",
                  h.kategori + 1, NAMA_KAT[h.kategori], h.P, h.lebar, h.tinggi, total);
}

void PengukurDimensi::begin() {
    pinMode(PIN_IR_PANJANG, INPUT);
    pinMode(PIN_IR_LEBAR_L, INPUT);
    pinMode(PIN_IR_LEBAR_R, INPUT);
    pinMode(PIN_IR_TINGGI,  INPUT);
    stateSekarang = IDLE;
}

bool PengukurDimensi::adaBenda() { 
    return _terhalang(PIN_IR_PANJANG); 
}

// Mengukur dimensi selama benda melewati sensor panjang
bool PengukurDimensi::update(HasilUkur &hasil, bool (*adaApi)()) {
    
    // Interupsi darurat: Jika api terdeteksi, langsung reset state ke IDLE
    if (adaApi && adaApi()) {
        stateSekarang = IDLE;
        return false;
    }

    switch (stateSekarang) {
        
        case IDLE:
            // Kondisi 1: Tunggu sampai ada benda memotong sensor panjang di pintu masuk
            if (_terhalang(PIN_IR_PANJANG)) {
                waktuMasuk = millis();
                waktuLepas = 0;
                statusTinggi = false;
                statusLebar = false;
                pernahKenaLebar = false;
                stateSekarang = MENGUKUR_MASUK;
            }
            break;

        case MENGUKUR_MASUK:
            // Kondisi 2: Benda sedang melewati sensor panjang di pintu masuk
            if (_terhalang(PIN_IR_PANJANG)) {
                waktuLepas = millis(); // Update terus waktu selama pintu masuk terhalang
            } else {
                // Benda sudah lolos dari sensor panjang
                if (waktuLepas > 0) {
                    stateSekarang = MENUNGGU_KELUAR;
                }
            }
            
            // Intip sensor tinggi (karena posisinya di atas sensor panjang)
            if (_terhalang(PIN_IR_TINGGI)) {
                statusTinggi = true;
            }
            break;

        case MENUNGGU_KELUAR:
            // Kondisi 3: Benda lepas dari pintu masuk, sedang berjalan di dalam rumahan kosong
            // Menunggu sampai bagian depan benda menyentuh sensor lebar di ujung keluar
            if (_terhalang(PIN_IR_LEBAR_L) && _terhalang(PIN_IR_LEBAR_R)) {
                statusLebar = true;
                pernahKenaLebar = true;
                stateSekarang = MENGUKUR_LEBAR;
            }
            
            // Fail-safe Timeout
            if ((millis() - waktuMasuk) > WAKTU_KE_UJUNG_MS) {
                stateSekarang = SELESAI;
            }
            break;

        case MENGUKUR_LEBAR:
            // Kondisi 4: Benda sedang melewati sensor lebar di pintu keluar
            // Tunggu sampai ekor benda lepas total dari sensor lebar
            if (!_terhalang(PIN_IR_LEBAR_L) && !_terhalang(PIN_IR_LEBAR_R)) {
                stateSekarang = SELESAI;
            }
            break;

        case SELESAI:
            // Kondisi 5: Benda sudah keluar total, saatnya kalkulasi data
            if (waktuLepas == 0) waktuLepas = millis();
            
            unsigned long durasiObjekLewat = waktuLepas - waktuMasuk;
            
            HasilUkur h;
            h.P = BELT_CM_PER_S * (durasiObjekLewat / 1000.0) * 10.0;
            
            if (h.P > DIM_MAKS) h.P = DIM_MAKS;
            
            h.panjang  = (h.P >= PANJANG_AMBANG_MM);
            h.lebar    = statusLebar;
            h.tinggi   = statusTinggi;
            h.kategori = (h.panjang ? 4 : 0) + (h.lebar ? 2 : 0) + (h.tinggi ? 1 : 0);
            
            hasil = h;            // Kirim data kembali ke main.ino via referensi
            stateSekarang = IDLE; // Reset kembali ke awal untuk bersiap ke benda berikutnya
            return true;          // Return TRUE menandakan data sudah valid & siap diproses sorting
    }
    
    return false; // Return FALSE berarti benda masih berproses di dalam terowongan
}

// Fungsi internal pembacaan logika IR_ON (LOW)
bool PengukurDimensi::_terhalang(uint8_t pin) { 
    return digitalRead(pin) == IR_ON; 
}