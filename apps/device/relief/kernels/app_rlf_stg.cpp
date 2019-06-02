#ifndef APP_RLF_STG_CPP_
#define APP_RLF_STG_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_stg(ST_Queue<APP_Stg_Data> &app_rlf_stg_input_data,
                 ST_Queue<APP_Diff_Data> &app_rlf_diff_input_data) {

  char writeCnt = 0;
  bool beginWriteOut = false;
  int maxHitVec[APP_QUERY_NUM][APP_FEATURE_DIM];
#pragma HLS array_partition variable = maxHitVec complete dim = 1
#pragma HLS array_partition variable = maxHitVec cyclic factor = 16 dim = 2
  int maxMissVec[APP_QUERY_NUM][APP_FEATURE_DIM];
#pragma HLS array_partition variable = maxMissVec complete dim = 1
#pragma HLS array_partition variable = maxMissVec cyclic factor = 16 dim = 2
  int tmpVec[APP_FEATURE_PER_CYCLE]
            [APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2];
#pragma HLS array_partition variable = tmpVec complete dim = 1
#pragma HLS array_partition variable = tmpVec cyclic factor = 2 dim = 2

  while (1) {
#pragma HLS pipeline
    APP_Stg_Data stg_in;
#pragma HLS array_partition variable = stg_in.data complete
    APP_Diff_Data diff_out;
#pragma HLS array_partition variable = diff_out.maxHitVec complete dim = 0
#pragma HLS array_partition variable = diff_out.maxMissVec complete dim = 0
    if (app_rlf_stg_input_data.read_nb(stg_in)) {
      char idx = stg_in.idx;
      char updateCnt =
          (idx + 1) & ((APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 1);
      int tmp;
      for (int j = 0; j < APP_FEATURE_PER_CYCLE; j++) {
#pragma HLS unroll
        if (idx == (APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 1) {
          tmpVec[j][(APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 1] =
              stg_in.data[j];
          tmp = tmpVec[j][0];
        } else {
          tmpVec[j][idx] = stg_in.data[j];
          tmp = tmpVec[j][idx + 1];
        }
        for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
          if (stg_in.posi && stg_in.update) {
            maxHitVec[i][APP_FEATURE_PER_CYCLE * updateCnt + j] = tmp;
          } else if (!stg_in.posi && stg_in.update) {
            maxMissVec[i][APP_FEATURE_PER_CYCLE * updateCnt + j] = tmp;
          }
        }
      }
      if (stg_in.eot) {
        beginWriteOut = true;
      }
    } else if (beginWriteOut) {
      for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
        for (int j = 0; j < APP_FEATURE_PER_CYCLE; j++) {
#pragma HLS unroll
          diff_out.maxHitVec[j] =
              maxHitVec[i][APP_FEATURE_PER_CYCLE * writeCnt + j];
          diff_out.maxMissVec[j] =
              maxMissVec[i][APP_FEATURE_PER_CYCLE * writeCnt + j];
        }
      }
      diff_out.idx = writeCnt;
      app_rlf_diff_input_data.write(diff_out);
      writeCnt++;
      if (writeCnt == (APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2)) {
        beginWriteOut = false;
        writeCnt = 0;
      }
    }
  }
}
#endif
