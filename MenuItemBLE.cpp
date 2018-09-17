#include <M5Stack.h>
#include <vector>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "MenuItemBLE.h"

void MenuItemBLE::ScanBLE()
{  
  M5.Lcd.fillRect( rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2, cursorColor);
  DrawTitle("BLE Scanning...");

  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true);   //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(1);

  int count = foundDevices.getCount();
  BLEAdvertisedDevice ad;
  DisposeSubItems();
  for ( int i = 0; i < count; ++i) {
    ad = foundDevices.getDevice(i);
    MenuItemBLE* mi = new MenuItemBLE((ad.getAddress().toString() + " " + ad.getName()).c_str());
    mi->pAddr = new BLEAddress(ad.getAddress());
    AddSubItem(mi);
  }
}
MenuItemBLE::~MenuItemBLE() {
  if (pClient && pClient->isConnected()) pClient->disconnect();
  pClient = 0;
}


/// Make SubItem from BLE Device Services
void MenuItemBLE::ScanServices()
{
  M5.Lcd.fillRect( rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2, cursorColor);
  DrawTitle("BLE Server Connecting...");

  pClient = BLEDevice::createClient();
  pClient->connect(*pAddr);

  M5.Lcd.fillRect( rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2, cursorColor);
  DrawTitle("BLE Server Connecting Done...");

  DisposeSubItems();

  std::map<std::string, BLERemoteService*>* pServices = pClient->getServices();
  std::map<std::string, BLERemoteService*>::iterator it = pServices->begin();
  std::string servName, strId;
  for (;it != pServices->end(); ++it) {
    strId = it->first.substr(0,8);
    uint32_t servId = strtol(strId.c_str(), 0, 16);
    if (servId < 0x10000) servName = strId.substr(4) + ":" + BLEUtils::gattServiceToString(servId);
    else servName = it->first;
    MenuItemBLE* mi = new MenuItemBLE(servName.c_str());
    mi->pRemoteServ = it->second;
    mi->pAddr = pAddr;
    AddSubItem(mi);
  }
}


/// Make SubItem from BLE Services Characteristic
void MenuItemBLE::ScanCharacteristic()
{
  DisposeSubItems();
  std::map<std::string, BLERemoteCharacteristic*>* pRemoteChars = pRemoteServ->getCharacteristics();
  std::map<std::string, BLERemoteCharacteristic*>::iterator it = pRemoteChars->begin();
  std::string servName, strId;
  for (;it != pRemoteChars->end(); ++it) {
    strId = it->first.substr(0,8);
    uint32_t servId = strtol(strId.c_str(), 0, 16);
    if (servId < 0x10000) servName = strId.substr(4) + ":" + BLEUtils::gattCharacteristicUUIDToString(servId);
    else servName = it->first;
    MenuItemBLE* mi = new MenuItemBLE(servName.c_str());
    mi->pRemoteChar = it->second;
    mi->pRemoteServ = pRemoteServ;
    mi->pAddr = pAddr;
    AddSubItem(mi);
  }
}

/*
// Make SubItem from BLE Characteristic Descriptor
void MenuItemBLE::ScanDescriptor()
{
  DisposeSubItems();
  std::map<std::string, BLERemoteDescriptor*>* pDescriptors = pRemoteChar->getDescriptors();
  std::map<std::string, BLERemoteDescriptor*>::iterator it = pDescriptors->begin();
  std::string servName, strId;
  for (;it != pDescriptors->end(); ++it) {
    strId = it->first.substr(0,8);
    uint32_t servId = strtol(strId.c_str(), 0, 16);
    if (servId < 0x10000) servName = strId.substr(4) + ":" + BLEUtils::gattDescriptorUUIDToString(servId);
    else servName = it->first;
    MenuItemBLE* mi = new MenuItemBLE(servName.c_str());
    mi->pDescriptor = it->second;
    AddSubItem(mi);
  }
}
//*/

void MenuItemBLE::BLEReadValue()
{
  M5.Lcd.fillRect( rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2, cursorColor);
  DrawTitle("BLE Read...");

  std::string value = pRemoteChar->readValue();

  M5.Lcd.fillRect( rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2, cursorColor);
  DrawTitle(value.c_str());
}


void MenuItemBLE::OnEnter() {
  if      (pRemoteChar) { if (pRemoteChar->canRead()) BLEReadValue(); }
  else if (pRemoteServ) { ScanCharacteristic(); }
  else if (pAddr) { ScanServices(); }
  else { ScanBLE(); }

  MenuItem::OnEnter();
}

void MenuItemBLE::OnAfterDraw() {
}
