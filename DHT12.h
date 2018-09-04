#ifndef _DHT12_h_
#define _DHT12_h_

#include "Arduino.h"

class DHT12
{
public:
  DHT12(uint8_t id=0);
  void setup();
  void loop();
private:
  float readTemperature(uint8_t scale=0);
  float readHumidity();
  uint8_t read();
  uint8_t datos[5];
  uint8_t _id;
};

#endif
