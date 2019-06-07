#ifndef APP_RLF_RDC_4TO2_CPP_
#define APP_RLF_RDC_4TO2_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_rdc_4to2(ST_Queue<APP_Reduce_Data4> &app_rlf_rdc_4to2_input_data,
                      ST_Queue<APP_Reduce_Data2> &app_rlf_rdc_2to1_input_data) {

  while (1) {
#pragma HLS pipeline
    APP_Reduce_Data4 reduce4_in;
#pragma HLS array_partition variable = reduce4_in.reduce complete dim = 0
#pragma HLS array_partition variable = reduce4_in.data complete
    APP_Reduce_Data2 reduce2_out;
#pragma HLS array_partition variable = reduce2_out.reduce complete dim = 0
#pragma HLS array_partition variable = reduce2_out.data complete
    if (app_rlf_rdc_4to2_input_data.read_nb(reduce4_in)) {
      for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
        for (int j = 0; j < 4; j += 2) {
#pragma HLS unroll
          reduce2_out.reduce[j >> 1] =
              reduce4_in.reduce[j] + reduce4_in.reduce[j + 1];
        }
      }
      for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
        reduce2_out.data[i] = reduce4_in.data[i];
      }
      reduce2_out.posi = reduce4_in.posi;
      reduce2_out.idx = reduce4_in.idx;
      reduce2_out.eot = reduce4_in.eot;
      app_rlf_rdc_2to1_input_data.write(reduce2_out);
    }
  }
}

#endif
