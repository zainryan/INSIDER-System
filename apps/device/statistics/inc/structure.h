#ifndef STRUCTURE_H_
#define STRUCTURE_H_

struct APP_Scatter_Data16 {
  unsigned int max[16];
  unsigned int min[16];
  unsigned long long sum[16];
  bool eop;
};

struct APP_Scatter_Data8 {
  unsigned int max[8];
  unsigned int min[8];
  unsigned long long sum[8];
  bool eop;
};

struct APP_Scatter_Data4 {
  unsigned int max[4];
  unsigned int min[4];
  unsigned long long sum[4];
  bool eop;
};

struct APP_Scatter_Data2 {
  unsigned int max[2];
  unsigned int min[2];
  unsigned long long sum[2];
  bool eop;
};

#endif
