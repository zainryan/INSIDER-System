#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "constant.h"

struct Grep_Input_Data {
  char data[MATCHING_VEC_SIZE];
  int col_indices[MATCHING_VEC_SIZE];
  int row_index;
  bool eop;
};

struct Matched_Pos {
  int row_index;
  int col_index;
  bool eop;
  bool valid;
};

struct Matching_Vec {
  bool matched[MATCHING_VEC_SIZE + COMPARE_CHUNK_SIZE - 1];
  int row_index;
  int col_indices[MATCHING_VEC_SIZE + COMPARE_CHUNK_SIZE - 1];
  bool eop;
  bool valid;
};

#endif
