// Definisi Pin
const int pinIN1 = 12;
const int pinIN2 = 14;
const int pinENA = 13; // Pin untuk PWM

// Konfigurasi PWM ESP32
const int pwmFrekuensi = 5000; // Frekuensi 5 kHz
const int pwmResolusi  = 8;    // Resolusi 8-bit (Nilai 0 - 255)

// Variabel Kecepatan Default (0 sampai 255)
int kecepatan Motor = 150; 

void setup() {
  Serial.begin(115200);
  
  // Set pin arah sebagai OUTPUT
  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  
  // Konfigurasi PWM untuk ESP32 versi baru (Arduino Core 3.0+)
  // Jika menggunakan versi lama, kamu bisa pakai ledcSetup & ledcAttachPin
  ledcAttachChannel(pinENA, pwmFrekuensi, pwmResolusi, 0); 
  
  motorBerhenti();
  
  Serial.println("--- Kontrol Motor L298N + PWM Siap! ---");
  Serial.println("Ketik perintah di Serial Monitor:");
  Serial.println("M = Maju  |  B = Mundur  |  S = Stop");
  Serial.println("Ketik angka 0 - 255 untuk mengubah KECEPATAN");
  Serial.println("----------------------------------------");
}

void loop() {
  if (Serial.available() > 0) {
    // Cek apakah input berupa angka (Kecepatan) atau huruf (Arah)
    if (isDigit(Serial.peek())) {
      // Jika input adalah angka, baca sebagai Integer
      kecepatanMotor = Serial.parseInt();
      // Batasi nilai input agar tetap di antara 0 - 255
      kecepatanMotor = constrain(kecepatanMotor, 0, 255);
      
      // Update kecepatan saat ini tanpa mengubah arah
      ledcWrite(pinENA, kecepatanMotor);
      
      Serial.print("-> Kecepatan diubah menjadi: ");
      Serial.println(kecepatanMotor);
    } 
    else {
      // Jika input adalah huruf, baca sebagai karakter perintah
      char perintah = Serial.read();
      
      switch (toupper(perintah)) {
        case 'M':
          motorMaju(kecepatanMotor);
          Serial.print("Status: MAJU | Kecepatan: ");
          Serial.println(kecepatanMotor);
          break;
        case 'B':
          motorMundur(kecepatanMotor);
          Serial.print("Status: MUNDUR | Kecepatan: ");
          Serial.println(kecepatanMotor);
          break;
        case 'S':
          motorBerhenti();
          Serial.println("Status: STOP");
          break;
        case '\n':
        case '\r':
          break;
        default:
          Serial.println("Perintah salah! Gunakan M, B, S atau Angka 0-255.");
          break;
      }
    }
  }
}

// Fungsi Motor Maju dengan PWM
void motorMaju(int speed) {
  digitalWrite(pinIN1, HIGH);
  digitalWrite(pinIN2, LOW);
  ledcWrite(pinENA, speed);
}

// Fungsi Motor Mundur dengan PWM
void motorMundur(int speed) {
  digitalWrite(pinIN1, LOW);
  digitalWrite(pinIN2, HIGH);
  ledcWrite(pinENA, speed);
}

// Fungsi Motor Berhenti
void motorBerhenti() {
  digitalWrite(pinIN1, LOW);
  digitalWrite(pinIN2, LOW);
  ledcWrite(pinENA, 0); // Matikan PWM
}