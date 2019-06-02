#ifndef APP_RLF_DIFF_CPP_
#define APP_RLF_DIFF_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_diff(ST_Queue<unsigned int> &app_rlf_diff_input_params,
                  ST_Queue<APP_Diff_Data> &app_rlf_diff_input_data,
                  ST_Queue<APP_Upd_Data> &app_rlf_upd_input_data) {

  bool validParam = false;
  int readParamCnt = 0;
  int readDataCnt = 0;
  int query[APP_QUERY_NUM][APP_FEATURE_DIM];
#pragma HLS array_partition variable = query complete dim = 1
#pragma HLS array_partition variable = query cyclic factor = 16 dim = 2

  while (1) {
#pragma HLS pipeline
    if (!validParam) {
      unsigned int param;
      if (app_rlf_diff_input_params.read_nb(param)) {
        query[readParamCnt >> APP_FEATURE_DIM_LOG]
             [readParamCnt & (APP_FEATURE_DIM - 1)] = (int)param;
        readParamCnt++;
        if (readParamCnt == APP_QUERY_NUM * APP_FEATURE_DIM) {
          validParam = true;
        }
      }
    } else {
      APP_Diff_Data diff_in;
#pragma HLS array_partition variable = diff_in.maxHitVec complete dim = 0
#pragma HLS array_partition variable = diff_in.maxMissVec complete dim = 0
      APP_Upd_Data update_out;
#pragma HLS array_partition variable = update_out.weight complete dim = 0
      if (app_rlf_diff_input_data.read_nb(diff_in)) {
        int idx = diff_in.idx;
        for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
          for (int j = 0; j < APP_FEATURE_PER_CYCLE; j++) {
#pragma HLS unroll
            update_out.weight[j] =
                ((int)(diff_in.maxMissVec[j] ==
                       query[i][idx * APP_FEATURE_PER_CYCLE + j])) -
                ((int)(diff_in.maxHitVec[j] ==
                       query[i][idx * APP_FEATURE_PER_CYCLE + j]));
          }
        }
        update_out.idx = diff_in.idx;
        app_rlf_upd_input_data.write(update_out);
      }
    }
  }
}

#endif
