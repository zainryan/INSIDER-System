#ifndef APP_RLF_FLT2_CPP_
#define APP_RLF_FLT2_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_flt2(ST_Queue<APP_Flt_Data2> &app_rlf_flt2_input_data,
                  ST_Queue<APP_Write_Data> &app_rlf_wr_input_data) {

  while (1) {
#pragma HLS pipeline
    APP_Flt_Data2 filter2_in;
#pragma HLS array_partition variable = filter2_in.data complete
#pragma HLS array_partition variable = filter2_in.validNum complete
    APP_Write_Data write_out;
#pragma HlS array_partition variable = write_out.data complete
    if (app_rlf_flt2_input_data.read_nb(filter2_in)) {
      for (int m = 0; m < APP_FEATURE_PER_CYCLE; m++) {
#pragma HLS unroll
        if (filter2_in.validNum[0] > m) {
          write_out.data[m] = filter2_in.data[m];
        } else if (filter2_in.validNum[0] + filter2_in.validNum[1] > m) {
          write_out.data[m] = filter2_in.data[m + 8 - filter2_in.validNum[0]];
        }
      }
      write_out.validNum = filter2_in.validNum[0] + filter2_in.validNum[1];
      write_out.eop = filter2_in.eop;
      app_rlf_wr_input_data.write(write_out);
    }
  }
}

#endif
