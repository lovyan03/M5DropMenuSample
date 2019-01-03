
#include "MenuItem.h"
#undef min
#include <algorithm>

int8_t MenuItem::nestOffset = 10;
int8_t MenuItem::itemHeight = 20;
int16_t MenuItem::itemWidth = 240;
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

MenuItem::MenuItem()
: title("")
, callback(0)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(0)
, parentItem(0)
, moving(false)
{
}

MenuItem::MenuItem(const String& titleStr, int tg, std::function<void(MenuItem*)> cb)
: title(titleStr)
, callback(cb)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(tg)
{
}
MenuItem::MenuItem(const String& titleStr, std::function<void(MenuItem*)> cb)
: title(titleStr)
, callback(cb)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(0)
{
}

MenuItem::MenuItem(const String& titleStr, int tg, std::function<void(MenuItem*)> cb, const std::vector<MenuItem*> &sub)
: title(titleStr)
, callback(cb)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(tg)
{
  SetSubItems(sub);
}
MenuItem::MenuItem(const String& titleStr, std::function<void(MenuItem*)> cb, const std::vector<MenuItem*> &sub)
: title(titleStr)
, callback(cb)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(0)
{
  SetSubItems(sub);
}

MenuItem::MenuItem(const String& titleStr, const std::vector<MenuItem*> &sub)
: title(titleStr)
, destRect(0,0,itemWidth, itemHeight)
, visible(false)
, tag(0)
{
  SetSubItems(sub);
}


void MenuItem::AddSubItem(MenuItem* mi)
{
  mi->parentItem = this;
  subItems.push_back(mi);
}

void MenuItem::AddSubItems(const std::vector<MenuItem*> &sub)
{
  subItems.reserve(subItems.size() + sub.size());
  std::copy(sub.begin(), sub.end(),std::back_inserter(subItems));
  for (std::vector<MenuItem*>::const_iterator it = sub.begin(); it != sub.end(); ++it) {
    (*it)->parentItem = this;
  }
}

void MenuItem::SetSubItems(const std::vector<MenuItem*> &sub)
{
  subItems.assign(sub.begin(), sub.end());
  for (std::vector<MenuItem*>::iterator it = subItems.begin(); it != subItems.end(); ++it) {
    (*it)->parentItem = this;
  }
}

void MenuItem::DisposeSubItems() {
  for (uint16_t i = 0; i != subItems.size(); ++i) {
    subItems[i]->DisposeSubItems();
    delete subItems[i];
    subItems[i] = 0;
  }
  subItems.clear();
}
void MenuItem::ParentUpdateDestRect()
{
  if (parentItem) parentItem->ParentUpdateDestRect();
  else UpdateDestRect();
}

int16_t MenuItem::UpdateDestRect(int16_t x, int16_t y) {
  if (destRect.x != x || destRect.y != y) {
    destRect.x = x;
    destRect.y = y;
  }
  x += nestOffset;
  y = destRect.y + destRect.h;
  for (uint16_t i = 0; i != subItems.size(); ++i) {
    if (!subItems[i]->visible) continue;
    y = subItems[i]->UpdateDestRect(x, y);
  }
  return y;
}
void MenuItem::DestRectYScroll(int16_t add_y)
{
  destRect.y += add_y;
  for (uint16_t i = 0; i != subItems.size(); ++i) {
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
  for (uint16_t i = 0; i != subItems.size(); ++i) {
    sub = subItems[i]->Move() || sub;
  }
  return moving || sub;
}

void MenuItem::DrawTitle()
{
  M5.Lcd.setTextColor(fontColor);

  if (36 < title.length()) {
    M5.Lcd.setCursor( rect.x + 14, rect.y + 1);
    M5.Lcd.print(title.substring(0,36));
    M5.Lcd.setCursor( rect.x + 14, rect.y + rect.h / 2 + 1);
    M5.Lcd.print(title.substring(36));
  } else {
    M5.Lcd.setCursor( rect.x + 14, rect.y + rect.h / 3);
    M5.Lcd.print(title);
  }
}
void MenuItem::DrawTitle(const String& str)
{
  M5.Lcd.setTextColor(fontColor);
  M5.Lcd.setCursor( rect.x + 14, rect.y + rect.h / 3);
  M5.Lcd.print(str);
}
void MenuItem::OnAfterDraw()
{
  if (!subItems.empty()) {
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

MenuItem* MenuItem::Draw(bool force, const MenuItem* forceItem, const Rect* forceRect)
{
  int16_t x,y,h;

  bool collision = (forceRect && rect.isCollision(*forceRect));
  if ((moving || force || collision) && rect.w && rect.h && rect.y > -itemHeight/2 && rect.y < TFT_WIDTH) {
    // draw frame line
    y = std::max(rect.y, (int16_t)0);
    h = rect.h + std::min(rect.y, (int16_t)0);
    M5.Lcd.drawRect( rect.x, y, rect.w, h, frameColor);
    // fill item body
    M5.Lcd.fillRect( rect.x+1, y+1, rect.w-2, h-2, this == forceItem ? cursorColor : fillColor);
    if (rect.Bottom() > 0) {
      DrawTitle();
    }
    OnAfterDraw();
  }
  MenuItem* res = this;
  if (!subItems.empty()) {
    for (uint16_t i = 0; i != subItems.size(); ++i) {
      if (subItems[i]->visible) {
        res = subItems[i]->Draw(force, forceItem, forceRect);
      }
    }
    if (res != this) { // subItems drawed
      h = std::max(0, res->prevRect.Bottom() - res->rect.Bottom());
      if (h)
      { // bottom side erase
        y = res->rect.Bottom();
        x = rect.Right();
        M5.Lcd.fillRect( x, y, res->rect.Right() - x, h, backgroundColor);
      }
      y = std::max(subItems[0]->prevRect.y, (int16_t)0);
      h = std::max(subItems[0]->rect.y - y, 0);
      if (h)
      { // right side erase
        x = rect.Right();
        M5.Lcd.fillRect( x, y, subItems[0]->rect.Right() - x, h, backgroundColor);
      }

      if (moving || res->moving)
      { // left side erase
      y = std::max(rect.Bottom(), (int16_t)0);
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
  if (!subItems.empty()) {
    for (uint16_t i = 0; i != subItems.size(); ++i) {
      subItems[i]->rect = rect;
      if (!subItems[i]->visible) {
        subItems[i]->visible = true;
      }
      subItems[i]->destRect.h = itemHeight;
      subItems[i]->rect.h = 0;
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

