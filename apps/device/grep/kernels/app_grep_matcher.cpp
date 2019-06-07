#ifndef APP_GREP_MATCHER_CPP_
#define APP_GREP_MATCHER_CPP_
#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_grep_matcher(ST_Queue<unsigned int> &app_input_params,
                      ST_Queue<Grep_Input_Data> &app_grep_input_data_chan,
                      ST_Queue<Matching_Vec> &app_grep_matching_vecs) {
  bool valid_grep_param_size = false;
  unsigned int grep_param_size;

  bool valid_grep_params = false;
  char grep_param_str[MAX_GREP_PARAM_SIZE];
#pragma HLS array_partition variable = grep_param_str complete dim = 1
  unsigned int loading_grep_param_str_index = 0;

  while (1) {
#pragma HLS pipeline
    if (!valid_grep_param_size) {
      if (app_input_params.read_nb(grep_param_size)) {
        valid_grep_param_size = true;
      }
    } else if (!valid_grep_params) {
      unsigned int input_param;
      if (app_input_params.read_nb(input_param)) {
        for (int i = 0; i < POKE_WIDTH_BYTES; i++) {
#pragma HLS unroll
          grep_param_str[i + loading_grep_param_str_index] =
              (input_param >> ((POKE_WIDTH_BYTES - 1 - i) << 3)) & 0xFF;
        }
        loading_grep_param_str_index += POKE_WIDTH_BYTES;
        if (loading_grep_param_str_index >= grep_param_size) {
          valid_grep_params = true;
        }
      }
    } else {
      Grep_Input_Data grep_input_data;
      int col_index;
      if (app_grep_input_data_chan.read_nb(grep_input_data)) {
        bool matched_str[MATCHING_VEC_SIZE];
#pragma HLS array_partition variable = matched_str complete dim = 1
        for (int i = 0; i < MATCHING_VEC_SIZE; i++) {
          matched_str[i] = (i + grep_param_size - 1 < MATCHING_VEC_SIZE);
#pragma HLS unroll
          for (int j = 0; j < MAX_GREP_PARAM_SIZE; j++) {
            if (j < grep_param_size && i + j < MATCHING_VEC_SIZE) {
              matched_str[i] &=
                  (grep_param_str[j] == grep_input_data.data[i + j]);
            }
          }
        }
        Matching_Vec matching_vec;
        matching_vec.eop = grep_input_data.eop;
        matching_vec.valid = true;
        matching_vec.row_index = grep_input_data.row_index;
        for (int i = 0; i < MATCHING_VEC_SIZE; i++) {
#pragma HLS unroll
          matching_vec.matched[i] = matched_str[i];
          matching_vec.col_indices[i] = grep_input_data.col_indices[i];
        }

        for (int i = MATCHING_VEC_SIZE;
             i < MATCHING_VEC_SIZE + COMPARE_CHUNK_SIZE - 1; i++) {
#pragma HLS unroll
          matching_vec.matched[i] = false;
          matching_vec.col_indices[i] = -MAX_GREP_PARAM_SIZE;
        }

        app_grep_matching_vecs.write(matching_vec);
      }
    }
  }
}

#endif
