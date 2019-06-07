#ifndef STRUCTURE_H_
#define STRUCTURE_H_
struct APP_Match {
  APP_Match() {}
  unsigned char record[64];
  unsigned char match[32];
  bool eop;
};

struct APP_Ver_Param {
  APP_Ver_Param() {}
  unsigned char query[32];
  unsigned int thres;
};

struct APP_Ver_Record {
  APP_Ver_Record() {}
  unsigned char record[64];
  unsigned int valid;
  bool eop;
};
struct APP_Reduce_Record32 {
  APP_Reduce_Record32() {}
  unsigned char overlap[32];
  unsigned char record[64];
  bool valid;
  bool eop;
};

struct APP_Reduce_Record16 {
  APP_Reduce_Record16() {}
  unsigned char overlap[16];
  unsigned char record[64];
  bool valid;
  bool eop;
};

struct APP_Reduce_Record8 {
  APP_Reduce_Record8() {}
  unsigned char overlap[8];
  unsigned char record[64];
  bool valid;
  bool eop;
};

struct APP_Reduce_Record4 {
  APP_Reduce_Record4() {}
  unsigned char overlap[4];
  unsigned char record[64];
  bool valid;
  bool eop;
};

struct APP_Reduce_Record2 {
  APP_Reduce_Record2() {}
  unsigned char overlap[2];
  unsigned char record[64];
  bool valid;
  bool eop;
};
#endif
