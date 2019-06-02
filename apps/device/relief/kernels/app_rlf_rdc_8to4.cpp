#ifndef APP_RLF_RDC_8TO4_CPP_
#define APP_RLF_RDC_8TO4_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_rdc_8to4(ST_Queue<APP_Reduce_Data8> &app_rlf_rdc_8to4_input_data,
                      ST_Queue<APP_Reduce_Data4> &app_rlf_rdc_4to2_input_data) {

  while (1) {
#pragma HLS pipeline
    APP_Reduce_Data8 reduce8_in;
#pragma HLS array_partition variable = reduce8_in.reduce complete dim = 0
#pragma HLS array_partition variable = reduce8_in.data complete
    APP_Reduce_Data4 reduce4_out;
#pragma HLS array_partition variable = reduce4_out.reduce complete dim = 0
#pragma HLS array_partition variable = reduce4_out.data complete
    if (app_rlf_rdc_8to4_input_data.read_nb(reduce8_in)) {
      for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
        for (int j = 0; j < 8; j += 2) {
#pragma HLS unroll
          reduce4_out.reduce[j >> 1] =
              (reduce8_in.reduce[j] + reduce8_in.reduce[j + 1]);
        }
      }
      for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
        reduce4_out.data[i] = reduce8_in.data[i];
      }
      reduce4_out.posi = reduce8_in.posi;
      reduce4_out.idx = reduce8_in.idx;
      reduce4_out.eot = reduce8_in.eot;
      app_rlf_rdc_4to2_input_data.write(reduce4_out);
    }
  }
}

#endif
