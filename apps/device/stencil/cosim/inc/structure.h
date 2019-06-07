#ifndef STRUCTURE_H_
#define STRUCTURE_H_

struct Point {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
  Point() : red(0), green(0), blue(0), alpha(0) {}
  Point& operator +=(const Point &p);
};

#endif
