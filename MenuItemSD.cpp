#include "MenuItemSD.h"
#include "SD.h"

void MenuItemSD::OnEnter() {
  //M5.Lcd.setCursor( rect.x + rect.w / 3, rect.y + rect.h / 3);

  DisposeSubItems();
  SD.begin();

  std::vector<MenuItemSD*> items;
  File root = SD.open(path.length() ? path : "/");
  File file = root.openNextFile();
  uint16_t count;
  uint16_t dirCount = 0;
  while (file) {
    String ptmp = file.name();
    items.push_back(new MenuItemSD(ptmp, file.isDirectory(), ptmp.substring(path.length())));
    if (file.isDirectory()) ++dirCount;
    file = root.openNextFile();
  }
  root.close();
  if (!items.empty()) {
    std::vector<MenuItem*> sub(items.size());
    int iFile = dirCount - 1, iDir = -1;
    for (int i = 0; i < count; ++i) {
      if (items[i]->isDir) {
        sub[++iDir] = items[i];
      } else {
        sub[++iFile] = items[i];
      }
    }
    sub[count] = 0;
    SetSubItems(sub);
  }
  MenuItem::OnEnter();
}

void MenuItemSD::OnAfterDraw() {
  if (path.length() == 0) return;
  Rect area ( rect.x + 11
            , rect.y + (rect.h - 8) / 2
            , 8
            , 8);
  if (isDir) {
    M5.Lcd.fillRect(area.x, area.y+1, area.w  , area.h-1, 0x0ff0);
    M5.Lcd.drawLine(area.x, area.y  , area.x+2, area.y  , 0x0ff0);
    M5.Lcd.drawLine(area.x, area.y+2, area.x+7, area.y+1, fillColor);
  } else {
    M5.Lcd.fillRect(area.x, area.y+1, area.w, area.h - 1, 0xFFFF);
    M5.Lcd.drawLine(area.x, area.y  , area.x+area.w - 2, area.y, 0xFFFF);
    M5.Lcd.drawLine(area.x+5, area.y  , area.x+5, area.y+2, 0);
    M5.Lcd.drawLine(area.x+5, area.y+2, area.x+7, area.y+2, 0);
    M5.Lcd.drawLine(area.x+1, area.y+2, area.x+3, area.y+2, 0);
    M5.Lcd.drawLine(area.x+1, area.y+4, area.x+6, area.y+4, 0);
    M5.Lcd.drawLine(area.x+1, area.y+6, area.x+6, area.y+6, 0);
  }
}
