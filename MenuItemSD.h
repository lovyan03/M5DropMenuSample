#ifndef _MenuItemSD_H_
#define _MenuItemSD_H_

#include "MenuItem.h"


class MenuItemSD : public MenuItem {
  static uint16_t maxFileCount;
public:
  String path;
  bool isDir;

  MenuItemSD(const String& titleStr, CALLBACK_MENUITEM cb = 0, MenuItem* sub[] = 0)
  : MenuItem(titleStr, cb, sub), path(), isDir(true) {};

  MenuItemSD(const String& titleStr, MenuItem* sub[])
  : MenuItem(titleStr, sub), path(), isDir(true) {};
  
//  MenuItemSD(const String& t, CALLBACK_MENUITEM cb, int tg = 0)  : MenuItem(t, cb, tg) , path(), isDir(true) {};
//  MenuItemSD(const String& t, MenuItem* sub[] = 0, int tg = 0)  : MenuItem(t, sub, tg) , path(), isDir(true) {};
  MenuItemSD(const String& p, bool isdir, const String& t, MenuItem* sub[] = 0)
  : MenuItem(t, sub) , path(p), isDir(isdir) {};

  virtual void OnEnter();
  virtual void OnAfterDraw();
};
//*/
#endif
