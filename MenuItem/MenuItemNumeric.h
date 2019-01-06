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
    while (M5.BtnC.isPressed()) M5.update();
    M5.Lcd.fillRect( rect.x+1, rect.y+1, rect.w-2, rect.h-2, fillColor);
    DrawTitle();
    ButtonDrawer btnDrawer;
    btnDrawer.setTitle("Back","-","+");
    DrawNum(value, cursorColor);
    int pv = value;
    int repeat = 0;
    do {
#ifdef _PLUSEncoder_H_
      if (PlusEncoder.update()) {
        if (PlusEncoder.isLongClick()) break;
        if (PlusEncoder.wasUp()   && value < maximum) { ++value; if (callback) { callback(this); } }
        if (PlusEncoder.wasDown() && value > minimum) { --value; if (callback) { callback(this); } }
      }
#endif
#ifdef _FACESGameBoy_H_
      if (FacesGameBoy.update()) {
        if (FacesGameBoy.wasLeft()
         || FacesGameBoy.wasPressedB()) break;
        if (FacesGameBoy.isUp()   && value < maximum) { ++value; ++repeat; if (callback) { callback(this); } }
        if (FacesGameBoy.isDown() && value > minimum) { --value; ++repeat; if (callback) { callback(this); } }
      }
#endif
      M5.update();
      if ((M5.BtnC.wasPressed() || M5.BtnC.pressedFor(250)) && value < maximum) { ++value; ++repeat; if (callback) { callback(this); } }
      if ((M5.BtnB.wasPressed() || M5.BtnB.pressedFor(250)) && value > minimum) { --value; ++repeat; if (callback) { callback(this); } }
      btnDrawer.draw();
      if (pv != value) {
        DrawNum(value, cursorColor);
        pv = value;
        if (0 < repeat && repeat < 100) delay(100 - repeat);
      } else {
        repeat = 0;
        delay(10);
      }
    } while (!M5.BtnA.wasPressed());
  }
private:
  void DrawNum(int value, uint16_t color)
  {
    Rect area ( rect.x + rect.w * 5 / 6 - 10
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
