#ifndef _MenuContainer_H_ 
#define _MenuContainer_H_ 

#include "MenuItem.h"

class MenuContainer : public MenuItem {
private:
  Rect _cursorRect;
  bool _moving = true;
  bool _force = true;

  void ScrollSubitemArea(MenuItem* mi) {
    int scroll = 0;
    for (uint16_t i = 0; i != mi->subItems.size(); ++i) {
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
  MenuContainer(const String& t, const std::vector<MenuItem*> &sub)
  : MenuItem(t, sub) {};

  void begin() {
    selectedItem = subItems[0];
    for (uint16_t i = 0; i != subItems.size(); ++i) {
      subItems[i]->visible = true;
    }
    destRect.w = 0;     // root item hidden
    destRect.h = itemHeight/2;
    UpdateDestRect();
  }

  void moveUp() {
    _moving = true;
    if (selectedItem->parentItem != this) {
      selectedItem = selectedItem->parentItem;
      for (uint16_t i = 0; i != selectedItem->subItems.size(); ++i) {
        selectedItem->subItems[i]->Hide();
      }
      UpdateDestRect();
      ScrollSubitemArea(selectedItem->parentItem);
    }
  }
  void moveNext() {
    _moving = true;
    MenuItem *parent = selectedItem->parentItem;
    for (uint16_t i = 0; i != parent->subItems.size(); ++i) {
      if (parent->subItems[i] != selectedItem) continue;
      selectedItem = parent->subItems[(i+1) % parent->subItems.size()];
      break;
    }
    if (selectedItem->destRect.h * 2 + selectedItem->destRect.y > TFT_WIDTH) {
      DestRectYScroll(-(selectedItem->destRect.h * 2 + selectedItem->destRect.y - TFT_WIDTH));
    }
  }
  void movePrev() {
    _moving = true;
    MenuItem *parent = selectedItem->parentItem;
    for (uint16_t i = 0; i != parent->subItems.size(); ++i) {
      if (parent->subItems[i] != selectedItem) continue;
      selectedItem = parent->subItems[(i > 0) ? i - 1 : parent->subItems.size() - 1];
      break;
    }
    if (selectedItem->destRect.h * 2 + selectedItem->destRect.y > TFT_WIDTH) {
      DestRectYScroll(-(selectedItem->destRect.h * 2 + selectedItem->destRect.y - TFT_WIDTH));
    }
  }
  void selectItem() {
    selectedItem->OnEnter();
    if (!selectedItem->subItems.empty())
    { // サブアイテムを展開
      MenuItem* mi = selectedItem;
      selectedItem = selectedItem->subItems[0];
      _moving = true;
      // _force = true;
      UpdateDestRect();

      ScrollSubitemArea(mi);
    } else {
      MenuItem* mi = selectedItem;
      while (mi && mi != this && !mi->callback) {
        mi = mi->parentItem;
      }
      if (mi->callback) {
        mi->callback(selectedItem);
        _force = true;
      }
    }
  }
  void loop() {
    if (_moving || _force) {
      if (selectedItem->destRect.y < itemHeight/2) {
        DestRectYScroll(-selectedItem->destRect.y + itemHeight );
      }

      _moving = Move();
      if (!_cursorRect.equal(selectedItem->destRect)) { // カーソル移動
        const Rect& c = _cursorRect;
        Rect r = c.mixRect(selectedItem->destRect);
        if (c.y     < r.y    ) M5.Lcd.fillRect( c.x , c.y    , c.w, r.y - c.y          , backgroundColor);
        if (c.y+c.h > r.y+r.h) M5.Lcd.fillRect( c.x , r.y+r.h, c.w, (c.y+c.h)-(r.y+r.h), backgroundColor);
        if (c.x     < r.x    ) M5.Lcd.fillRect( c.x , c.y    , r.x-c.x            ,c.h , backgroundColor);
        if (c.x+c.w > r.x+r.w) M5.Lcd.fillRect( r.x+r.w, c.y , (c.x+c.w)-(r.x+r.w),c.h , backgroundColor);
        Draw(_force, selectedItem, &_cursorRect);
        _cursorRect = r;
        _moving = true;
        M5.Lcd.fillRect( _cursorRect.x+1, _cursorRect.y+1, _cursorRect.w-2, _cursorRect.h-2, cursorColor);
        if (destRect.h > 0) {
          selectedItem->DrawTitle();
          selectedItem->OnAfterDraw();
        }
      } else {
        Draw(_force, selectedItem);
      }
    } else {
      delay(16);
    }
    _force = false;
  }
};

#endif
