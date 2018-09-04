#ifndef _MenuContainer_H_ 
#define _MenuContainer_H_ 

#include "MenuItem.h"

class MenuContainer : public MenuItem {
private:
  Rect _cursorRect;
  bool _moving = true;

  void ScrollSubitemArea(MenuItem* mi) {
    int scroll = 0;
    for (int i = 0; mi->subItems[i]; ++i) {
      if (mi->subItems[i]->destRect.h * 2 + mi->subItems[i]->destRect.y > TFT_WIDTH) {
        scroll = mi->subItems[i]->destRect.h * 2 + mi->subItems[i]->destRect.y - TFT_WIDTH;
      }
    }
    if (scroll > 0) {
      DestRectYScroll(-scroll);
    }
  }

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
  int16_t lastY = 0;
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
        ScrollSubitemArea(selectedItem->parentItem);
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
      if (selectedItem->destRect.h * 2 + selectedItem->destRect.y > TFT_WIDTH) {
        DestRectYScroll(-(selectedItem->destRect.h * 2 + selectedItem->destRect.y - TFT_WIDTH));
      }
    }
    if (M5.BtnC.wasPressed()) { 
      selectedItem->OnEnter();
      if (selectedItem->subItems && selectedItem->subItems[0]) {
        MenuItem* mi = selectedItem;
        selectedItem = selectedItem->subItems[0];
        force = true;
        UpdateDestRect();

        ScrollSubitemArea(mi);
      } else {
        MenuItem* mi = selectedItem;
        while (mi && mi != this && !mi->callback) {
          mi = mi->parentItem;
        }
        if (mi->callback) {
          mi->callback(selectedItem);
          force = true;
        }
      }
    }
    if (force) {
      if (selectedItem->destRect.y < itemHeight/2) {
        DestRectYScroll(-selectedItem->destRect.y + itemHeight );
      }
    } 
    if (_moving || force) {
      bool topErase = (subItems[0] && subItems[0]->rect.y != subItems[0]->destRect.y);
      _moving = Move();
      MenuItem* mi = 0;
      if (!_cursorRect.equal(selectedItem->destRect)) { // カーソル移動
        const Rect& c = _cursorRect;
        Rect r = c.mixRect(selectedItem->destRect);
        if (c.y     < r.y    ) M5.Lcd.fillRect( c.x , c.y    , c.w, r.y - c.y          , backgroundColor);
        if (c.y+c.h > r.y+r.h) M5.Lcd.fillRect( c.x , r.y+r.h, c.w, (c.y+c.h)-(r.y+r.h), backgroundColor);
        if (c.x     < r.x    ) M5.Lcd.fillRect( c.x , c.y    , r.x-c.x            ,c.h , backgroundColor);
        if (c.x+c.w > r.x+r.w) M5.Lcd.fillRect( r.x+r.w, c.y , (c.x+c.w)-(r.x+r.w),c.h , backgroundColor);
        _cursorRect = r;
        _moving = true;
        mi = selectedItem;
      }
      
      mi = Draw(_moving || force, mi);
      int16_t ly = mi->rect.Bottom();
      if (ly < lastY) {
        M5.Lcd.fillRect( mi->rect.x, ly, mi->rect.w, lastY - ly, backgroundColor);
      }
      lastY = ly;
      if (topErase && 0 < subItems[0]->rect.y){
        M5.Lcd.fillRect( subItems[0]->rect.x, 0, subItems[0]->rect.w, subItems[0]->rect.y, backgroundColor);
      }
      M5.Lcd.setTextColor(0xFFFF, cursorColor);
      M5.Lcd.setCursor( selectedItem->rect.x + 10, selectedItem->rect.y + selectedItem->rect.h / 3);
      M5.Lcd.fillRect( selectedItem->rect.x+1, selectedItem->rect.y+1, selectedItem->rect.w-2, selectedItem->rect.h-2, cursorColor);
      M5.Lcd.fillRect( _cursorRect.x+1, _cursorRect.y+1, _cursorRect.w-2, _cursorRect.h-2, cursorColor);
      M5.Lcd.print(selectedItem->title);
      selectedItem->OnAfterDraw();
    }
  }
};

#endif
