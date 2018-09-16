#ifndef _ADInputDemo_h_
#define _ADInputDemo_h_

#include <M5Stack.h>

class ADInputDemo
{
public:
  ADInputDemo() {}
  void setup()
  {
    pinMode(35,INPUT);
    pinMode(36,INPUT);
    dacWrite(25, 0); // anti speaker noise.

    M5.Lcd.fillScreen(0);
    M5.Lcd.setCursor(0,0);
    _x = 0;
    for (int i = 0 ; i < 10; ++i) {
      M5.Lcd.drawLine(0,210 - i * 20, 320, 210 - i * 20, 0x4208);
    }
    for (int i = 0 ; i < 16; ++i) {
      M5.Lcd.drawLine(10 + i * 20, 20, 10 + i * 20, 220, 0x4208);
    }
    for (int i = 0 ; i < 11; ++i) {
      M5.Lcd.drawLine(0,220 - i * 20, 320, 220 - i * 20, 0x8410);
    }
    for (int i = 0 ; i < 16; ++i) {
      M5.Lcd.drawLine(i * 20, 20, i * 20, 220, 0x8410);
    }
  }

  void loop()
  {
    _ad35sum += analogRead(35);
    _ad36sum += analogRead(36);
    if (++_sumCount % 8 == 0) {
      uint16_t ad35 = _ad35sum / _sumCount;
      uint16_t ad36 = _ad36sum / _sumCount;
      int y_35 = 220 - ad35 / 20;
      int y_36 = 220 - ad36 / 20;
      M5.Lcd.drawPixel(_x, y_35, 0xffe0);
      M5.Lcd.drawPixel(_x, y_36, 0x001f);
      M5.Lcd.setCursor(0,0);
      M5.Lcd.printf("AD35 :%04d / AD36 :%04d", ad35, ad36);
      _x = (_x + 1) % TFT_HEIGHT;
      _sumCount = 0;
      _ad35sum = 0;
      _ad36sum = 0;
    }
  }

private:
  uint16_t _x = 0;
  uint32_t _ad35sum = 0;
  uint32_t _ad36sum = 0;
  uint8_t _sumCount = 0;
};

#endif
