#ifndef _Rect_H_
#define _Rect_H_

struct Rect {
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

#endif
