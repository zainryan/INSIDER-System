#pragma once

#include <insider_kernel.h>
#include "structure.h"

#ifdef CSIM
#define reg(x) (x)
#else
#include <hls/utils/x_hls_utils.h>
#endif

#define DELAY (2) // power of 2
#define LOG_DELAY (1)
#define BATCH (KERNEL_BATCH_SIZE >> LOG_DELAY)

inline void mult_core(ST_Queue<APP_Param> &app_conv_mult_0_param,
    ST_Queue<APP_Param> &app_conv_mult_1_param,
    ST_Queue<APP_Mult_Data> &app_conv_mult_input_data_0,
    ST_Queue<APP_Mult_Data> &app_conv_mult_input_data_1,
    ST_Queue<APP_Reduce_Data> &app_conv_sec_reduce_input_data,
    Elem kernels[][KERNEL_SIZE],
    APP_Mult_Data *reg_input_data,
    Result_Elem *result,
    bool *param_ready,
    unsigned int  *input_param_cnt,
    unsigned char *input_matrix_blocks,
    unsigned char *phase,
    unsigned char engine_id) {
#pragma HLS inline
  APP_Mult_Data input_data;
  APP_Mult_Data data_replica;
  APP_Param app_param;
  APP_Reduce_Data data_out;

  if ((!(*param_ready)) && app_conv_mult_0_param.read_nb(app_param)) {
    app_conv_mult_1_param.write(app_param);
    if (app_param.idx_blk == engine_id) {
      kernels[app_param.idx_dim_0][app_param.idx_dim_1] = app_param.val;
    } 
    if (++(*input_param_cnt) == NUM_OF_KERNELS * KERNEL_SIZE) {
      *param_ready = true;
    }
  } else if (*param_ready) {
    if ((*phase) == 0 && app_conv_mult_input_data_0.read_nb(input_data)) {
      (*reg_input_data) = reg(input_data);
      data_replica.eop = reg_input_data->eop; 
      data_replica.data = reg_input_data->data; 
      app_conv_mult_input_data_1.write(data_replica);
      *phase = 1;
    }

    if (*phase > 0) {
      unsigned char sec_idx = *input_matrix_blocks;
      (*input_matrix_blocks) = ((*input_matrix_blocks) + 1) & (DBLOCKS_PER_KERNEL - 1);

      unsigned char kernels_idx_dim1[ELEMS_PER_DBLOCK];
#pragma HLS array_partition variable = kernels_idx_dim1 complete dim = 0
      for (char j = 0; j < ELEMS_PER_DBLOCK; j++) {
        kernels_idx_dim1[j] = sec_idx * ELEMS_PER_DBLOCK + j;
      }

      for (char i = 0; i < BATCH; i++) { // MUST change accordingly
        int delta_result = 0;
        int idx = (*phase - 1) * BATCH + i;
        for (char j = 0; j < ELEMS_PER_DBLOCK; j++) {
          char tmp = reg_input_data->data(8 * j + 7, 8 * j);
          delta_result += tmp * kernels[idx][kernels_idx_dim1[j]];
        }
        result[idx] = delta_result;
      }

      if (*phase == DELAY) {  // last phase
        data_out.id = sec_idx;
        data_out.eop = reg_input_data->eop;
        for (int i = 0; i < KERNEL_BATCH_SIZE; i++) {
#pragma HLS unroll
            data_out.data(16 * i + 15, 16 * i) = result[i];
        }
        app_conv_sec_reduce_input_data.write(data_out);
        *phase = 0;
      } else  *phase = *phase + 1;
    }
  }
}
