#ifndef _BLEDemo_H_
#define _BLEDemo_H_

#include <M5Stack.h>
#include <BLERemoteCharacteristic.h>

#include "../MenuItemBLE.h"

class BLEDemo : public MenuCallBack
{
public:
  BLEDemo() {};

  virtual void operator()(MenuItem* sender)
  {
    MenuItemBLE* mi = static_cast<MenuItemBLE*>(sender);
    if (!mi) return;
    if (!mi->pRemoteChar) return;
    pRemoteServ = mi->pRemoteServ;
    pRemoteChar = mi->pRemoteChar;

    btnDrawer.setTitle("Back","","");
    if (setup()) {
      do {
        btnDrawer.draw();
        M5.update();
      } while (loop() && !M5.BtnA.isPressed());
      close();
      M5.Lcd.fillScreen(0);
    }
  }

  bool setup()
  {
    M5.Lcd.setTextColor(0xffff, 0);
    M5.Lcd.fillScreen(0);
    M5.Lcd.setCursor(0,  0); M5.Lcd.print(pRemoteChar->toString().c_str());
    M5.Lcd.setCursor(0, 50); M5.Lcd.print("uint8_t :");
    M5.Lcd.setCursor(0, 70); M5.Lcd.print("uint16_t:");
    M5.Lcd.setCursor(0, 90); M5.Lcd.print("uint32_t:");
    M5.Lcd.setCursor(0,110); M5.Lcd.print("str HEX :");
    M5.Lcd.setCursor(0,130); M5.Lcd.print("string  :");
    return true;
  }

  bool loop()
  {
    if (!pRemoteServ->getClient()->isConnected()) return false;
    std::string value = pRemoteChar->readValue();
    M5.Lcd.setCursor(60,110);  
    for (int i = 0; i < value.size(); ++i) {
      M5.Lcd.printf("%02x", value[i]);
    }
    M5.Lcd.setCursor(60,130);  M5.Lcd.print(value.c_str());
    M5.Lcd.print("      \r\n          ");
    M5.Lcd.setCursor(60, 50);  M5.Lcd.printf("%02x", pRemoteChar->readUInt8());
    M5.Lcd.setCursor(60, 70);  M5.Lcd.printf("%04x", pRemoteChar->readUInt16());
    M5.Lcd.setCursor(60, 90);  M5.Lcd.printf("%08x", pRemoteChar->readUInt32());
    return true;
  }

private:
  BLERemoteCharacteristic* pRemoteChar;
  BLERemoteService *pRemoteServ;
};
#endif
