#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "constant.h"

struct APP_Dist_Data {
  int data[16];
  bool posi;
  int idx;
  bool eot;
};

struct APP_Reduce_Data16 {
  int data[16];
  int reduce[16];
  bool posi;
  int idx;
  bool eot;
};

struct APP_Reduce_Data8 {
  int data[16];
  int reduce[8];
  bool posi;
  int idx;
  bool eot;
};

struct APP_Reduce_Data4 {
  int data[16];
  int reduce[4];
  bool posi;
  int idx;
  bool eot;
};

struct APP_Reduce_Data2 {
  int data[16];
  int reduce[2];
  bool posi; // mark it from positive training set
  int idx;
  bool eot;
};

struct APP_Reduce_Data {
  int data[16];
  int reduce;
  bool posi; // mark it from positive training set
  int idx;
  bool eot;
};
struct APP_Stg_Data {
  int data[16];
  bool update;
  bool posi;
  int idx;
  bool eot;
};

struct APP_Diff_Data {
  int maxHitVec[16];
  int maxMissVec[16];
  int idx;
};

struct APP_Upd_Data {
  int weight[16];
  int idx;
};

struct APP_Flt_Param {
  bool weight[APP_FEATURE_DIM];
  APP_Flt_Param() {
#pragma HLS array_partition variable = weight complete
  }
};

struct APP_Flt_Data16 {
  int data[16];
  int idx;
  bool eop;
};

struct APP_Flt_Data8 {
  char idx;
  int data[16];
  char validNum[8];
  bool eop;
};

struct APP_Flt_Data4 {
  char idx;
  int data[16];
  char validNum[4];
  bool eop;
};

struct APP_Flt_Data2 {
  char idx;
  int data[16];
  char validNum[2];
  bool eop;
};

struct APP_Write_Data {
  char idx;
  int data[16];
  char validNum;
  bool eop;
};

#endif
