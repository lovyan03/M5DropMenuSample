#ifndef _PLUSEncoder_H_
#define _PLUSEncoder_H_

#include <M5Stack.h>

class PLUSEncoder
{
public:
  const uint16_t HOLD_MSEC = 500;
  PLUSEncoder() {}
  bool update()
  {
    if (!Wire.requestFrom(0x62, 2)) return false;
    _prevEnc = _enc;
    _prevPress = _press;
    bool pr = false;
    while (Wire.available()){
      _rawsum += Wire.read();
      pr = (Wire.read() != 0xff);
    }
    _enc = _rawsum / 3;
    if (_enc != 0) _rawsum = 0;
    uint32_t msec = millis();

    if (pr) {
      if (!_prevPress) {
        _press = 1;
        _pressTime = msec;
      } else 
      if (1 == _prevPress && (_pressTime + HOLD_MSEC < msec)) {
        _press = 2;
      }
    } else {
      _press = 0;
    }
    return true;
  }

  bool isUp()        { return _enc > 0; }
  bool isDown()      { return _enc < 0; }
  bool isPressed()   { return _press; }
  bool wasUp()       { return _enc != _prevEnc && _enc > 0; }
  bool wasDown()     { return _enc != _prevEnc && _enc < 0; }
  bool wasPressed()  { return !_prevPress && _press; }
  bool isClick()     { return _prevPress == 1 && _press == 0; }
  bool isLongClick() { return _prevPress == 1 && _press == 2; }
  bool wasHolding()  { return _prevPress == 2 && _press == 2; }

private:
  uint32_t _pressTime;
  int8_t _rawsum = 0x00;
  int8_t _enc = 0x00;
  int8_t _prevEnc = 0x00;
  uint8_t _press = 0x00;     // 0:release  1:click  2:holding
  uint8_t _prevPress = 0x00;
};

PLUSEncoder PlusEncoder;

#endif

extern PLUSEncoder PlusEncoder;
