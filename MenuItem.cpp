
#include "MenuItem.h"

uint8_t MenuItem::nestOffset = 10;
uint8_t MenuItem::itemHeight = 20;
uint16_t MenuItem::itemWidth = 200;
uint16_t MenuItem::fontColor = 0xFFFF;
uint16_t MenuItem::fillColor = 0x0410;
uint16_t MenuItem::frameColor = 0x841F;
uint16_t MenuItem::cursorColor = 0x421F;
uint16_t MenuItem::backgroundColor = 0x0000;


bool operator==(const Rect& lhs, const Rect& rhs) {
  return lhs.equal(rhs);
}
bool operator!=(const Rect& lhs, const Rect& rhs) {
  return !(lhs == rhs);
}

MenuItem::MenuItem() {}

MenuItem::MenuItem(const String& titleStr, int tg, CALLBACK_MENUITEM cb, MenuItem* sub[])
: title(titleStr)
, callback(cb)
, subItems(sub)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(tg)
{
  SetSubItems(sub);
}
MenuItem::MenuItem(const String& titleStr, CALLBACK_MENUITEM cb, MenuItem* sub[])
: title(titleStr)
, callback(cb)
, subItems(sub)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(0)
{
  SetSubItems(sub);
}

MenuItem::MenuItem(const String& titleStr, MenuItem* sub[])
: title(titleStr)
, subItems(sub)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(0)
{
  SetSubItems(sub);
}
void MenuItem::SetSubItems(MenuItem* sub[])
{
  subItems = sub;
  if (!sub) return;
  for (uint8_t i = 0; subItems[i]; ++i) {
    subItems[i]->parentItem = this;
  }
}
void MenuItem::DisposeSubItems() {
  if (!subItems) return;
  for (int i = 0; subItems[i]; ++i) {
    subItems[i]->DisposeSubItems();
    delete subItems[i];
    subItems[i] = 0;
  }
  if (subItems) {
    delete[] subItems;
  }
  subItems = 0;
}

int16_t MenuItem::UpdateDestRect(int16_t x, int16_t y) {
  if (destRect.x != x || destRect.y != y) {
    destRect.x = x;
    destRect.y = y;
  }
  x += nestOffset;
  y = destRect.y + destRect.h;
  for (uint8_t i = 0; subItems && subItems[i]; ++i) {
    if (!subItems[i]->visible) continue;
    y = subItems[i]->UpdateDestRect(x, y);
  }
  return y;
}
void MenuItem::DestRectYScroll(int16_t add_y)
{
  destRect.y += add_y;
  for (uint8_t i = 0; subItems && subItems[i]; ++i) {
    if (subItems[i]->visible) subItems[i]->DestRectYScroll(add_y);
  }
}

bool Rect::MoveAndErase(const Rect& destRect, uint16_t bgcol, bool force) {
  bool res = (destRect != *this);
  if (force || res) {
    Rect r = mixRect(destRect);

    //if (y   < r.y    ) M5.Lcd.fillRect( x , y      , w, r.y - y        , bgcol);
    if (y+h > r.y+r.h) M5.Lcd.fillRect( x , r.y+r.h, w, (y+h)-(r.y+r.h), bgcol);
    //if (x   < r.x    ) M5.Lcd.fillRect( x , y      , r.x-x          ,h , bgcol);
    //if (x+w > r.x+r.w) M5.Lcd.fillRect( r.x+r.w, y , (x+w)-(r.x+r.w),h , bgcol);

    *this = r;
  }
  return res;
}
bool MenuItem::Draw(bool force, const MenuItem* forceItem)
{
  bool res = rect.MoveAndErase(destRect, backgroundColor, force);
  if (forceItem && (this == forceItem->parentItem || this->parentItem == forceItem->parentItem)) force = true;
  if ((res || force) && rect.w && rect.h) {
    M5.Lcd.drawRect( rect.x, rect.y, rect.w, rect.h, frameColor);
    if (forceItem != this) {
      M5.Lcd.fillRect( rect.x+1, rect.y+1, rect.w-2, rect.h-2, fillColor);
      M5.Lcd.setTextColor(0xFFFF, fillColor);
      M5.Lcd.setCursor( rect.x + 10, rect.y + rect.h / 3);
      M5.Lcd.print(title);
    }
    OnAfterDraw();
  }
  if (subItems) {
    uint8_t i = 0;
    bool subDraw = false;
    for (; subItems[i]; ++i) {
      if (!subItems[i]->visible) continue;
      subDraw = true;
      res = subItems[i]->Draw(force, forceItem) || res;
    }
    if (res && subDraw) {
      --i;
      M5.Lcd.fillRect( rect.x+rect.w, 0, nestOffset, rect.y + rect.h, backgroundColor);
      M5.Lcd.fillRect( rect.x
                     , rect.y+rect.h
                     , subItems[i]->rect.x - rect.x
                     , (subItems[i]->rect.y+subItems[i]->rect.h)-(rect.y+rect.h)
                     , backgroundColor);
    }
  }
  return res;
}

void MenuItem::Hide() {
  visible = false;
  M5.Lcd.fillRect( rect.x, rect.y, rect.w, rect.h, backgroundColor);
}

void MenuItem::OnEnter() {
  if (subItems && subItems[0]) {
    for (uint8_t i = 0; subItems[i]; ++i) {
      subItems[i]->rect = rect;
      if (!subItems[i]->visible) {
        subItems[i]->visible = true;
        subItems[i]->rect.h = 0;
      }
    }
  } else {
    int i = 0;
    M5.Lcd.setTextColor(fontColor, cursorColor);
    bool flg;
    do {
      M5.update();
      flg = M5.BtnC.isPressed();
      if (!flg || (++i / 20) % 2) {
        M5.Lcd.setCursor( rect.x + 10, rect.y + rect.h / 3);
        M5.Lcd.print(title);
      } else {
        M5.Lcd.fillRect( rect.x+1, rect.y+1, rect.w-2, rect.h-2, cursorColor);
      }
      delay(5);
    } while ( flg );
  }
}
  
void MenuItemBoolean::DrawSwitch(int mode)
{
  Rect area ( rect.x + rect.w * 3 / 4
            , rect.y + rect.h  / 6
            , rect.w * 1 / 6
            , rect.h * 2 / 3);
  M5.Lcd.drawRect(area.x, area.y, area.w, area.h, backgroundColor);
  area.x += 1;  area.y += 1;  area.w -= 2;  area.h -= 2;  
  M5.Lcd.fillRect(area.x, area.y,area.w, area.h, fillColor);
  switch (mode){
  case 0:    M5.Lcd.fillRect(area.x           , area.y, area.w * 2/3, area.h, frameColor);  break;
  default:   M5.Lcd.fillRect(area.x + area.w/6, area.y, area.w * 2/3, area.h, frameColor);  break;
  case 1:    M5.Lcd.fillRect(area.x + area.w/3, area.y, area.w * 2/3, area.h, fontColor);   break;
  }
}
  
void MenuItemBoolean::OnEnter(){
  DrawSwitch(-1);
  do {
    M5.update();
    delay(5);
  } while (M5.BtnC.isPressed());
  value = !value;
  OnAfterDraw();
}
void MenuItemBoolean::OnAfterDraw(){
  DrawSwitch(value);
}
