#ifndef _MPU9250Demo_h_
#define _MPU9250Demo_h_

#include <M5Stack.h>
#include "utility/MPU9250.h"

class MPU9250Demo
{
public:
  MPU9250Demo();
  bool setup();
  void loop();
private:
  void plotpoint(uint16_t x, uint16_t y, int r, int g, int b);
  MPU9250 IMU;
  uint16_t _x;
};

#endif
