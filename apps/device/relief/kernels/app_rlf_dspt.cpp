#ifndef APP_RLF_DSPT_CPP_
#define APP_RLF_DSPT_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_rlf_dspt(ST_Queue<unsigned int> &app_rlf_input_params,
                  ST_Queue<APP_Data> &app_rlf_input_data,
                  ST_Queue<unsigned int> &app_rlf_upd_input_params,
                  ST_Queue<unsigned int> &app_rlf_dist_input_params,
                  ST_Queue<unsigned int> &app_rlf_diff_input_params,
                  ST_Queue<APP_Dist_Data> &app_rlf_dist_input_data,
                  ST_Queue<APP_Flt_Data16> &app_rlf_flt16_input_data,
                  ST_Queue<bool> &app_rlf_max_min_init) {
  bool valid_param = false;
  unsigned int readParamCnt = 0;
  unsigned int readDataCnt = 0;
  unsigned int posSetNum = 0;
  unsigned int negSetNum = 0;

  while (1) {
#pragma HLS pipeline
    if (!valid_param) {
      unsigned int param;
      if (app_rlf_input_params.read_nb(param)) {
        if (readParamCnt == 0) {
          app_rlf_upd_input_params.write(param);
        } else if (readParamCnt == 1) {
          posSetNum = (param * APP_FEATURE_DIM) >> APP_FEATURE_PER_CYCLE_LOG2;
        } else if (readParamCnt == 2) {
          negSetNum = (param * APP_FEATURE_DIM) >> APP_FEATURE_PER_CYCLE_LOG2;
        } else {
          app_rlf_dist_input_params.write(param);
          app_rlf_diff_input_params.write(param);

          if (readParamCnt == APP_QUERY_NUM * APP_FEATURE_DIM + 2) {
            valid_param = true;
            app_rlf_max_min_init.write(true);
          }
        }
        readParamCnt++;
      }
    } else {
      APP_Data app_data_in;
      APP_Dist_Data dist_out;
#pragma HLS array_partition variable = dist_out.data complete
      APP_Flt_Data16 filter16_out;
#pragma HLS array_partition variable = filter16_out.data complete
      if (app_rlf_input_data.read_nb(app_data_in)) {
        if (readDataCnt < posSetNum + negSetNum) {
          for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
            dist_out.data[i] = app_data_in.data(32 * i + 31, 32 * i);
          }
          dist_out.idx = readDataCnt &
                         ((APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 1);
          if (readDataCnt == (posSetNum + negSetNum) - 1) {
            dist_out.eot = true;
          } else {
            dist_out.eot = false;
          }
          if (readDataCnt < posSetNum) {
            dist_out.posi = true;
          } else {
            dist_out.posi = false;
          }
          app_rlf_dist_input_data.write(dist_out);
        } else {
          for (int i = 0; i < APP_FEATURE_PER_CYCLE; i++) {
#pragma HLS unroll
            filter16_out.data[i] = app_data_in.data(32 * i + 31, 32 * i);
          }
          filter16_out.idx =
              readDataCnt &
              ((APP_FEATURE_DIM >> APP_FEATURE_PER_CYCLE_LOG2) - 1);
          filter16_out.eop = app_data_in.eop;
          app_rlf_flt16_input_data.write(filter16_out);
        }
        readDataCnt++;
      }
    }
  }
}
#endif
