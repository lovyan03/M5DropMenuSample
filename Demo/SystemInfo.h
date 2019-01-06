#ifndef _SystemInfo_H_
#define _SystemInfo_H_

#include <M5Stack.h>

#include "../MenuItem.h"

class SystemInfo : public MenuCallBack
{
public:
  bool setup()
  {
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(0xffff);
    M5.Lcd.setCursor(0,10);
    M5.Lcd.fillRect(0,M5.Lcd.getCursorY(), TFT_HEIGHT, 8,0x0410);

    uint64_t chipid;
    chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    M5.Lcd.printf(" System Information\r\n");
    M5.Lcd.printf("   ESP32 Chip ID : %04X\r\n",(uint16_t)(chipid>>32));//print High 2 bytes
    M5.Lcd.printf("   Chip Revision : %d\r\n", ESP.getChipRevision());
    M5.Lcd.printf("  Number of Core : %d\r\n", chip_info.cores);
    M5.Lcd.printf("   CPU Frequency : %3d MHz\r\n", ESP.getCpuFreqMHz());  
    M5.Lcd.printf(" Flash Frequency : %3d MHz\r\n", ESP.getFlashChipSpeed() / 1000000);
    M5.Lcd.printf(" Flash Chip Size : %d Byte\r\n", ESP.getFlashChipSize());
    M5.Lcd.printf(" ESP-IDF version : %s\r\n", esp_get_idf_version());
    M5.Lcd.println();
    M5.Lcd.fillRect(0,M5.Lcd.getCursorY(), TFT_HEIGHT, 8,0x0410);
    M5.Lcd.printf(" Memory\r\n");
    M5.Lcd.printf("        Total Heap Size : %7d Byte\r\n", ESP.getHeapSize());
    M5.Lcd.printf("       Total PSRAM Size : %7d Byte\r\n", ESP.getPsramSize());
    M5.Lcd.printf("         Free Heap Size : %7d Byte\r\n", esp_get_free_heap_size());
    M5.Lcd.printf(" Minimum Free Heap Size : %7d Byte\r\n", esp_get_minimum_free_heap_size());
    M5.Lcd.println();
    M5.Lcd.fillRect(0,M5.Lcd.getCursorY(), TFT_HEIGHT, 8,0x0410);
    M5.Lcd.printf(" Mac Address\r\n");
    uint8_t mac[6];
    esp_base_mac_addr_get(mac);            M5.Lcd.printf("Base Mac Address");printMac(mac);
    esp_efuse_mac_get_default(mac);        M5.Lcd.printf("         Default");printMac(mac);
    esp_read_mac(mac, ESP_MAC_WIFI_STA);   M5.Lcd.printf("   Wi-Fi Station");printMac(mac);
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);M5.Lcd.printf("   Wi-Fi Soft AP");printMac(mac);
    esp_read_mac(mac, ESP_MAC_BT);         M5.Lcd.printf("       Bluetooth");printMac(mac);
    esp_read_mac(mac, ESP_MAC_ETH);        M5.Lcd.printf("        Ethernet");printMac(mac);
  }
  bool loop()
  {
    delay(10);
    return true;
  }
private:
  void printMac(uint8_t* mac) {
    M5.Lcd.printf(" : %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  }
};

#endif
