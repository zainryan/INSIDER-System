#pragma once

#include <insider_kernel.h>

#include "structure.h"

inline Elem GFMul1(Elem num) {
#pragma HLS inline
  return num;
}

inline Elem GFMul2(Elem num) {
#pragma HLS inline
  if (num(7, 7) == 1) {
    num <<= 1;
    num = (Elem) (0x1b ^ ((unsigned char) num));
  } else {
    num <<= 1;
  }
  return num;
}

inline Elem GFMul(Elem param,
    Elem num) {
#pragma HLS inline
  if (param == 1) {
    num = GFMul1(num);
  } else if (param == 2) {
    num = GFMul2(num);
  } else if (param == 3) {
    num = (Elem) GFMul2(num) ^  num;
  }
  return num;
}

inline void mixcols (Matrix &data, Matrix &table) {
#pragma HLS inline
  Matrix new_data;
  for (unsigned char i = 0; i < 4; i++) {
#pragma HLS unroll
    for (unsigned char j = 0; j < 4; j++) {
#pragma HLS unroll
      Elem delta_result = 0;
      for (unsigned char k = 0; k < 4; k++) {
#pragma HLS unroll
        Elem num = (Elem) data(32 * k + 8 * j + 7, 32 * k + 8 * j);
        Elem xor_result = GFMul(((Elem) table(32 * i + 8 * k + 7, 32 * i + 8 * k)), num);
        delta_result = xor_result ^ delta_result;
      }
      new_data(32 * i + 8 * j + 7, 32 * i + 8 * j) = delta_result;
    }
  }
  data = new_data;
}
