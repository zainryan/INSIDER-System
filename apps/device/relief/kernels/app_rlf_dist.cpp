#ifndef APP_RLF_DIST_CPP_
#define APP_RLF_DIST_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_dist(ST_Queue<APP_Dist_Data> &app_rlf_dist_input_data,
                  ST_Queue<unsigned int> &app_rlf_dist_input_params,
                  ST_Queue<APP_Reduce_Data16> &app_rlf_rdc_16to8_input_data) {

  bool valid_param = false;
  int readParamCnt = 0;
  int query[APP_QUERY_NUM][APP_FEATURE_DIM];
#pragma HLS array_partition variable = query complete dim = 1
#pragma HLS array_partition variable = query cyclic factor = 16 dim = 2

  while (1) {
#pragma HLS pipeline
    if (!valid_param) {
      unsigned int param;
      if (app_rlf_dist_input_params.read_nb(param)) {
        query[readParamCnt >> APP_FEATURE_DIM_LOG]
             [readParamCnt & (APP_FEATURE_DIM - 1)] = (int)param;
        readParamCnt++;
        if (readParamCnt == APP_QUERY_NUM * APP_FEATURE_DIM) {
          valid_param = true;
        }
      }
    } else {
      APP_Dist_Data dist_in;
#pragma HLS array_partition variable = dist_in.data complete
      APP_Reduce_Data16 reduce16_out;
#pragma HLS array_partition variable = reduce16_out.reduce complete dim = 0
#pragma HLS array_partition variable = reduce16_out.data complete
      if (app_rlf_dist_input_data.read_nb(dist_in)) {
        int idx = dist_in.idx;
        for (int j = 0; j < APP_FEATURE_PER_CYCLE; j++) {
#pragma HLS unroll
          for (int i = 0; i < APP_QUERY_NUM; i++) {
#pragma HLS unroll

            reduce16_out.reduce[j] =
                (dist_in.data[j] - query[i][APP_FEATURE_PER_CYCLE * idx + j]) *
                (dist_in.data[j] - query[i][APP_FEATURE_PER_CYCLE * idx + j]);
          }
          reduce16_out.data[j] = dist_in.data[j];
        }
        reduce16_out.posi = dist_in.posi;
        reduce16_out.idx = dist_in.idx;
        reduce16_out.eot = dist_in.eot;
        app_rlf_rdc_16to8_input_data.write(reduce16_out);
      }
    }
  }
}

#endif
