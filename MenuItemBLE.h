#ifndef _MenuItemBLE_H_
#define _MenuItemBLE_H_

#include "MenuItem.h"
#include "esp_bt_defs.h"

class BLEAddress;
class BLEClient;
class BLERemoteService;
class BLERemoteCharacteristic;
class BLERemoteDescriptor;
class MenuItemBLE : public MenuItem {
public:
  MenuItemBLE(const String& titleStr, std::function<void(MenuItem*)>cb = 0)
  : MenuItem(titleStr, cb), pAddr(0), pClient(0), pRemoteServ(0), pRemoteChar(0) {};

  virtual ~MenuItemBLE();
  BLEAddress* pAddr;
  BLEClient* pClient;
  BLERemoteService* pRemoteServ;
  BLERemoteCharacteristic* pRemoteChar;
  BLERemoteDescriptor* pDescriptor;
  void ScanBLE();
  void ScanServices();
  void ScanCharacteristic();
  void BLEReadValue();

  virtual void OnEnter();
  virtual void OnAfterDraw();
};

#endif
