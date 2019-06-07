#ifndef APP_KNN_CPP_
#define APP_KNN_CPP_
#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_knn(ST_Queue<APP_Data> &app_input_data,
             ST_Queue<APP_Data> &app_output_data,
             ST_Queue<unsigned int> &app_input_params) {
  ap_uint<MAX_FEATURE_WEIGHT_LOG2> predicting_vec[FEATURE_DIM / 64][64];
#pragma HLS array_partition variable = predicting_vec cyclic factor = 64 dim = 2

  bool valid_predicting_vec = false;
  unsigned int received_predicting_vec_idx = 0;

  unsigned int dist_arr[DATA_BUS_WIDTH / sizeof(unsigned int) / 2];
#pragma HLS array_partition variable = dist_arr complete dim = 1
  unsigned int result_arr[DATA_BUS_WIDTH / sizeof(unsigned int) / 2];
#pragma HLS array_partition variable = result_arr complete dim = 1
  unsigned int dist_arr_idx = 0;

  unsigned int cnt_input_buf = 0;
  unsigned int cur_result;
  unsigned int predicting_vec_dim0_idx = 0;
  unsigned int dist[(RESULT_SIZE + WEIGHT_SIZE * FEATURE_DIM) / DATA_BUS_WIDTH];
#pragma HLS array_partition variable = dist complete dim = 0

  while (1) {
#pragma HLS pipeline
    unsigned int input_param;
    if (!valid_predicting_vec) {
      if (app_input_params.read_nb(input_param)) {
        predicting_vec[received_predicting_vec_idx >> 6]
                      [received_predicting_vec_idx & 63] = input_param;
        received_predicting_vec_idx++;
        if (received_predicting_vec_idx == FEATURE_DIM) {
          valid_predicting_vec = true;
        }
      }
    } else {
      APP_Data input_app_data;
      if (app_input_data.read_nb(input_app_data)) {
        if (cnt_input_buf == 0) {
          cur_result = (input_app_data.data(503, 496) - '0') * 10 +
                       (input_app_data.data(511, 504) - '0');
        } else {
          unsigned char weights[DATA_BUS_WIDTH / WEIGHT_SIZE];
#pragma HLS array_partition variable = weights complete dim = 1
          for (int i = 0; i < DATA_BUS_WIDTH / WEIGHT_SIZE; i++) {
#pragma HLS unroll
            weights[i] = input_app_data.data(i * 8 + 7, i * 8) - '0';
          }

          unsigned int tmp_dist = 0;
          for (int i = 0; i < 64; i++) {
#pragma HLS unroll
            char diff = (char)predicting_vec[predicting_vec_dim0_idx][i] -
                        (char)weights[i];
            unsigned char mul = diff * diff;
            tmp_dist += mul;
          }
          dist[cnt_input_buf] = tmp_dist;
          predicting_vec_dim0_idx++;
        }
        cnt_input_buf++;
        if (cnt_input_buf ==
            (RESULT_SIZE + WEIGHT_SIZE * FEATURE_DIM) / DATA_BUS_WIDTH) {
          unsigned int sum_dist = 0;
          for (int i = 0;
               i < (RESULT_SIZE + WEIGHT_SIZE * FEATURE_DIM) / DATA_BUS_WIDTH;
               i++) {
#pragma HLS pipeline
            sum_dist += dist[i];
          }
          dist_arr[dist_arr_idx] = sum_dist;
          result_arr[dist_arr_idx] = cur_result;
          dist_arr_idx++;
          cnt_input_buf = 0;
          predicting_vec_dim0_idx = 0;
          if (dist_arr_idx == DATA_BUS_WIDTH / sizeof(unsigned int) / 2) {
            dist_arr_idx = 0;
            APP_Data output_app_data;
            output_app_data.eop = input_app_data.eop;
            output_app_data.len = 64;
            for (int idx = 0; idx < 512; idx += 64) {
              output_app_data.data(idx + 31, idx) = dist_arr[idx / 64];
              output_app_data.data(idx + 63, idx + 32) = result_arr[idx / 64];
            }
            app_output_data.write(output_app_data);
          }
        }
      }
    }
  }
}

#endif
