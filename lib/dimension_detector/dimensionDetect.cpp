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
                    // PERBAIKAN: Jika saat lepas dari sensor panjang ternyata sensor lebar SUDAH KENA,
                    // langsung lompat ke MENGUKUR_LEBAR, jika belum baru MENUNGGU_KELUAR
                    if (statusLebar) {
                        stateSekarang = MENGUKUR_LEBAR;
                    } else {
                        stateSekarang = MENUNGGU_KELUAR;
                    }
                }
            }
            
            // Intip sensor tinggi (Aman)
            if (_terhalang(PIN_IR_TINGGI)) {
                statusTinggi = true;
            }

            // PERBAIKAN KRUSIAL: Intip sensor lebar sejak di sini! 
            // Jadi kalau bagian depan benda kena sensor lebar duluan, langsung tercatat.
            if (_terhalang(PIN_IR_LEBAR_L) && _terhalang(PIN_IR_LEBAR_R)) {
                statusLebar = true;
                pernahKenaLebar = true;
            }
            break;

        case MENUNGGU_KELUAR:
            // Kondisi 3: Menunggu benda menyentuh sensor lebar (untuk benda pendek/tipis)
            if (_terhalang(PIN_IR_LEBAR_L) && _terhalang(PIN_IR_LEBAR_R)) {
                statusLebar = true;
                pernahKenaLebar = true;
                stateSekarang = MENGUKUR_LEBAR;
            }
            
            // Fail-safe Timeout (Sekarang bisa dikembalikan ke 57 ms dengan aman)
            if ((millis() - waktuMasuk) > TIMEOUT_HOUSING_MS) {
                stateSekarang = SELESAI;
                Serial.println(waktuMasuk);
                Serial.println(millis());
            }
            break;

        case MENGUKUR_LEBAR:
            // Kondisi 4: Tunggu sampai ekor benda lepas total dari sensor lebar
            if (!_terhalang(PIN_IR_LEBAR_L) && !_terhalang(PIN_IR_LEBAR_R)) {
                stateSekarang = SELESAI;
            }
            break;

        case SELESAI: { 
            // Ambil waktu lepas yang valid. Jika dipicu timeout, gunakan catatan waktu lepas terakhir yang terekam
            unsigned long waktuSelesaiMurni = (waktuLepas > 0) ? waktuLepas : millis();
            unsigned long durasiObjekLewat = waktuSelesaiMurni - waktuMasuk;
            
            HasilUkur h;
            h.P = BELT_CM_PER_S * ((float)durasiObjekLewat / 1000.0) * 10.0; 
            
            if (h.P > DIM_MAKS) h.P = DIM_MAKS;
            
            h.panjang  = (h.P >= PANJANG_AMBANG_MM);
            h.lebar    = statusLebar;
            h.tinggi   = statusTinggi;
            h.kategori = (h.panjang ? 4 : 0) + (h.lebar ? 2 : 0) + (h.tinggi ? 1 : 0);
            
            // ===== PRINT KALIBRASI UNTUK AHMAD =====
            Serial.println("\n========== DATA KALIBRASI ESP32 ==========");
            Serial.print("Durasi Terhalang Murni : "); Serial.print(durasiObjekLewat); Serial.println(" ms");
            Serial.print("Durasi dalam Detik     : "); Serial.print((float)durasiObjekLewat / 1000.0, 4); Serial.println(" s");
            Serial.print("Panjang Akhir (h.P)    : "); Serial.print(h.P); Serial.println(" mm");
            Serial.print("Status S/L/T           : "); 
            Serial.printf("P:%d | L:%d | T:%d\n", h.panjang, h.lebar, h.tinggi);
            Serial.println("==========================================\n");
            
            hasil = h;            // Kirim data kembali via referensi
            stateSekarang = IDLE; // Reset kembali ke awal
            return true;          // Return TRUE menandakan data sudah siap
        }
    }
    return false;
}

// Fungsi internal pembacaan logika IR_ON (LOW)
bool PengukurDimensi::_terhalang(uint8_t pin) { 
    return digitalRead(pin) == IR_ON; 
}