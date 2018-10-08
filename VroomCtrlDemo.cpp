
#include "VroomCtrlDemo.h"
#include <M5Stack.h>
#undef min
#include <algorithm>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanSec = 30;

// vroomコントローラのサービスUUID
const static BLEUUID uuidVroomServ("500c0001-164a-4d7a-a6cc-57301b115071");
const static BLEUUID uuidVroomChar("500c0002-164a-4d7a-a6cc-57301b115071");

static BLEAddress* pServerAddr = 0;
static BLEClient* pClient = 0;
static BLERemoteCharacteristic* pRemoteChar = 0;
int16_t qw,qx,qy,qz; // Quaternion
int16_t ax,ay,az;    // accel
int16_t gx,gy,gz;    // gyro
int16_t sx,sy;       // stick
byte btn;            // button
int seq, prevSeq;

static void notifyCallback(BLERemoteCharacteristic*, uint8_t* b, size_t length, bool isNotify) {
  if (length < 20) return;
  qw = b[0] << 8 | b[1];
  qx = b[2] << 8 | b[3];
  qy = b[4] << 8 | b[5];
  qz = b[6] << 8 | b[7];
  ax = ( b[ 8]       << 8) | (b[ 9] & 0xf0);
  ay = ((b[ 9]&0x0f) <<12) | (b[10] << 4  );
  az = ( b[11]       << 8) | (b[12] & 0xf0);
  gx = ((b[12]&0x0f) <<12) | (b[13] << 4  );
  gy = ( b[14]       << 8) | (b[15] & 0xf0);
  gz = ((b[15]&0x0f) <<12) | (b[16] << 4  );
  sx = b[19]&0xff;
  sy = b[18]&0xff;
  btn= b[17];
  ++seq;
}

class advdCallback: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice ad) {
    if (!ad.haveServiceUUID()
     || !ad.getServiceUUID().equals(uuidVroomServ)) {
      return;
    }
    pServerAddr = new BLEAddress(ad.getAddress());
    ad.getScan()->stop();
  }
};

VroomCtrlDemo::VroomCtrlDemo()
{
}

bool VroomCtrlDemo::setup()
{
  M5.Lcd.fillScreen(0);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("BLE Scanning...");

  BLEDevice::init("");
  advdCallback* adCB = new advdCallback();
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(adCB);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(scanSec);
  pBLEScan->setAdvertisedDeviceCallbacks(0);
  delete adCB;

  if (pServerAddr) {
    pClient = BLEDevice::createClient();
    M5.Lcd.print("Found.\nConnecting...");
    if (pClient->connect(*pServerAddr)) {
      M5.Lcd.print("done.\nCharacteristic...");
      pRemoteChar = pClient->getService(uuidVroomServ)->getCharacteristic(uuidVroomChar);
      pRemoteChar->registerForNotify(notifyCallback);
      M5.Lcd.print("done.\n");
      return true;
    } else {
      M5.Lcd.print("error.");
      delete pClient ;
      pClient = 0;
    }
  } else {
    M5.Lcd.println("not found...");
  }
  delay(500);
  return false;
}
void VroomCtrlDemo::close() 
{
  if (pRemoteChar) {
    try {
      pRemoteChar->registerForNotify(0);
    } catch(...){}
    pRemoteChar = 0;
    if (pClient) {
      try {
        if (pClient->isConnected()) {
          pClient->disconnect();
        }
      } catch(...){}
      pClient = 0;
    }
  }
  if (pServerAddr) {
    delete pServerAddr;
    pServerAddr = 0;
  }
}

bool VroomCtrlDemo::loop() 
{
  if (prevSeq == seq) {
    if (!pClient->isConnected()) {
      return  false;
    }
    if (!pRemoteChar) {
      return false;
    }
    return true;
  }
  prevSeq = seq;

  M5.Lcd.setCursor(0, 60); M5.Lcd.printf("qw:%04x", (uint16_t)qw);
  M5.Lcd.setCursor(0, 70); M5.Lcd.printf("qx:%04x", (uint16_t)qx);
  M5.Lcd.setCursor(0, 80); M5.Lcd.printf("qy:%04x", (uint16_t)qy);
  M5.Lcd.setCursor(0, 90); M5.Lcd.printf("qz:%04x", (uint16_t)qz);

  M5.Lcd.setCursor(0,110); M5.Lcd.printf("ax:%04x", (uint16_t)ax);
  M5.Lcd.setCursor(0,120); M5.Lcd.printf("ay:%04x", (uint16_t)ay);
  M5.Lcd.setCursor(0,130); M5.Lcd.printf("az:%04x", (uint16_t)az);
  
  M5.Lcd.setCursor(0,150); M5.Lcd.printf("gx:%04x", (uint16_t)gx);
  M5.Lcd.setCursor(0,160); M5.Lcd.printf("gy:%04x", (uint16_t)gy);
  M5.Lcd.setCursor(0,170); M5.Lcd.printf("gz:%04x", (uint16_t)gz);
  
  M5.Lcd.setCursor(0,190);  M5.Lcd.printf("SX:%02x", (uint8_t)sx);
  M5.Lcd.setCursor(0,200);  M5.Lcd.printf("SY:%02x", (uint8_t)sy);
  M5.Lcd.setCursor(0,210);  M5.Lcd.printf("Btn:%02x", btn);

  M5.Lcd.fillRect(42, 60,278,8,0);  M5.Lcd.fillRect(180+(qw<0?qw/256:0), 60, abs(qw/256), 7, 0xffff);
  M5.Lcd.fillRect(42, 70,278,8,0);  M5.Lcd.fillRect(180+(qx<0?qx/256:0), 70, abs(qx/256), 7, 0xf800);
  M5.Lcd.fillRect(42, 80,278,8,0);  M5.Lcd.fillRect(180+(qy<0?qy/256:0), 80, abs(qy/256), 7, 0x07e0);
  M5.Lcd.fillRect(42, 90,278,8,0);  M5.Lcd.fillRect(180+(qz<0?qz/256:0), 90, abs(qz/256), 7, 0x001f);
  M5.Lcd.fillRect(42,110,278,8,0);  M5.Lcd.fillRect(180+(ax<0?ax/ 16:0),110, abs(ax/ 16), 7, 0xf800);
  M5.Lcd.fillRect(42,120,278,8,0);  M5.Lcd.fillRect(180+(ay<0?ay/ 16:0),120, abs(ay/ 16), 7, 0x07e0);
  M5.Lcd.fillRect(42,130,278,8,0);  M5.Lcd.fillRect(180+(az<0?az/ 16:0),130, abs(az/ 16), 7, 0x001f);
  M5.Lcd.fillRect(42,150,278,8,0);  M5.Lcd.fillRect(180+(gx<0?gx/ 16:0),150, abs(gx/ 16), 7, 0xf800);
  M5.Lcd.fillRect(42,160,278,8,0);  M5.Lcd.fillRect(180+(gy<0?gy/ 16:0),160, abs(gy/ 16), 7, 0x07e0);
  M5.Lcd.fillRect(42,170,278,8,0);  M5.Lcd.fillRect(180+(gz<0?gz/ 16:0),170, abs(gz/ 16), 7, 0x001f);
  M5.Lcd.fillRect(42,190,278,8,0);  M5.Lcd.fillRect(52+std::min(sx,(int16_t)0x80),190, abs(sx - 0x80), 7, 0xf800);
  M5.Lcd.fillRect(42,200,278,8,0);  M5.Lcd.fillRect(52+std::min(sy,(int16_t)0x80),200, abs(sy - 0x80), 7, 0x07e0);
  for (int i = 0; i < 7; ++i) {
    M5.Lcd.fillRect(60+i*10, 210, 7,7, (btn & (1 << i)) ? 0xffff : 0x8410);
  }

  return true;
}
