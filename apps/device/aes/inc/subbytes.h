#pragma once 

#include <insider_kernel.h>

#include "structure.h" 

inline void subbytes (Matrix &data, Elem table[][256], unsigned char section_id) {
#pragma HLS inline
  for (int i = 0; i < 16; i++) {
#pragma HLS unroll
    Elem row_id = data(8 * i + 7, 8 * i + 4); // (7, 4);
    Elem col_id = data(8 * i + 3, 8 * i + 0); // (3, 0);
    data(8 * i + 7, 8 * i + 0) = table[16 * section_id + i][16 * row_id + col_id];
  }
}
