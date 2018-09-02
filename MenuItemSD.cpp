#include "MenuItemSD.h"
#include "SD.h"

uint16_t MenuItemSD::maxFileCount = 100;

void MenuItemSD::OnEnter() {
  //M5.Lcd.setCursor( rect.x + rect.w / 3, rect.y + rect.h / 3);

  DisposeSubItems();
  SD.begin();

  MenuItem* items[maxFileCount + 1];
  File root = SD.open(path.length() ? path : "/");
  File file = root.openNextFile();
  uint16_t i;
  for (i = 0; file && i < maxFileCount; ++i) {
    String ptmp = file.name();
    items[i] = new MenuItemSD(ptmp, file.isDirectory(), ptmp.substring(path.length()));
    file = root.openNextFile();
  }
  root.close();
  items[i] = 0;

  if (i > 0) {
    MenuItem** sub = new MenuItem*[i + 1];
    memcpy(sub, items, i * (sizeof(MenuItem*)));
    sub[i] = 0;
    SetSubItems(sub);
  }
  MenuItem::OnEnter();
}

void MenuItemSD::OnAfterDraw() {
  if (path.length() == 0) return;
  Rect area ( rect.x + 5
            , rect.y + (rect.h - 10) / 2
            , 10
            , 10);
  if (isDir) {
    M5.Lcd.fillRect(area.x, area.y+1, area.w  , area.h-1, 0x0ff0);
    M5.Lcd.drawLine(area.x, area.y  , area.x+3, area.y  , 0x0ff0);
    M5.Lcd.drawLine(area.x, area.y+2, area.x+9, area.y+1, fillColor);
  } else {
    M5.Lcd.fillRect(area.x, area.y+1, area.w-1, area.h-1, 0xFFFF);
    M5.Lcd.drawLine(area.x, area.y  , area.x + area.w-3, area.y, 0xFFFF);
    M5.Lcd.drawLine(area.x+9, area.y+3, area.x+9, area.y+9, 0xFFFF);
    M5.Lcd.drawLine(area.x+6, area.y  , area.x+6, area.y+3, 0);
    M5.Lcd.drawLine(area.x+6, area.y+3, area.x+9, area.y+3, 0);
    M5.Lcd.drawLine(area.x+2, area.y+3, area.x+4, area.y+3, 0);
    M5.Lcd.drawLine(area.x+2, area.y+5, area.x+7, area.y+5, 0);
    M5.Lcd.drawLine(area.x+2, area.y+7, area.x+7, area.y+7, 0);
  }
}
