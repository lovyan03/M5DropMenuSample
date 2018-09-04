#ifndef _MenuItem_H_
#define _MenuItem_H_

#include <M5Stack.h>

#ifndef min
  #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
  #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef array_length
  #define array_length(array) (sizeof(array) / sizeof(array[0]))
#endif

typedef struct Rect {
  int16_t x, y, w, h;
  Rect (int16_t xpos = 0, int16_t ypos = 0, int16_t width = 0, int16_t height = 0): x(xpos), y(ypos), w(width), h(height) {}
  Rect (const Rect& src) : x(src.x), y(src.y), w(src.w), h(src.h) {}
  bool equal(const Rect& rhs) const { return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h; }
  int16_t Bottom() const { return y+h; }
  int16_t Right() const { return x+w; }
  Rect mixRect(const Rect& dst) const {
    return Rect( ((x + (x < dst.x) - (x > dst.x)) + dst.x) / 2
               , ((y + (y < dst.y) - (y > dst.y)) + dst.y) / 2
               , ((w + (w < dst.w) - (w > dst.w)) + dst.w) / 2
               , ((h + (h < dst.h) - (h > dst.h)) + dst.h) / 2
               );
  }
  void Inflate(int inc) { x-=inc; y-=inc; w+=inc*2; h+=inc*2; };
};

typedef class MenuItem;
typedef void (* CALLBACK_MENUITEM)(MenuItem *); 

typedef class MenuItem {
public:
  static int8_t nestOffset;     // nest x offset
  static int8_t itemHeight;
  static int16_t itemWidth;
  static uint16_t fontColor;
  static uint16_t fillColor;    // item fill 
  static uint16_t frameColor;   // item frame
  static uint16_t cursorColor;  // focused fill
  static uint16_t backgroundColor;
  static MenuItem* selectedItem;
  MenuItem();
  MenuItem(const String& titleStr, int tg = 0, CALLBACK_MENUITEM cb = 0, MenuItem* sub[] = 0);
  MenuItem(const String& titleStr, CALLBACK_MENUITEM cb, MenuItem* sub[] = 0);
  MenuItem(const String& titleStr, MenuItem* sub[]);
  void SetSubItems(MenuItem* sub[] = 0);
  void DisposeSubItems();

  int16_t UpdateDestRect(int16_t x = 0, int16_t y = 0);
  void DestRectYScroll(int16_t add_y);
  bool Move();
  MenuItem* Draw(bool force, const MenuItem* forceItem = 0);
  void Hide();

  virtual void OnEnter();
  virtual void OnExit() { }
  virtual void OnAfterDraw() { }

  CALLBACK_MENUITEM callback = 0;
  int tag = 0;
  String title;

  MenuItem* parentItem = 0;
  MenuItem** subItems = 0;
  Rect rect;        // displayPoint
  Rect destRect;    // destinationPoint
  bool visible;
};

class MenuItemBoolean : public MenuItem {
public:
  bool value = false;

  MenuItemBoolean(const String& titleStr, int tg = 0, CALLBACK_MENUITEM cb = 0)
  : MenuItem(titleStr, tg, cb), value(false) {};

  MenuItemBoolean(const String& titleStr, CALLBACK_MENUITEM cb)
  : MenuItem(titleStr, cb), value(false) {};

  virtual void OnAfterDraw();
  virtual void OnEnter();
private:
  void DrawSwitch(int mode);
};

#endif
