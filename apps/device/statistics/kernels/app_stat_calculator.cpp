#ifndef APP_STAT_CAL_CPP_
#define APP_STAT_CAL_CPP_

#include <insider_kernel.h>

#include "structure.h"

void app_stat_calculator(
    ST_Queue<unsigned int> &app_input_params,
    ST_Queue<APP_Data> &app_input_data,
    ST_Queue<APP_Scatter_Data16> &app_stat_rdc_16to8_input) {
  APP_Data cur_input_data;
  unsigned int app_data[16];
  unsigned int col_count = 0; // count per row
  unsigned int app_col_num;
  bool valid_param = false;
  unsigned int max_vec[16];
  unsigned int min_vec[16];
  unsigned long long total_vec[16];
  while (1) {
#pragma HLS pipeline
    if (!valid_param) {
      unsigned int param;
      if (app_input_params.read_nb(param)) {
        app_col_num = param; // number of cols
        valid_param = true;
        for (int idx = 0; idx < 16; idx++) {
#pragma HLS unroll
          max_vec[idx] = 0;
          min_vec[idx] = 0xFFFFFFFF;
          total_vec[idx] = 0;
        }
      }
    } else {
      if (col_count < app_col_num) {
        if (app_input_data.read_nb(cur_input_data)) {
          for (int idx = 0; idx < 16; idx++) {
#pragma HLS unroll
            app_data[idx] = cur_input_data.data(32 * idx + 31, 32 * idx);
          }
          for (int idx = 0; idx < 16; idx++) {
#pragma HLS unroll
            max_vec[idx] =
                (app_data[idx] > max_vec[idx]) ? app_data[idx] : max_vec[idx];
          }
          for (int idx = 0; idx < 16; idx++) {
#pragma HLS unroll
            min_vec[idx] =
                (app_data[idx] < min_vec[idx]) ? app_data[idx] : min_vec[idx];
          }
          for (int idx = 0; idx < 16; idx++) {
#pragma HLS unroll
            total_vec[idx] += app_data[idx];
          }

          col_count += 64; // count in 64B
          if (col_count == app_col_num) {
            col_count = 0;
            APP_Scatter_Data16 data_out;
            for (int i = 0; i < 16; i++) {
#pragma HLS unroll
              data_out.max[i] = max_vec[i];
              data_out.min[i] = min_vec[i];
              data_out.sum[i] = total_vec[i];
            }
            data_out.eop = cur_input_data.eop;
            for (int i = 0; i < 16; i++) {
#pragma HLS unroll
              max_vec[i] = 0;
              min_vec[i] = 0xFFFFFFFF;
              total_vec[i] = 0;
            }
            if (cur_input_data.eop == true) {
              valid_param = false;
            }
            app_stat_rdc_16to8_input.write(data_out);
          }
        }
      }
    }
  }
}
#endif
