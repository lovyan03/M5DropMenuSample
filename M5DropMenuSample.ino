
#include <M5Stack.h>
#include "MenuContainer.h"
#include "MenuItemSD.h"

MenuContainer _menu;

void setup() {
  
  M5.begin();
  digitalWrite(25,0); // anti speaker noise.
  

  _menu.callback = CallBackMenuItem;
  _menu.SetSubItems(new MenuItem*[4]
                     { new MenuItem("tree sample", new MenuItem*[4]
                       { new MenuItem("sub 1", new MenuItem*[4]
                         { new MenuItem("sub 1-1")
                         , new MenuItem("sub 1-2")
                         , new MenuItem("sub 1-3")
                         , 0
                         }            )
                       , new MenuItem("sub 2")
                       , new MenuItem("sub 3")
                       , 0
                       }            )
                     , new MenuItem("GPIO switch", CallBackGPIOtest, new MenuItem*[5]
                       { new MenuItemBoolean("GPIO16",16)
                       , new MenuItemBoolean("GPIO17",17)
                       , new MenuItemBoolean("GPIO 2", 2)
                       , new MenuItemBoolean("GPIO 5", 5)
                       , 0
                       } )
                     , new MenuItemSD("SD card")
                     , 0
                     }
                   );
  _menu.begin();
}
void loop() {
  M5.update();
  _menu.loop();
}

void CallBackMenuItem(MenuItem* sender) 
{
  M5.Lcd.fillRect(0,0,320,8,0);
  M5.Lcd.setCursor(15, 0);
  M5.Lcd.print(sender->title);
}

void CallBackGPIOtest(MenuItem* sender) 
{
  MenuItemBoolean* mi = static_cast<MenuItemBoolean*>(sender);
  if (!mi) return;

  pinMode(mi->tag, OUTPUT);
  digitalWrite(mi->tag, mi->value);
}
