
#include <M5Stack.h>
#include "MenuContainer.h"
#include "MenuItemSD.h"

// 表示するメニュー情報の定義
MenuContainer _menu;
void setup() {
  
  M5.begin();
  digitalWrite(26,0);
  
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17 , OUTPUT);

  _menu.callback = CallBackMenuItem;
  _menu.SetSubItems(new MenuItem*[4]
                     { new MenuItem("sub1", new MenuItem*[4]
                       { new MenuItem("sub1-1")
                       , new MenuItem("sub1-2")
                       , new MenuItem("sub1-3")
                       , 0
                       }            )
                     , new MenuItem("bool items", CallBackMenuItemBoolean, new MenuItem*[5]
                       { new MenuItemBoolean("GPIO 2", 2)
                       , new MenuItemBoolean("GPIO 5", 5)
                       , new MenuItemBoolean("GPIO 16", 16)
                       , new MenuItemBoolean("GPIO 17", 17)
                       , 0
                       } )
                     , new MenuItemSD("SD card")
                     , 0
                     }
                   );
  _menu.begin();
}

void CallBackMenuItem(MenuItem* sender) 
{
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print(sender->title);
}

void CallBackMenuItemBoolean(MenuItem* sender) 
{
  MenuItemBoolean* mi = static_cast<MenuItemBoolean*>(sender);
  if (!mi) return;
  M5.Lcd.setCursor(0, 0);
  digitalWrite(mi->tag, mi->value ? true : false);
  M5.Lcd.printf("GPIO %d : %d", mi->tag, mi->value);
}


void loop() {
  M5.update();
  _menu.loop();

/*
  int index = _menuScene.loop();
  if (-1 != index) {
    ExecMenu(index);      // 該当メニュー選択実行
    //_menuScene.loop(true);
  }
  //*/
}
