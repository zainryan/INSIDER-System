#ifndef APP_GREP_REDUCER_CPP_
#define APP_GREP_REDUCER_CPP_
#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_grep_reducer(ST_Queue<Matching_Vec> &app_grep_matching_vecs,
                      ST_Queue<Matched_Pos> &app_grep_matched_pos_chan) {
  while (1) {
#pragma HLS pipeline
    Matching_Vec matching_vec;
    if (app_grep_matching_vecs.read_nb(matching_vec)) {
      Matched_Pos matched_pos;
      if (!matching_vec.valid && matching_vec.eop) {
        Matched_Pos matched_pos;
        matched_pos.eop = true;
        matched_pos.valid = false;
        app_grep_matched_pos_chan.write(matched_pos);
      } else {
        int col_indices[(MATCHING_VEC_SIZE + COMPARE_CHUNK_SIZE - 1) /
                        COMPARE_CHUNK_SIZE];
#pragma HLS array_partition variable = col_indices complete dim = 1

        for (int i = 0; i < (MATCHING_VEC_SIZE + COMPARE_CHUNK_SIZE - 1) /
                                COMPARE_CHUNK_SIZE;
             i++) {
#pragma HLS unroll
          col_indices[i] = -MAX_GREP_PARAM_SIZE;
          for (int j = 0; j < COMPARE_CHUNK_SIZE; j++) {
            unsigned char idx = i * COMPARE_CHUNK_SIZE + j;
            if (col_indices[i] < 0 && matching_vec.matched[idx]) {
              col_indices[i] = matching_vec.col_indices[idx];
            }
          }
        }

        int col_index = -MAX_GREP_PARAM_SIZE;
        for (int i = 0; i < (MATCHING_VEC_SIZE + COMPARE_CHUNK_SIZE - 1) /
                                COMPARE_CHUNK_SIZE;
             i++) {
#pragma HLS unroll
          if (col_indices[i] != -MAX_GREP_PARAM_SIZE) {
            col_index = col_indices[i];
            break;
          }
        }

        bool matched = (col_index != -MAX_GREP_PARAM_SIZE);
        if (matched || matching_vec.eop) {
          Matched_Pos matched_pos;
          matched_pos.valid = matched;
          matched_pos.eop = matching_vec.eop;
          matched_pos.row_index = matching_vec.row_index;
          matched_pos.col_index = col_index;
          app_grep_matched_pos_chan.write(matched_pos);
        }
      }
    }
  }
}
#endif
