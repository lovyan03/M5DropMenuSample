#ifndef _AD34FFTTaskDemo_h_
#define _AD34FFTTaskDemo_h_
#include "arduinoFFT.h"
#include <M5Stack.h>

#include "../MenuItem.h"

#define ADC_MIC_PIN 34
#define FFT_LEN 128
#define SAMPLINGFREQUENCY 40000
#define SAMPLING_TIME_US (1000000UL/SAMPLINGFREQUENCY)

class AD34FFTTaskDemo : public MenuCallBack
{
public:
  AD34FFTTaskDemo() {}
  bool setup()
  {
    pinMode(ADC_MIC_PIN, INPUT);
    pinMode(25, OUTPUT);
    dacWrite(25, 0);
    isRunningFFT = true;
    startMsec = millis();
    frameMain = 0;
    xTaskCreatePinnedToCore(taskFftLoop, "taskfft", 4096, this, 1, &xHandleFft, 0);
    return true;
  }
  void close() {
    isRunningFFT = false;
  }
  bool loop()
  {
    ++frameMain;
    int x, y, py;
    for (int n = 1; n < FFT_LEN / 2; ++n)
    {
      x = n * 5 - 3;
      y = fftdata[n];
      py = prevdata[n];
      if (py > y) M5.Lcd.fillRect(x, 220-py, 4, py-y, 0);
      M5.Lcd.fillRect(x, 220-y, 4, y+1, 0x001f-(y>>3) + ((y>>2)<<5) + ((y>>3)<<11));
      prevdata[n] = y;
    }
    uint32_t m = millis() - startMsec;
    if (m != 0) {
      M5.Lcd.setCursor(0,0);
      M5.Lcd.printf("MainTask%3dFPS / FFT Task%3dFPS"
                   , (int)((double)frameMain * 1000 / m)
                   , (int)((double)frameFFT  * 1000 / m));
    }
    while (frameMain > frameFFT) delayMicroseconds(1);
    return true;
  }
private:
  unsigned char prevdata[FFT_LEN / 2];
  volatile unsigned char fftdata[FFT_LEN / 2];
  volatile bool isRunningFFT;
  volatile uint32_t frameFFT = 0;
  TaskHandle_t xHandleFft = NULL;
  uint32_t startMsec;
  uint32_t frameMain;

  static void taskFftLoop(void* arg) {
    AD34FFTTaskDemo* Me = (AD34FFTTaskDemo*)arg;
    Me->frameFFT = 0;
    uint32_t nextTime = 0;
    double AdcMeanValue = 0;
    int x, y, n;
    arduinoFFT FFT = arduinoFFT();
    double adBuf[FFT_LEN];
    while (Me->isRunningFFT) {
      ++Me->frameFFT;
      double vImag[FFT_LEN] = {0};
      for (n = 0; n < FFT_LEN; n++) {
        double v = analogRead(ADC_MIC_PIN);
        AdcMeanValue += (v - AdcMeanValue) * 0.001;
        adBuf[n] = v - AdcMeanValue;
        delayMicroseconds(1);
        while (micros() < nextTime);
        nextTime = micros() + SAMPLING_TIME_US;
      }
      FFT.Windowing(adBuf, FFT_LEN, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(adBuf, vImag, FFT_LEN, FFT_FORWARD);
      FFT.ComplexToMagnitude(adBuf, vImag, FFT_LEN);
      for (n = 0; n < FFT_LEN / 2; n++) {
        Me->fftdata[n] = map(min(256, adBuf[n]/32), 0, 256, 0, 204);
      }
      while (Me->isRunningFFT && Me->frameMain < Me->frameFFT) delayMicroseconds(1);
    }
    vTaskDelete(Me->xHandleFft);
  }
};
#endif
