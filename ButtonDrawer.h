#ifndef _ButtonDrawer_H_
#define _ButtonDrawer_H_

#include <M5Stack.h>

#include "Rect.h"

class ButtonDrawer {
public:
  static ButtonDrawer* getInstance() {
    static ButtonDrawer singleton;
    return &singleton;
  }
  ButtonDrawer(){};
  ButtonDrawer(const String& btnA, const String& btnB, const String& btnC)
   : titles{btnA,btnB,btnC}
  {
  };
  void setTitle(const String& btnA, const String& btnB, const String& btnC) {
    titles[0] = btnA;
    titles[1] = btnB;
    titles[2] = btnC;
  }
  void setTitle(uint8_t idx, const String& str) {
    if (idx < 3) titles[idx] = str;
  }
  uint16_t colorFrame[2] = { 0xA514,0xffff };
  uint16_t colorFill[2]  = { 0,0 };
  uint16_t colorFont[2]  = { 0xffff,0xffff };

  void draw()
  {
    Rect r(32, 226, 64, 14);
    draw(r, M5.BtnA.isPressed(), titles[0]);
    r.x += 96;
    draw(r, M5.BtnB.isPressed(), titles[1]);
    r.x += 96;
    draw(r, M5.BtnC.isPressed(), titles[2]);
  }

private:
  String titles[3];
  void draw(Rect r, bool pressed, const String& title)
  {
    uint16_t color = colorFrame[pressed];
    M5.Lcd.drawRect(r.x+1,r.y  ,r.w-2,r.h   ,color);
    M5.Lcd.drawRect(r.x  ,r.y+1,r.w  ,r.h-2 ,color);
    r.Inflate(-2);
    int h = (r.h - 8)/2;
    int w = title.length() * 6;
    int x = (r.w - w) / 2;
    color = colorFill[pressed];
    M5.Lcd.fillRect(r.x    , r.y, x, r.h, color);
    M5.Lcd.fillRect(r.x+x+w, r.y, r.w-w-x, r.h, color);
    if (h > 0) {
      M5.Lcd.fillRect(r.x+x, r.y,       w, h, color);
      M5.Lcd.fillRect(r.x+x, r.y+r.h-h, w, h, color);
    }
    M5.Lcd.setCursor(r.x+x, r.y + h);
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(colorFont[pressed], color);
    M5.Lcd.print(title);
  }
};

#endif
