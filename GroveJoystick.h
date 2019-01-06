#ifndef _GroveJoystick_H_
#define _GroveJoystick_H_

#include <M5Stack.h>

class GroveJoystick
{
public:
  GroveJoystick() {}
  bool update()
  {
    if (!Wire.requestFrom(0x52,3)) return false;
    while (Wire.available()) {
      PrevPress = Press;
      PrevUpDown = UpDown;
      PrevLeftRight = LeftRight;
      X = Wire.read();
      Y = Wire.read();
      Press = Wire.read();
      switch (Rotate) {
      default:
        UpDown    = (Y < 48) ? -2 : (Y < 64) ? -1 : (Y > 207) ?  2 : (Y > 191) ?  1 : 0;
        LeftRight = (X < 48) ? -2 : (X < 64) ? -1 : (X > 207) ?  2 : (X > 191) ?  1 : 0;
        break;
      case 1:
        UpDown    = (X < 48) ?  2 : (X < 64) ?  1 : (X > 207) ? -2 : (X > 191) ? -1 : 0;
        LeftRight = (Y < 48) ? -2 : (Y < 64) ? -1 : (Y > 207) ?  2 : (Y > 191) ?  1 : 0;
        break;
      case 2:
        UpDown    = (Y < 48) ?  2 : (Y < 64) ?  1 : (Y > 207) ? -2 : (Y > 191) ? -1 : 0;
        LeftRight = (X < 48) ?  2 : (X < 64) ?  1 : (X > 207) ? -2 : (X > 191) ? -1 : 0;
        break;
      case 3:
        UpDown    = (X < 48) ? -2 : (X < 64) ? -1 : (X > 207) ?  2 : (X > 191) ?  1 : 0;
        LeftRight = (Y < 48) ?  2 : (Y < 64) ?  1 : (Y > 207) ? -2 : (Y > 191) ? -1 : 0;
        break;
      }
    }
    return true;
  }
  uint8_t Rotate = 0;
  uint8_t X = 0;
  uint8_t Y = 0;
  bool PrevPress = false;
  bool Press = false;
  int8_t PrevUpDown = 0;
  int8_t PrevLeftRight = 0;
  int8_t UpDown = 0;
  int8_t LeftRight = 0;
  bool wasUp()      { return 0 > UpDown && PrevUpDown == 0; }
  bool wasDown()    { return 0 < UpDown && PrevUpDown == 0; }
  bool wasLeft()    { return 0 < LeftRight && PrevLeftRight == 0; }
  bool wasRight()   { return 0 > LeftRight && PrevLeftRight == 0; }
  bool wasPressed() { return !PrevPress && Press; }
};

GroveJoystick JoyStick;

#endif

extern GroveJoystick JoyStick;
