#ifndef APP_STAT_WR_CPP_
#define APP_STAT_WR_CPP_

#include <insider_kernel.h>

#include "structure.h"

void app_stat_writer(ST_Queue<APP_Scatter_Data2> &app_stat_writer_input,
                     ST_Queue<APP_Data> &app_output_data) {
  unsigned int read_cnt = 0;
  APP_Data app_data_out;
  unsigned int max_buffer[4];
#pragma HLS array_partition variable = max_buffer complete
  unsigned int min_buffer[4];
#pragma HLS array_partition variable = min_buffer complete
  unsigned long long sum_buffer[4];
#pragma HLS array_partition variable = sum_buffer complete
  while (1) {
#pragma HLS pipeline
    APP_Scatter_Data2 data_in;
    if (app_stat_writer_input.read_nb(data_in)) {
      max_buffer[read_cnt] =
          (data_in.max[0] > data_in.max[1]) ? data_in.max[0] : data_in.max[1];
      min_buffer[read_cnt] =
          (data_in.min[0] < data_in.min[1]) ? data_in.min[0] : data_in.min[1];
      sum_buffer[read_cnt] = (data_in.sum[0] + data_in.sum[1]);

      app_data_out.data(128 * read_cnt + 31, 128 * read_cnt) =
          max_buffer[read_cnt];
      app_data_out.data(128 * read_cnt + 63, 128 * read_cnt + 32) =
          min_buffer[read_cnt];
      app_data_out.data(128 * read_cnt + 127, 128 * read_cnt + 64) =
          sum_buffer[read_cnt];
      read_cnt++;
      if (read_cnt == 4) {
        app_data_out.eop = data_in.eop;
        app_data_out.len = 64;
        read_cnt = 0;
        app_output_data.write(app_data_out);
      }
    }
  }
}
#endif
