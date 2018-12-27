#ifndef _FACESGameBoy_h_
#define _FACESGameBoy_h_

#include <M5Stack.h>

class FACESGameBoy
{
public:
  FACESGameBoy() {}
  bool update()
  {
    if (!Wire.requestFrom(0x08, 1)) return false;
    PrevBit = Bit;
    while (Wire.available()){
      Bit = Wire.read();
    }
    return true;
  }

  uint8_t Bit = 0x00;
  uint8_t PrevBit = 0x00;
  bool isUp()            { return !( Bit & 0x01 ); }
  bool isDown()          { return !( Bit & 0x02 ); }
  bool isLeft()          { return !( Bit & 0x04 ); }
  bool isRight()         { return !( Bit & 0x08 ); }
  bool isPressedA()      { return !( Bit & 0x10 ); }
  bool isPressedB()      { return !( Bit & 0x20 ); }
  bool isPressedStart()  { return !( Bit & 0x80 ); }
  bool isPressedSelect() { return !( Bit & 0x40 ); }
  bool wasUp()            { return !( Bit & 0x01 ) && ( PrevBit & 0x01 ); }
  bool wasDown()          { return !( Bit & 0x02 ) && ( PrevBit & 0x02 ); }
  bool wasLeft()          { return !( Bit & 0x04 ) && ( PrevBit & 0x04 ); }
  bool wasRight()         { return !( Bit & 0x08 ) && ( PrevBit & 0x08 ); }
  bool wasPressedA()      { return !( Bit & 0x10 ) && ( PrevBit & 0x10 ); }
  bool wasPressedB()      { return !( Bit & 0x20 ) && ( PrevBit & 0x20 ); }
  bool wasPressedStart()  { return !( Bit & 0x80 ) && ( PrevBit & 0x80 ); }
  bool wasPressedSelect() { return !( Bit & 0x40 ) && ( PrevBit & 0x40 ); }
};

FACESGameBoy FacesGameBoy;

#endif

extern FACESGameBoy FacesGameBoy;
