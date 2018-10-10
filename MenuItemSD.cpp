#include "MenuItemSD.h"
#include "SD.h"

void MenuItemSD::OnEnter() {
  DisposeSubItems();
  SD.begin();

  std::vector<MenuItem*> filesItems;
  File root = SD.open(path.length() ? path : "/");
  File file = root.openNextFile();
  MenuItemSD* mi;
  while (file) {
    String ptmp = file.name();
    mi = new MenuItemSD(ptmp, file.isDirectory(), ptmp.substring(path.length()));
    // ディレクトリは先に追加する
    if (file.isDirectory()) AddSubItem(mi);
    else filesItems.push_back(mi);
    file = root.openNextFile();
  }
  root.close();
  if (!filesItems.empty()) {
    // ファイルがあればまとめて追加する
    AddSubItems(filesItems);
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
