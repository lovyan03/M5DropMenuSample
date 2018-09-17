#ifndef _VroomCtrlDemo_h_
#define _VroomCtrlDemo_h_

#include "MenuItem.h"

class VroomCtrlDemo : public MenuCallBack
{
public:
  VroomCtrlDemo();
  bool setup();
  bool loop();
  void close();
};

#endif
