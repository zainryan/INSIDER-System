#ifndef APP_GREP_WR_CPP_
#define APP_GREP_WR_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_grep_writer(ST_Queue<APP_Data> &app_output_data,
                     ST_Queue<Matched_Pos> &app_grep_matched_pos_chan) {

  int row_indices[DATA_BUS_WIDTH / sizeof(int) / 2];
#pragma HLS array_partition variable = row_indices complete dim = 1
  int col_indices[DATA_BUS_WIDTH / sizeof(int) / 2];
#pragma HLS array_partition variable = col_indices complete dim = 1
  int arr_index = 0;
  int prev_row_index = -1;

  while (1) {
#pragma HLS pipeline
    Matched_Pos matched_pos;
    if (app_grep_matched_pos_chan.read_nb(matched_pos)) {
      if (matched_pos.valid) {
        if (prev_row_index != matched_pos.row_index) {
          row_indices[arr_index] = matched_pos.row_index;
          col_indices[arr_index] = matched_pos.col_index;
          arr_index++;
          prev_row_index = matched_pos.row_index;
        }
      }
      if (arr_index == DATA_BUS_WIDTH / sizeof(int) / 2 || matched_pos.eop) {
        APP_Data cur_output_data;
        for (int idx = 0; idx < 512; idx += 64) {
          cur_output_data.data(idx + 31, idx) = row_indices[idx / 64];
          cur_output_data.data(idx + 63, idx + 32) = col_indices[idx / 64];
        }
        cur_output_data.eop = matched_pos.eop;
        cur_output_data.len = arr_index * sizeof(int) * 2;
        app_output_data.write(cur_output_data);
        arr_index = 0;
      }
    }
  }
}

#endif
