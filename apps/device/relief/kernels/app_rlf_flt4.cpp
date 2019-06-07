#ifndef APP_RLF_FLT4_CPP_
#define APP_RLF_FLT4_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_flt4(ST_Queue<APP_Flt_Data4> &app_rlf_flt4_input_data,
                  ST_Queue<APP_Flt_Data2> &app_rlf_flt2_input_data) {

  while (1) {
#pragma HLS pipeline
    APP_Flt_Data4 filter4_in;
#pragma HLS array_partition variable = filter4_in.data complete
#pragma HLS array_partition variable = filter4_in.validNum complete
    APP_Flt_Data2 filter2_out;
#pragma HlS array_partition variable = filter2_out.data complete
#pragma HLS array_partition variable = filter2_out.validNum complete
    if (app_rlf_flt4_input_data.read_nb(filter4_in)) {
      for (int i = 0; i < (APP_FEATURE_PER_CYCLE >> 2); i += 2) {
#pragma HLS unroll
        for (int m = 0; m < 8; m++) {
#pragma HLS unroll
          if (filter4_in.validNum[i] > m) {
            filter2_out.data[4 * i + m] = filter4_in.data[4 * i + m];
          } else if (filter4_in.validNum[i] + filter4_in.validNum[i + 1] > m) {
            filter2_out.data[4 * i + m] =
                filter4_in.data[4 * i + m - filter4_in.validNum[i] + 4];
          }
        }
        filter2_out.validNum[i >> 1] =
            filter4_in.validNum[i] + filter4_in.validNum[i + 1];
      }
      filter2_out.eop = filter4_in.eop;
      app_rlf_flt2_input_data.write(filter2_out);
    }
  }
}

#endif
