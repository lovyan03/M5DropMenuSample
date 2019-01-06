#ifndef _VroomCtrlDemo_H_
#define _VroomCtrlDemo_H_

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
