#ifndef APP_RLF_WR_CPP_
#define APP_RLF_WR_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_wr(ST_Queue<APP_Write_Data> &app_rlf_wr_input_data,
                ST_Queue<APP_Data> &app_rlf_output_data) {
  char writeCnt = 0;
  APP_Data app_data_out;
  bool lastPkt = false;
  int stgVecS1[APP_FEATURE_PER_CYCLE];
#pragma HLS array_partition variable = stgVecS1 complete dim = 0
  int stgVecS2[APP_FEATURE_PER_CYCLE];
#pragma HLS array_partition variable = stgVecS2 complete dim = 0
  int stgValidNum = 0;

  while (1) {
#pragma HLS pipeline
    APP_Write_Data write_in;
#pragma HLS array_partition variable = write_in.data complete
    if (app_rlf_wr_input_data.read_nb(write_in)) {
      if (stgValidNum + write_in.validNum < APP_FEATURE_PER_CYCLE) {
        for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
          if (i >= stgValidNum && i < stgValidNum + write_in.validNum) {
            stgVecS1[i] = write_in.data[i - stgValidNum];
          }
        }
        stgValidNum += write_in.validNum;
        if (write_in.eop) {
          lastPkt = true;
        }
      } else {
        for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
          if (i < stgValidNum) {
            stgVecS2[i] = stgVecS1[i];
          } else {
            stgVecS2[i] = write_in.data[i - stgValidNum];
          }
          if (i < write_in.validNum - (APP_FEATURE_PER_CYCLE - stgValidNum)) {
            stgVecS1[i] =
                write_in.data[i + APP_FEATURE_PER_CYCLE - stgValidNum];
          }
        }
        stgValidNum = write_in.validNum - (APP_FEATURE_PER_CYCLE - stgValidNum);
        for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
          app_data_out.data(32 * i + 31, 32 * i) = stgVecS2[i];
        }
        if (write_in.eop && stgValidNum > 0) {
          app_data_out.eop = false;
          lastPkt = true;
        } else {
          app_data_out.eop = write_in.eop;
        }
        app_data_out.len = 64;
        app_rlf_output_data.write(app_data_out);
      }
    } else if (lastPkt) {
      for (char i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
        app_data_out.data(32 * i + 31, 32 * i) = stgVecS1[i];
      }
      lastPkt = false;
      app_data_out.len = stgValidNum * sizeof(int);
      app_data_out.eop = true;
      app_rlf_output_data.write(app_data_out);
    }
  }
}

#endif
