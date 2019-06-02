#ifndef APP_RLF_MAX_VEC_CPP_
#define APP_RLF_MAX_VEC_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_max_vec(ST_Queue<APP_Reduce_Data> &app_rlf_max_vec_input_data,
                     ST_Queue<APP_Stg_Data> &app_rlf_stg_input_data,
                     ST_Queue<bool> &app_rlf_max_vec_init) {

  bool validParam = false;
  bool writeTail = false;
  int writeCnt = 0;
  long long finalDist[APP_QUERY_NUM];
#pragma HLS array_partition variable = finalDist complete
  long long maxHitDist[APP_QUERY_NUM];
#pragma HLS array_partition variable = maxHitDist complete
  long long maxMissDist[APP_QUERY_NUM];
#pragma HLS array_partition variable = maxMissDist complete
  APP_Stg_Data stg_out;
#pragma HLS array_partition variable = stg_out.data complete dim = 1

  while (1) {
#pragma HLS pipeline
    if (!validParam) {
      bool param;
      if (app_rlf_max_vec_init.read_nb(param)) {
        for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
          maxHitDist[i] = LLONG_MAX;
          maxMissDist[i] = LLONG_MAX;
          stg_out.update = false;
        }
        validParam = true;
      }
    } else {
      APP_Reduce_Data reduce_in;
#pragma HLS array_partition variable = reduce_in.data complete
      if (app_rlf_max_vec_input_data.read_nb(reduce_in)) {
        for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll
          if (reduce_in.idx == 0) {
            finalDist[i] = 0;
          }
          finalDist[i] += reduce_in.reduce;
          if (reduce_in.idx ==
              (APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 1) {
            if (reduce_in.posi) {
              if (maxHitDist[i] > finalDist[i]) {
                maxHitDist[i] = finalDist[i];
                stg_out.update = true;
              } else {
                stg_out.update = false;
              }
            } else {
              if (maxMissDist[i] > finalDist[i]) {
                maxMissDist[i] = finalDist[i];
                stg_out.update = true;
              } else {
                stg_out.update = false;
              }
            }
          }
        }

        for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
          stg_out.data[i] = reduce_in.data[i];
        }

        if (reduce_in.eot) {
          writeTail = true;
        }
        stg_out.idx = reduce_in.idx;
        stg_out.eot = false;
        if (reduce_in.idx ==
            (APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 1) {
          stg_out.posi = reduce_in.posi;
        }
        app_rlf_stg_input_data.write(stg_out);
      } else if (writeTail) {
        stg_out.idx = writeCnt;
        stg_out.posi = false; // neg + tail => fill the tail of neg
        if (writeCnt == (APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 2) {
          stg_out.eot = true;
          writeTail = false;
        } else {
          stg_out.eot = false;
        }
        writeCnt++;
        app_rlf_stg_input_data.write(stg_out);
      }
    }
  }
}

#endif
