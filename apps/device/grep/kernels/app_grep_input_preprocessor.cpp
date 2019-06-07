#ifndef APP_GREP_INPUT_PREPROCESSOR_CPP_
#define APP_GREP_INPUT_PREPROCESSOR_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"
void app_grep_input_preprocessor(
    ST_Queue<APP_Data> &app_input_data,
    ST_Queue<Grep_Input_Data> &app_grep_input_data_chan) {
  char prev_input_data_str[MAX_GREP_PARAM_SIZE - 1];
  int row_index = 0;
  int col_state_cnt = 0;
  while (1) {
#pragma HLS pipeline
    APP_Data cur_input_data;
    if (app_input_data.read_nb(cur_input_data)) {
      char cur_input_data_in_bytes[DATA_BUS_WIDTH];
#pragma HLS array_partition variable = cur_input_data_in_bytes complete dim = 1

      bool has_newline = false;
      for (int i = 0; i < DATA_BUS_WIDTH; i++) {
#pragma HLS unroll
        cur_input_data_in_bytes[i] = cur_input_data.data(i * 8 + 7, i * 8);
        has_newline |= (cur_input_data_in_bytes[i] == '\n');
      }

      Grep_Input_Data grep_input_data;
      for (int i = 0; i < MATCHING_VEC_SIZE; i++) {
#pragma HLS unroll
        grep_input_data.col_indices[i] =
            -MAX_GREP_PARAM_SIZE + 1 + (col_state_cnt << DATA_BUS_WIDTH_LOG2) +
            i;
        if (grep_input_data.col_indices[i] >= 0) {
          if (i < MAX_GREP_PARAM_SIZE - 1) {
            grep_input_data.data[i] = prev_input_data_str[i];
          } else {
            grep_input_data.data[i] =
                cur_input_data_in_bytes[i - (MAX_GREP_PARAM_SIZE - 1)];
          }
        } else {
          grep_input_data.data[i] = '\0';
        }
      }
      grep_input_data.row_index = row_index;
      grep_input_data.eop = cur_input_data.eop;
      app_grep_input_data_chan.write(grep_input_data);

      for (int i = 0; i < MAX_GREP_PARAM_SIZE - 1; i++) {
#pragma HLS unroll
        prev_input_data_str[i] =
            cur_input_data_in_bytes[DATA_BUS_WIDTH - (MAX_GREP_PARAM_SIZE - 1) +
                                    i];
      }

      if (has_newline) {
        row_index++;
        col_state_cnt = 0;
      } else {
        col_state_cnt++;
      }
    }
  }
}
#endif
