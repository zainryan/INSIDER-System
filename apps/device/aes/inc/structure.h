#pragma once

#include <ap_int.h>

typedef ap_uint<8> Elem;
typedef ap_uint<128> Matrix;

struct APP_Chuck_Data {
  Matrix data;
  bool eop;
};

