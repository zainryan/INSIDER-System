#pragma once

#include <ap_int.h>

typedef ap_uint<8> Elem;
typedef ap_uint<128> Matrix;
//typedef ap_uint<2048> S_Matrix;

struct APP_Chuck_Data {
  Matrix data;
  bool eop;
};

