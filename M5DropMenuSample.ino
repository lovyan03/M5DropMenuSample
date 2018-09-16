
#include <M5Stack.h>
#include <vector>
#include <SD.h>
#include "MenuContainer.h"
#include "MenuItemSD.h"
#include "MenuItemBLE.h"
#include "Demo\ADInputDemo.h"
#include "Demo\DHT12Demo.h"
#include "Demo\MPU9250Demo.h"
#include "Demo\BLEDemo.h"
#include "VroomCtrlDemo.h"

typedef std::vector<MenuItem*> submenu;
MenuContainer _menu;

void setup() {
  M5.begin();

  _menu.callback = CallBackMenuItem;
  _menu.SetSubItems(submenu
                     { new MenuItem("tree sample", submenu
                       { new MenuItem("sub 1", submenu
                         { new MenuItem("sub 1-1")
                         , new MenuItem("sub 1-2")
                         , new MenuItem("sub 1-3")
                         } )
                       , new MenuItem("sub 2")
                       , new MenuItem("sub 3", submenu
                         { new MenuItem("sub 3-1", submenu
                           { new MenuItem("sub 3-1-1")
                           , new MenuItem("sub 3-1-2")
                           , new MenuItem("sub 3-1-3")
                           } )
                         , new MenuItem("sub 3-2")
                         , new MenuItem("sub 3-3", submenu
                           { new MenuItem("sub 3-3-1", submenu
                             { new MenuItem("sub 3-3-1-1")
                             , new MenuItem("sub 3-3-1-2")
                             , new MenuItem("sub 3-3-1-3")
                             } )
                           , new MenuItem("sub 3-1-2")
                           , new MenuItem("sub 3-1-3")
                           } )
                         } )
                       } )
                     , new MenuItem("Sensor Demo", submenu
                       { new MenuItem("AD35 AD36 Input", CallBackADInput)
                       , new MenuItem("DHT12"  , CallBackDHT12)
                       , new MenuItem("MPU9250", CallBackMPU9250)
                       , new MenuItem("BLE Vroom Controller", CallBackVroom)
                       } )
                     , new MenuItem("GPIO switch", CallBackGPIOtest, submenu
                       { new MenuItemBoolean("GPIO16",16)
                       , new MenuItemBoolean("GPIO17",17)
                       , new MenuItemBoolean("GPIO 2", 2)
                       , new MenuItemBoolean("GPIO 5", 5)
                       } )
                     , new MenuItemBLE("BLE"   , CallBackBLEDemo)
                     , new MenuItemSD("SD card", CallBackSDtest)
                     }
                   );
//*/
  _menu.begin();

  Wire.begin();
}
void loop() {
  M5.update();
  DrawButtons("   Back","   Next","    Ok");
  delay(1);
  _menu.loop();
}

void CallBackMenuItem(MenuItem* sender) 
{
  M5.Lcd.fillRect(0,0,320,8,0);
  M5.Lcd.setCursor(15, 0);
  M5.Lcd.print("select menu is : ");
  M5.Lcd.print(sender->title);
}

void CallBackGPIOtest(MenuItem* sender) 
{
  MenuItemBoolean* mi = static_cast<MenuItemBoolean*>(sender);
  if (!mi) return;

  pinMode(mi->tag, OUTPUT);
  digitalWrite(mi->tag, mi->value);
}

void CallBackADInput(MenuItem* sender) 
{
  ADInputDemo obj;
  obj.setup();
  while (!M5.BtnA.isPressed()) {
    M5.update();
    obj.loop();
    DrawButtons("   Back","","");
  }
  M5.Lcd.fillScreen(0);
}

void CallBackDHT12(MenuItem* sender) 
{
  DHT12Demo obj;
  obj.setup();
  while (!M5.BtnA.isPressed()) {
    M5.update();
    obj.loop();
    DrawButtons("   Back","","");
  }
  M5.Lcd.fillScreen(0);
}

void CallBackMPU9250(MenuItem* sender) 
{
  MPU9250Demo obj;
  if (obj.setup()) {
    while (!M5.BtnA.isPressed()) {
      M5.update();
      obj.loop();
      DrawButtons("   Back","","");
    }
  }
  M5.Lcd.fillScreen(0);
}

void CallBackVroom(MenuItem* sender) 
{
  VroomCtrlDemo obj;
  if (obj.setup()) {
    while (!M5.BtnA.isPressed()) {
      M5.update();
      obj.loop();
      DrawButtons("   Back","","");
    }
    obj.close();
  }
  M5.Lcd.fillScreen(0);
}

void CallBackBLEDemo(MenuItem* sender) 
{
  MenuItemBLE* mi = static_cast<MenuItemBLE*>(sender);
  if (!mi) return;
  if (!mi->pRemoteChar) return;
  BLEDemo obj;
  obj.setup(mi->pRemoteChar);
  while (!M5.BtnA.isPressed()) {
    M5.update();
    obj.loop();
    DrawButtons("   Back","","");
  }
  M5.Lcd.fillScreen(0);
}

void CallBackSDtest(MenuItem* sender) 
{
  MenuItemSD* mi = static_cast<MenuItemSD*>(sender);
  if (!mi) return;

  SD.begin();
  File file = SD.open(mi->path, FILE_READ);
  if (!file.isDirectory()) {
    FileView(file);
  }
  file.close();
  M5.Lcd.fillScreen(0);
}

void FileView(File ff){
  M5.Lcd.clear(0);
  M5.Lcd.setTextColor(0xffff,0);
  M5.Lcd.setCursor(0,0);
    size_t len = ff.size();
  uint8_t buf[256];
   
  if (ff.read(buf, len)) {
    for(int i=0; i<len; ++i){
      M5.Lcd.write(buf[i]);
    }
    DrawButtons("   Back","","");
    while (!M5.BtnA.isPressed()) {
      M5.update();
    }
  }
}

void DrawButton(Rect r, uint16_t color, uint16_t bgColor, const char* title)
{
  M5.Lcd.drawRect(r.x+1,r.y  ,r.w-2,r.h   ,color);
  M5.Lcd.drawRect(r.x  ,r.y+1,r.w  ,r.h-2 ,color);
  r.Inflate(-2);
  int h = (r.h - 8)/2;
  int w = strlen(title) * 6;
  M5.Lcd.fillRect(r.x+w, r.y, r.w-w, r.h, bgColor);
  if (h > 0) {
    M5.Lcd.fillRect(r.x, r.y,       w, h, bgColor);
    M5.Lcd.fillRect(r.x, r.y+r.h-h, w, h, bgColor);
  }
  M5.Lcd.setCursor(r.x, r.y + h);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(0xffff, bgColor);
  M5.Lcd.print(title);
}
void DrawButtons(const char* btnA,const char* btnB,const char* btnC)
{       
  Rect r(32, 226, 64, 14);
  DrawButton(r, M5.BtnA.isPressed() ? 0xffff:0xA514, 0, btnA);
  r.x += 96;
  DrawButton(r, M5.BtnB.isPressed() ? 0xffff:0xA514, 0, btnB);
  r.x += 96;
  DrawButton(r, M5.BtnC.isPressed() ? 0xffff:0xA514, 0, btnC);
}
