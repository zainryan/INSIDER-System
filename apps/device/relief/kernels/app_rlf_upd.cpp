#ifndef APP_RLF_UPD_CPP_
#define APP_RLF_UPD_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_upd(ST_Queue<unsigned int> &app_rlf_upd_input_params,
                 ST_Queue<APP_Upd_Data> &app_rlf_upd_input_data,
                 ST_Queue<APP_Flt_Param> &app_rlf_flt16_input_params) {

  bool validParam = false;
  int readParamCnt = 0;
  int readDataCnt = 0;
  int appThres = 0;
  int totalWeight[APP_FEATURE_DIM];
#pragma HLS array_partition variable = totalWeight cyclic factor = 16 dim = 1
  APP_Flt_Param filter_param_out;
#pragma HLS array_partition variable = filter_param_out complete

  while (1) {
#pragma HLS pipeline
    if (!validParam) {
      unsigned int param;
      if (app_rlf_upd_input_params.read_nb(param)) {
        appThres = (int)param;
        validParam = true;
      }
    } else {
      APP_Upd_Data update_in;
#pragma HLS array_partition variable = update_in.weight complete dim = 0
      if (app_rlf_upd_input_data.read_nb(update_in)) {
        int idx = update_in.idx;
        for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
          totalWeight[idx * APP_FEATURE_PER_CYCLE + i] = 0;
          for (int j = 0; j < APP_QUERY_NUM; j++) {
#pragma HLS unroll
            totalWeight[idx * APP_FEATURE_PER_CYCLE + i] += update_in.weight[i];
          }
          filter_param_out.weight[idx * APP_FEATURE_PER_CYCLE + i] =
              (totalWeight[idx * APP_FEATURE_PER_CYCLE + i] >= appThres);
        }
        if (idx == (APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 1) {
          app_rlf_flt16_input_params.write(filter_param_out);
        }
      }
    }
  }
}

#endif
