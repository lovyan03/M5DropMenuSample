#include "MPU9250Demo.h"
#include <M5Stack.h>

MPU9250Demo::MPU9250Demo()
{
}

bool MPU9250Demo::setup()
{
  M5.Lcd.fillScreen(0);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.setTextColor(0xffff);
  if (0x71 != IMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250)) {
    M5.Lcd.println("MPU9250 not connected...");
    delay(2);
    return false;
  }
  IMU.initMPU9250();
  if ( 0x48 != IMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963)) {
    M5.Lcd.println("AK8963 not connected...");
    delay(2);
    return false;
  }
    
  IMU.initAK8963(IMU.magCalibration);
  return true;
}

void MPU9250Demo::plotpoint(uint16_t x, uint16_t y, int r, int g, int b)
{
  if (r == g && g == b) {
    M5.Lcd.drawPixel(x, y + r, 0xFFFF );
  } else if (r == g) {
    M5.Lcd.drawPixel(x, y + r, 0xFFE0 );
    M5.Lcd.drawPixel(x, y + b, 0x001F);
  } else if (r == b) {
    M5.Lcd.drawPixel(x, y + r, 0xF81F);
    M5.Lcd.drawPixel(x, y + g, 0x07E0);
  } else if (g == b) {
    M5.Lcd.drawPixel(x, y + r, 0xF800);
    M5.Lcd.drawPixel(x, y + g, 0x07FF);
  } else {
    M5.Lcd.drawPixel(x, y + r, 0xF800);
    M5.Lcd.drawPixel(x, y + g, 0x07E0);
    M5.Lcd.drawPixel(x, y + b, 0x001F);
  }
}

void MPU9250Demo::loop()
{  
  if (!(IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)) return;

  IMU.readAccelData(IMU.accelCount);
  IMU.getAres();
  IMU.ax = (float)IMU.accelCount[0]*IMU.aRes;
  IMU.ay = (float)IMU.accelCount[1]*IMU.aRes;
  IMU.az = (float)IMU.accelCount[2]*IMU.aRes;

  IMU.readGyroData(IMU.gyroCount);
  IMU.getGres();
  IMU.gx = (float)IMU.gyroCount[0]*IMU.gRes;
  IMU.gy = (float)IMU.gyroCount[1]*IMU.gRes;
  IMU.gz = (float)IMU.gyroCount[2]*IMU.gRes;

  IMU.readMagData(IMU.magCount);
  IMU.getMres();
  IMU.mx = (float)IMU.magCount[0]*IMU.mRes;
  IMU.my = (float)IMU.magCount[1]*IMU.mRes;
  IMU.mz = (float)IMU.magCount[2]*IMU.mRes;

  M5.Lcd.drawLine(_x, 0, _x, TFT_WIDTH, 0);

  M5.Lcd.setCursor( 10, 5);
  M5.Lcd.setTextColor(0xffff);    M5.Lcd.print("accel (mg) ");

  M5.Lcd.setTextColor(0xf800,0);    M5.Lcd.printf("  x:%+6d", (int)(1000 * IMU.ax));
  M5.Lcd.setTextColor(0x07e0,0);    M5.Lcd.printf("  y:%+6d", (int)(1000 * IMU.ay));
  M5.Lcd.setTextColor(0x001f,0);    M5.Lcd.printf("  z:%+6d", (int)(1000 * IMU.az));

  M5.Lcd.setCursor( 10, 80);
  M5.Lcd.setTextColor(0xffff);    M5.Lcd.print("gyro (o/s) ");
  M5.Lcd.setTextColor(0xf800,0);    M5.Lcd.printf("  x:%+6d", (int)(IMU.gx));
  M5.Lcd.setTextColor(0x07e0,0);    M5.Lcd.printf("  y:%+6d", (int)(IMU.gy));
  M5.Lcd.setTextColor(0x001f,0);    M5.Lcd.printf("  z:%+6d", (int)(IMU.gz));

  M5.Lcd.setCursor( 10,160);
  M5.Lcd.setTextColor(0xffff);    M5.Lcd.print("  mag (mG) ");
  M5.Lcd.setTextColor(0xf800,0);    M5.Lcd.printf("  x:%+6d", (int)(IMU.mx/2));
  M5.Lcd.setTextColor(0x07e0,0);    M5.Lcd.printf("  y:%+6d", (int)(IMU.my/2));
  M5.Lcd.setTextColor(0x001f,0);    M5.Lcd.printf("  z:%+6d", (int)(IMU.mz/2));

  plotpoint(_x, 40, IMU.ax * 20, IMU.ay * 20, IMU.az * 20);
  plotpoint(_x,120, IMU.gx / 10, IMU.gy / 10, IMU.gz / 10);
  plotpoint(_x,200, IMU.mx /300, IMU.my /300, IMU.mz /300);

  _x = (++_x % TFT_HEIGHT);
  M5.Lcd.drawLine(_x+1, 0, _x+1, TFT_WIDTH, 0xffff);

}
