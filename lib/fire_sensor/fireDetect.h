#ifndef FIREDETECT_H
#define FIREDETECT_H

#include <Arduino.h>
#include "config.h"

class PengamanApi {
  public:
    PengamanApi();
    void debug();
    void begin();
    bool cek();
    void reset();
    bool sedangEmergency() const;

  private:
    bool _emergency;
};

#endif