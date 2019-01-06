#ifndef _DHT12Demo_H_
#define _DHT12Demo_H_

#include <M5Stack.h>

#include "../MenuItem.h"

class DHT12Demo : public MenuCallBack
{
public:
  DHT12Demo(uint8_t id=0)
  {
    _id = (id==0 || id>126) 
        ? 0x5c
        : id;
  }

  bool setup()
  {
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(0xffff, 0);
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
    return true;
  }

  bool loop()
  {
    switch (read()) {
      case 1:  M5.Lcd.setCursor(0,0); M5.Lcd.print("error : endTransmission"); return true;
      case 2:  M5.Lcd.setCursor(0,0); M5.Lcd.print("error : available      "); return true;
      case 3:  M5.Lcd.setCursor(0,0); M5.Lcd.print("error : checksum       "); return true;
      default: M5.Lcd.setCursor(0,0); M5.Lcd.print("                       "); break;
    }

    uint16_t temperature10 = (datos[2] * 10 + datos[3]);
    uint16_t humidity10    = (datos[0] * 10 + datos[1]);
    int y_temperature = 220 - temperature10 / 5;
    int y_humedad     = 220 - humidity10 / 5;
    M5.Lcd.drawPixel(_x, y_temperature, 0xff00);
    M5.Lcd.drawPixel(_x, y_humedad, 0x00ff);
    _x = (_x + 1) % 320;
    
    M5.Lcd.setCursor(0,10);
    //Read temperature with preset scale.
    M5.Lcd.print("Temperatura: ");
    M5.Lcd.printf("%4.1f", (float)temperature10 / 10);

    //Read humidity.
    M5.Lcd.print("   Humedad: ");
    M5.Lcd.printf("%4.1f", (float)humidity10 / 10);
    return true;
  }

private:
  uint8_t read()
  {
    Wire.beginTransmission(_id);
    Wire.write(0);
    if (Wire.endTransmission()!=0) return 1;
    Wire.requestFrom(_id, (uint8_t)5);
    for (int i=0;i<5;i++) {
      datos[i]=Wire.read();
    };
    delay(50);
    if (Wire.available()!=0) return 2;
    if (datos[4]!=(datos[0]+datos[1]+datos[2]+datos[3])) return 3;
    return 0;
  }

  uint8_t datos[5];
  uint8_t _id;
  uint16_t _x = 0;
};

#endif
