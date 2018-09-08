#ifndef _ADInputDemo_h_
#define _ADInputDemo_h_

#include "Arduino.h"

class ADInputDemo
{
public:
  ADInputDemo();
  void setup();
  void loop();
private:
  uint16_t _x = 0;
  uint32_t _ad35sum = 0;
  uint32_t _ad36sum = 0;
  uint8_t _sumCount = 0;
};

#endif
