#ifndef APP_RLF_RDC_2TO1_CPP_
#define APP_RLF_RDC_2TO1_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_rdc_2to1(ST_Queue<APP_Reduce_Data2> &app_rlf_rdc_2to1_input_data,
                      ST_Queue<APP_Reduce_Data> &app_rlf_max_min_input_data) {

  while (1) {
#pragma HLS pipeline
    APP_Reduce_Data2 reduce2_in;
#pragma HLS array_partition variable = reduce2_in.reduce complete dim = 0
#pragma HLS array_partition variable = reduce2_in.data complete
    APP_Reduce_Data reduce_out;
#pragma HLS array_partition variable = reduce_out.data complete
    if (app_rlf_rdc_2to1_input_data.read_nb(reduce2_in)) {
      for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
        reduce_out.reduce = reduce2_in.reduce[0] + reduce2_in.reduce[1];
      }
      for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
        reduce_out.data[i] = reduce2_in.data[i];
      }
      reduce_out.posi = reduce2_in.posi;
      reduce_out.idx = reduce2_in.idx;
      reduce_out.eot = reduce2_in.eot;
      app_rlf_max_min_input_data.write(reduce_out);
    }
  }
}

#endif
