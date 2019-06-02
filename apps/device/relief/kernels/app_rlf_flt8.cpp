#ifndef APP_RLF_FLT8_CPP_
#define APP_RLF_FLT8_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_flt8(ST_Queue<APP_Flt_Data8> &app_rlf_flt8_input_data,
                  ST_Queue<APP_Flt_Data4> &app_rlf_flt4_input_data) {

  while (1) {
#pragma HLS pipeline
    APP_Flt_Data8 filter8_in;
#pragma HLS array_partition variable = filter8_in.data complete
#pragma HLS array_partition variable = filter8_in.validNum complete
    APP_Flt_Data4 filter4_out;
#pragma HlS array_partition variable = filter4_out.data complete
#pragma HLS array_partition variable = filter4_out.validNum complete
    if (app_rlf_flt8_input_data.read_nb(filter8_in)) {
      for (int i = 0; i < (APP_FEATURE_PER_CYCLE >> 1); i += 2) {
#pragma HLS unroll
        for (int m = 0; m < 4; m++) {
#pragma HLS unroll
          if (filter8_in.validNum[i] > m) {
            filter4_out.data[2 * i + m] = filter8_in.data[2 * i + m];
          } else if (filter8_in.validNum[i] + filter8_in.validNum[i + 1] > m) {
            filter4_out.data[2 * i + m] =
                filter8_in.data[2 * i + m - filter8_in.validNum[i] + 2];
          }
        }
        filter4_out.validNum[i >> 1] =
            filter8_in.validNum[i] + filter8_in.validNum[i + 1];
      }
      filter4_out.eop = filter8_in.eop;
      app_rlf_flt4_input_data.write(filter4_out);
    }
  }
}

#endif
