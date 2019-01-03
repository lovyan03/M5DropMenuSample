#ifndef _MenuItemBoolean_H_
#define _MenuItemBoolean_H_

#include "../MenuItem.h"

class MenuItemBoolean : public MenuItem {
public:
  bool value = false;

  MenuItemBoolean(const String& titleStr, int tg = 0, std::function<void(MenuItem*)> cb = 0)
  : MenuItem(titleStr, tg, cb), value(false) {};

  MenuItemBoolean(const String& titleStr, std::function<void(MenuItem*)> cb)
  : MenuItem(titleStr, cb), value(false) {};

  virtual void OnAfterDraw(){
    DrawSwitch(value);
  }
  virtual void OnEnter() {
    DrawSwitch(-1);
    do {
      M5.update();
      delay(5);
    } while (M5.BtnC.isPressed());
    value = !value;
    DrawSwitch(value);
  }

private:
  void DrawSwitch(int mode)
  {
    Rect area ( rect.x + rect.w * 5 / 6 - 10
              , rect.y + rect.h / 6
              , rect.w / 6
              , rect.h -(rect.h / 6)*2);
    M5.Lcd.drawRect(area.x+1, area.y, area.w-2, area.h, frameColor);
    M5.Lcd.drawRect(area.x, area.y+1, area.w, area.h-2, frameColor);
    area.Inflate(-2);
    int16_t w = area.w * 2 / 3;
    switch (mode){
    case 0:    M5.Lcd.fillRect(area.x    , area.y, w       , area.h, selectedItem==this ? cursorColor:fillColor);
               M5.Lcd.fillRect(area.x + w, area.y, area.w-w, area.h, backgroundColor);
               break;
    default:   M5.Lcd.fillRect(area.x, area.y, area.w, area.h, backgroundColor);
               M5.Lcd.fillRect(area.x+(area.w-w)/2, area.y, w, area.h, cursorColor);
               break;  
    case 1:    M5.Lcd.fillRect(area.Right()-w, area.y, w       , area.h, fontColor);
               M5.Lcd.fillRect(area.x        , area.y, area.w-w, area.h, backgroundColor);
               break;
    }
  }
};

#endif
