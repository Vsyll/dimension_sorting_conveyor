#ifndef BELT_H
#define BELT_H

#include <Arduino.h>
#include "config.h"

class Belt {
  public:
    Belt();
    void begin();
    void run(bool aktif, int duty);
    
};

#endif