
#include "MenuItem.h"

int8_t MenuItem::nestOffset = 10;
int8_t MenuItem::itemHeight = 20;
int16_t MenuItem::itemWidth = 200;
uint16_t MenuItem::fontColor = 0xFFFF;
uint16_t MenuItem::fillColor = 0x0410;
uint16_t MenuItem::frameColor = 0x630C;
uint16_t MenuItem::cursorColor = 0x421F;
uint16_t MenuItem::backgroundColor = 0x0000;
MenuItem* MenuItem::selectedItem = 0;


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

bool MenuItem::Move()
{
  moving = !rect.equal(destRect);
  if (moving) {
    prevRect = rect;
    rect = rect.mixRect(destRect);
  }
  bool sub = false;
  if (subItems) {
    for (uint8_t i = 0; subItems[i]; ++i) {
      sub = subItems[i]->Move() || sub;
    }
  }
  return moving || sub;
}

void MenuItem::DrawTitle()
{
  M5.Lcd.setTextColor(fontColor);
  M5.Lcd.setCursor( rect.x + 14, rect.y + rect.h / 3);
  M5.Lcd.print(title);
}
void MenuItem::OnAfterDraw()
{
  if (subItems && subItems[0]) {
    int x = rect.x + 5;
    int y = rect.y + rect.h / 2 - 3;
    if (subItems[0]->visible) {
      M5.Lcd.drawLine( x  , y  , x+4, y  , fontColor);
      M5.Lcd.drawLine( x+1, y+1, x+2, y+4, fontColor);
      M5.Lcd.drawLine( x+3, y+1, x+2, y+4 , fontColor);
    } else {
      M5.Lcd.drawLine( x  , y  , x  , y+4, fontColor);
      M5.Lcd.drawLine( x+1, y+1, x+4, y+2, fontColor);
      M5.Lcd.drawLine( x+1, y+3, x+4, y+2, fontColor);
    }
  }
}

MenuItem* MenuItem::Draw(bool force, const MenuItem* forceItem)
{
  int x,y,w,h;

  if (forceItem && (this == forceItem->parentItem || this->parentItem == forceItem->parentItem)) force = true;
  if ((moving || force) && rect.w && rect.h && rect.y > -itemHeight/2 && rect.y < TFT_WIDTH) {
    // 枠を描画
    y = max(0, rect.y);
    h = rect.h + min(0, rect.y);
    M5.Lcd.drawRect( rect.x, y, rect.w, h, frameColor);
    if (forceItem != this) {
      // 枠内を塗る
      M5.Lcd.fillRect( rect.x+1, y+1, rect.w-2, h-2, fillColor);
      if (rect.Bottom() > 0) {
        DrawTitle();
      }
    }
    OnAfterDraw();
  }
  MenuItem* res = this;
  if (subItems) {
    for (uint8_t i = 0; subItems[i]; ++i) {
      if (subItems[i]->visible) {
        res = subItems[i]->Draw(force, forceItem);
      }
    }
    if (res != this) { // subItems drawed
      // right bottom area erase
      h = max(0, res->prevRect.Bottom() - res->rect.Bottom());
      if (h) {
        y = res->rect.Bottom();
        x = rect.Right();
        M5.Lcd.fillRect( x, y, res->rect.Right() - x, h, backgroundColor);
      }
      y = max(0, subItems[0]->prevRect.y);
      h = max(0, subItems[0]->rect.y - y);
      if (h) {
        x = rect.Right();
        M5.Lcd.fillRect( x, y, subItems[0]->rect.Right() - x, h, backgroundColor);
      }

      if (moving || res->moving) {
      y = max(0, rect.Bottom());
      M5.Lcd.fillRect( rect.x
                     , y
                     , subItems[0]->rect.x - rect.x
                     , (res->rect.Bottom()) - y
                     , backgroundColor);
      }
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
    bool flg;
    do {
      M5.update();
      flg = M5.BtnC.isPressed();
      if (!flg || (++i / 20) % 2) {
        DrawTitle();
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

void MenuItemBoolean::OnEnter(){
  DrawSwitch(-1);
  do {
    M5.update();
    delay(5);
  } while (M5.BtnC.isPressed());
  value = !value;
  DrawSwitch(value);
}
void MenuItemBoolean::OnAfterDraw(){
  DrawSwitch(value);
}
