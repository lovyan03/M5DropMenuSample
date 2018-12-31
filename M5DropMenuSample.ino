#include <vector>
#include <SD.h>
#include <M5Stack.h>
#include "GroveJoystick.h"
#include "FACESGameBoy.h"
#include "PLUSEncoder.h"

#include "ButtonDrawer.h"
#include "MenuContainer.h"
#include "MenuItemSD.h"
#include "MenuItemBLE.h"
#include "Demo\I2CScanner.h"
#include "Demo\AD34FFTTaskDemo.h"
#include "Demo\ADInputDemo.h"
#include "Demo\DHT12Demo.h"
#include "Demo\MPU9250Demo.h"
#include "Demo\BLEDemo.h"
#include "Demo\ScrollDemo.h"
#include "VroomCtrlDemo.h"

MenuContainer _menu;

void CallBackMenuItem(MenuItem* sender) 
{
  M5.Lcd.fillRect(0,0,320,8,0);
  M5.Lcd.setTextColor(0xffff,0);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(15, 0);
  M5.Lcd.print("CallbackSample : ");
  M5.Lcd.print(sender->title);
}

void setup() {
  M5.begin();

  typedef std::vector<MenuItem*> submenu;
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
                     , new MenuItem("Demo", submenu
                       { new MenuItem("I2C Scanner", I2CScanner() )
                       , new MenuItem("FIRE/GO AD34Mic FFT MultiTask", AD34FFTTaskDemo() )
                       , new MenuItem("AD35 AD36 Input", ADInputDemo() )
                       , new MenuItem("DHT12"  , DHT12Demo() )
                       , new MenuItem("MPU9250", MPU9250Demo() )
                       , new MenuItem("Scroll Demo", ScrollDemo())
                       , new MenuItem("BLE Vroom Controller", VroomCtrlDemo() )
                       } )
                     , new MenuItem("GPIO switch", CallBackGPIOtest, submenu
                       { new MenuItemBoolean("GPIO16",16)
                       , new MenuItemBoolean("GPIO17",17)
                       , new MenuItemBoolean("GPIO 2", 2)
                       , new MenuItemBoolean("GPIO 5", 5)
                       } )
                     , new MenuItemBLE("BLE"   , BLEDemo() )
                     , new MenuItemSD("SD card", CallBackSDtest)
                     }
                   );

  _menu.begin();
  Wire.begin();
  ButtonDrawer::getInstance()->setTitle("Back","Next","Ok");
}


void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) { _menu.moveUp();     }
  if (M5.BtnB.wasPressed()) { _menu.moveNext();   }
  if (M5.BtnC.wasPressed()) { _menu.selectItem(); }

#ifdef _GroveJoystick_h_
  JoyStick.update();
  if (JoyStick.wasLeft())    _menu.moveUp();
  if (JoyStick.wasDown())    _menu.moveNext();
  if (JoyStick.wasUp())      _menu.movePrev();
  if (JoyStick.wasRight()
   || JoyStick.wasPressed()) _menu.selectItem();
#endif
#ifdef _FACESGameBoy_h_
  FacesGameBoy.update();
  if (FacesGameBoy.wasLeft()
   || FacesGameBoy.wasPressedB()) _menu.moveUp();
  if (FacesGameBoy.wasDown())     _menu.moveNext();
  if (FacesGameBoy.wasUp())       _menu.movePrev();
  if (FacesGameBoy.wasRight()
   || FacesGameBoy.wasPressedA()) _menu.selectItem();
#endif
#ifdef _PLUSEncoder_h_
  PlusEncoder.update();
  if (PlusEncoder.isLongClick()) _menu.moveUp();
  if (PlusEncoder.wasDown())     _menu.moveNext();
  if (PlusEncoder.wasUp())       _menu.movePrev();
  if (PlusEncoder.isClick())     _menu.selectItem();
#endif

  ButtonDrawer::getInstance()->draw();
  _menu.loop();
}

void CallBackGPIOtest(MenuItem* sender) 
{
  MenuItemBoolean* mi = static_cast<MenuItemBoolean*>(sender);
  if (!mi) return;

  pinMode(mi->tag, OUTPUT);
  digitalWrite(mi->tag, mi->value);
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
    ButtonDrawer btnDrawer("Back","","");
    for(int i=0; i<len; ++i){
      M5.Lcd.write(buf[i]);
    }
    btnDrawer.draw();
    while (!M5.BtnA.isPressed()) {
      M5.update();
    }
  }
}
