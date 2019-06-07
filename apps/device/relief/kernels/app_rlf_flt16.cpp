#ifndef APP_RLF_FLT16_CPP_
#define APP_RLF_FLT16_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_flt16(ST_Queue<APP_Flt_Param> &app_rlf_flt16_input_params,
                   ST_Queue<APP_Flt_Data16> &app_rlf_flt16_input_data,
                   ST_Queue<APP_Flt_Data8> &app_rlf_flt8_input_data) {

  bool validParam = false;
  bool weight[APP_FEATURE_DIM];
#pragma HLS array_partition variable = weight complete

  while (1) {
#pragma HLS pipeline
    if (!validParam) {
      APP_Flt_Param param;
#pragma HLS array_partition variable = param complete
      if (app_rlf_flt16_input_params.read_nb(param)) {
        for (int i = 0; i < APP_FEATURE_DIM; i++) {
#pragma HLS unroll
          weight[i] = param.weight[i];
        }
        validParam = true;
      }
    } else {
      APP_Flt_Data16 filter16_in;
#pragma HLS array_partition variable = filter16_in.data complete
      APP_Flt_Data8 filter8_out;
#pragma HlS array_partition variable = filter8_out.data complete
#pragma HLS array_partition variable = filter8_out.validNum complete
      if (app_rlf_flt16_input_data.read_nb(filter16_in)) {
        int idx = filter16_in.idx;
        for (int i = 0; i < APP_FEATURE_PER_CYCLE; i += 2) {
#pragma HLS unroll
          if (weight[APP_FEATURE_PER_CYCLE * idx + i] &&
              weight[APP_FEATURE_PER_CYCLE * idx + i + 1]) {
            filter8_out.data[i] = filter16_in.data[i];
            filter8_out.data[i + 1] = filter16_in.data[i + 1];
            filter8_out.validNum[i >> 1] = 2;
          } else if (weight[APP_FEATURE_PER_CYCLE * idx + i]) {
            filter8_out.data[i] = filter16_in.data[i];
            filter8_out.validNum[i >> 1] = 1;
          } else if (weight[APP_FEATURE_PER_CYCLE * idx + i + 1]) {
            filter8_out.data[i] = filter16_in.data[i + 1];
            filter8_out.validNum[i >> 1] = 1;
          } else {
            filter8_out.data[i] = filter16_in.data[i];
            filter8_out.validNum[i >> 1] = 0;
          }
        }
        filter8_out.eop = filter16_in.eop;
        app_rlf_flt8_input_data.write(filter8_out);
      }
    }
  }
}

#endif
