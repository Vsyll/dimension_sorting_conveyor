#ifndef PENGAMAN_API_H
#define PENGAMAN_API_H
#include <Arduino.h>

// Sensor api KY-026 + relay cut-off + status emergency
class PengamanApi {
  public:
    PengamanApi(uint8_t pinFlame, uint8_t pinRelay, int ambang,
                uint8_t relayAktif, uint8_t relayPutus)
      : _pinFlame(pinFlame), _pinRelay(pinRelay), _ambang(ambang),
        _relayAktif(relayAktif), _relayPutus(relayPutus) {}

    void mulai() {
      pinMode(_pinRelay, OUTPUT);
      digitalWrite(_pinRelay, _relayAktif);   // daya aktuator ON
      _emergency = false;
    }

    // true bila api terdeteksi (sekaligus memutus daya aktuator)
    bool cek() {
      if (analogRead(_pinFlame) < _ambang) {
        _emergency = true;
        digitalWrite(_pinRelay, _relayPutus);
      }
      return _emergency;
    }

    void reset() {
      _emergency = false;
      digitalWrite(_pinRelay, _relayAktif);
    }

    bool sedangEmergency() const { return _emergency; }

  private:
    uint8_t _pinFlame, _pinRelay, _relayAktif, _relayPutus;
    int     _ambang;
    bool    _emergency = false;
};

#endif