#ifndef _MenuContainer_H_ 
#define _MenuContainer_H_ 

#include "MenuItem.h"

class MenuContainer : public MenuItem {
private:
  MenuItem* selectedItem;
  Rect _cursorRect;
  bool _moving = true;

public:
  MenuContainer() : MenuItem() {};
  MenuContainer(const String& t, MenuItem* sub[] = 0)
  : MenuItem(t, sub) {};


  void begin() {
    selectedItem = subItems[0];
    for (uint8_t i = 0; subItems[i]; ++i) {
      subItems[i]->visible = true;
    }
    destRect.w = 0;     // root item hidden
    destRect.h = itemHeight/2;
    UpdateDestRect();
  }


  void loop(bool force = false) {
    if (M5.BtnA.wasPressed())
    { // 親階層を選択
      force = true;
      if (selectedItem->parentItem != this) {
        selectedItem = selectedItem->parentItem;
        for (uint8_t i = 0; selectedItem->subItems[i]; ++i) {
          selectedItem->subItems[i]->Hide();
        }
        UpdateDestRect();
      }
    }
    if (M5.BtnB.wasPressed())
    { // toggle cursor メニューカーソル位置を順に変更
      force = true;
      MenuItem *parent = selectedItem->parentItem;
      for (uint8_t i = 0; parent->subItems[i]; ++i) {
        if (parent->subItems[i] != selectedItem) continue;
        if (!parent->subItems[++i]) i = 0;
        selectedItem = parent->subItems[i];
        break;
      }
    }
    if (M5.BtnC.wasPressed()) { 
      selectedItem->OnEnter();
      if (selectedItem->subItems && selectedItem->subItems[0]) {
        selectedItem = selectedItem->subItems[0];
        force = true;
        UpdateDestRect();
      } else {
        MenuItem* mi = selectedItem;
        while (mi && mi != this && !mi->callback) {
          mi = mi->parentItem;
        }
        if (mi->callback) {
          mi->callback(selectedItem);
        }
      }
    }
    if (force) { 
      if (selectedItem->destRect.h * 2 + selectedItem->destRect.y > TFT_WIDTH) {
        DestRectYScroll(-(selectedItem->destRect.h * 2 + selectedItem->destRect.y - TFT_WIDTH));
      } else
      if (selectedItem->destRect.y < 0) {
        DestRectYScroll(-selectedItem->destRect.y);
      }
      //_moving = true; force = false;
    } 
    if (_moving || force) {
      if (!_cursorRect.equal(selectedItem->destRect)) { // カーソル移動
        const Rect& c = _cursorRect;
        Rect r = c.mixRect(selectedItem->destRect);
        if (c.y     < r.y    ) M5.Lcd.fillRect( c.x , c.y    , c.w, r.y - c.y          , backgroundColor);
        if (c.y+c.h > r.y+r.h) M5.Lcd.fillRect( c.x , r.y+r.h, c.w, (c.y+c.h)-(r.y+r.h), backgroundColor);
        if (c.x     < r.x    ) M5.Lcd.fillRect( c.x , c.y    , r.x-c.x            ,c.h , backgroundColor);
        if (c.x+c.w > r.x+r.w) M5.Lcd.fillRect( r.x+r.w, c.y , (c.x+c.w)-(r.x+r.w),c.h , backgroundColor);
        _cursorRect = r;
        _moving = true;
        bool topErase = (subItems[0] && subItems[0]->rect.y != subItems[0]->destRect.y);
        Draw(force, selectedItem);
        if (topErase && 0 < subItems[0]->rect.y){
          M5.Lcd.fillRect( subItems[0]->rect.x, 0, subItems[0]->rect.w, subItems[0]->rect.y, backgroundColor);
        }
      } else {
        _moving = Draw(force);
      }
      M5.Lcd.setTextColor(0xFFFF, cursorColor);
      M5.Lcd.setCursor( selectedItem->rect.x + 10, selectedItem->rect.y + selectedItem->rect.h / 3);
      M5.Lcd.fillRect( selectedItem->rect.x+1, selectedItem->rect.y+1, selectedItem->rect.w-2, selectedItem->rect.h-2, cursorColor);
      M5.Lcd.fillRect( _cursorRect.x+1, _cursorRect.y+1, _cursorRect.w-2, _cursorRect.h-2, cursorColor);
      M5.Lcd.print(selectedItem->title);
      selectedItem->OnAfterDraw();
    }
    delay(10);
  }  
};

#endif
