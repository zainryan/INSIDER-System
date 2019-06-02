#ifndef APP_RLF_RDC_16TO8_CPP_
#define APP_RLF_RDC_16TO8_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_rdc_16to8(
    ST_Queue<APP_Reduce_Data16> &app_rlf_rdc_16to8_input_data,
    ST_Queue<APP_Reduce_Data8> &app_rlf_rdc_8to4_input_data) {

  while (1) {
#pragma HLS pipeline
    APP_Reduce_Data16 reduce16_in;
#pragma HLS array_partition variable = reduce16_in.reduce complete dim = 0
#pragma HLS array_partition variable = reduce16_in.data complete
    APP_Reduce_Data8 reduce8_out;
#pragma HLS array_partition variable = reduce8_out.reduce complete dim = 0
#pragma HLS array_partition variable = reduce8_out.data complete
    if (app_rlf_rdc_16to8_input_data.read_nb(reduce16_in)) {
      for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
        for (int j = 0; j < APP_FEATURE_PER_CYCLE; j += 2) {
#pragma HLS unroll
          reduce8_out.reduce[j >> 1] =
              reduce16_in.reduce[j] + reduce16_in.reduce[j + 1];
        }
      }
      for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
        reduce8_out.data[i] = reduce16_in.data[i];
      }
      reduce8_out.posi = reduce16_in.posi;
      reduce8_out.idx = reduce16_in.idx;
      reduce8_out.eot = reduce16_in.eot;
      app_rlf_rdc_8to4_input_data.write(reduce8_out);
    }
  }
}

#endif
