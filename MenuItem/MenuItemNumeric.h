#ifndef _MenuItemNumeric_H_
#define _MenuItemNumeric_H_

#include "../MenuItem.h"

class MenuItemNumeric : public MenuItem {
public:
  int minimum = 0;
  int maximum = 1;
  int value = 0;

  MenuItemNumeric(const String& titleStr, int min, int max, int value, int tg = 0, std::function<void(MenuItem*)> cb = 0)
  : MenuItem(titleStr, tg, cb), minimum(min), maximum(max), value(value) {};

  MenuItemNumeric(const String& titleStr, int min, int max, int value, std::function<void(MenuItem*)> cb)
  : MenuItem(titleStr, cb), minimum(min), maximum(max), value(value) {};

  virtual void OnAfterDraw(){
    DrawNum(value, fillColor);
  }
  virtual void OnEnter(){
    M5.Lcd.fillRect( rect.x+1, rect.y+1, rect.w-2, rect.h-2, fillColor);
    DrawTitle();
    ButtonDrawer btnDrawer;
    btnDrawer.setTitle("Back","-","+");
    DrawNum(value, cursorColor);
    int pv = value;
    do {
  #ifdef _PLUSEncoder_h_
      PlusEncoder.update();
      if (PlusEncoder.isLongClick()) break;
      if (PlusEncoder.wasUp()   && value > minimum) { --value; if (callback) { callback(this); } }
      if (PlusEncoder.wasDown() && value < maximum) { ++value; if (callback) { callback(this); } }
  #endif
      M5.update();
      if ((M5.BtnB.wasPressed() || M5.BtnB.pressedFor(150)) && value > minimum) { --value; if (callback) { callback(this); } }
      if ((M5.BtnC.wasPressed() || M5.BtnC.pressedFor(150)) && value < maximum) { ++value; if (callback) { callback(this); } }
      btnDrawer.draw();
      if (pv != value) {
        DrawNum(value, cursorColor);
        pv = value;
      }
      delay(20);
    } while (!M5.BtnA.wasPressed());
  }
private:
  void DrawNum(int value, uint16_t color)
  {
    Rect area ( rect.x + rect.w * 3 / 4
              , rect.y + rect.h / 6
              , rect.w / 6
              , rect.h -(rect.h / 6)*2);
    M5.Lcd.drawRect(area.x+1, area.y, area.w-2, area.h, frameColor);
    M5.Lcd.drawRect(area.x, area.y+1, area.w, area.h-2, frameColor);
    area.Inflate(-2);
    M5.Lcd.setCursor(area.x, area.y+1);
    M5.Lcd.setTextColor(fontColor);
    M5.Lcd.fillRect(area.x, area.y, area.w, area.h, color);
    M5.Lcd.printf("%5d", value);
  }
};

#endif
