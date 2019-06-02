#ifndef APP_STAT_RDC8_CPP_
#define APP_STAT_RDC8_CPP_

#include <insider_kernel.h>

#include "structure.h"

void app_stat_rdc_8to4(ST_Queue<APP_Scatter_Data8> &app_stat_rdc_8to4_input,
                       ST_Queue<APP_Scatter_Data4> &app_stat_rdc_4to2_input) {
  while (1) {
#pragma HLS pipeline
    APP_Scatter_Data8 data_in;
    APP_Scatter_Data4 data_out;
    if (app_stat_rdc_8to4_input.read_nb(data_in)) {
      for (int i = 0; i < 8; i += 2) {
#pragma HLS unroll
        data_out.max[i >> 1] = (data_in.max[i] > data_in.max[i + 1])
                                   ? data_in.max[i]
                                   : data_in.max[i + 1];
      }
      for (int i = 0; i < 8; i += 2) {
#pragma HLS unroll
        data_out.min[i >> 1] = (data_in.min[i] < data_in.min[i + 1])
                                   ? data_in.min[i]
                                   : data_in.min[i + 1];
      }
      for (int i = 0; i < 8; i += 2) {
#pragma HLS unroll
        data_out.sum[i >> 1] = data_in.sum[i] + data_in.sum[i + 1];
      }
      data_out.eop = data_in.eop;
      app_stat_rdc_4to2_input.write(data_out);
    }
  }
}
#endif
