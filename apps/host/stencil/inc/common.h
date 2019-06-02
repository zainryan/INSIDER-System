#ifndef COMMON_H_
#define COMMON_H_

#include "structure.h"

inline Point host_uint_to_point(unsigned int uint) {
  Point point;
  point.red = (uint >> 0) & 0xFFFFFFFF;
  point.green = (uint >> 8) & 0xFFFFFFFF;
  point.blue = (uint >> 16) & 0xFFFFFFFF;
  point.alpha = (uint >> 24) & 0xFFFFFFFF;
  return point;
}

inline Point operator*(const Point &pa, const Point &pb) {
  Point pc;
  pc.red = pa.red * pb.red;
  pc.green = pa.green * pb.green;
  pc.blue = pa.blue * pb.blue;
  pc.alpha = pa.alpha * pb.alpha;
  return pc;
}

inline void add(volatile Point &a, const Point &b) {
  a.red += b.red;
  a.green += b.green;
  a.blue += b.blue;
  a.alpha += b.alpha;
}

#endif
