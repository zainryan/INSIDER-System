#pragma once 

#include <insider_kernel.h>
#include "structure.h"

inline void row_reduce(
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_input_data_0,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_input_data_1,
    ST_Queue<APP_Combine_L0_Data> *app_conv_combine_l0_input_data,
    APP_Reduce_Data *input_data_0,
    APP_Reduce_Data *input_data_1,
    Result_Elem *total,
    bool *data_0_ready,
    bool *data_1_ready
    ) {
#pragma HLS inline
  APP_Combine_L0_Data data_out;
  if ((*data_0_ready || (*data_0_ready = app_conv_row_reduce_input_data_0.read_nb(*input_data_0))) &&
      (*data_1_ready || (*data_1_ready = app_conv_row_reduce_input_data_1.read_nb(*input_data_1))) 
     ) {

    unsigned char sec_id = input_data_0->id;

    for (int i = 0; i < KERNEL_BATCH_SIZE; i++) {
#pragma HLS unroll
      total[i] += (int) input_data_0->data(16 * i + 15, 16 * i); 
    }
    for (int i = 0; i < KERNEL_BATCH_SIZE; i++) {
#pragma HLS unroll
      total[i + KERNEL_BATCH_SIZE] += (int) input_data_1->data(16 * i + 15, 16 * i); 
    }

    if (sec_id == DBLOCKS_PER_KERNEL - 1) {
      for (int i = 0; i < 2 * KERNEL_BATCH_SIZE; i++) {
#pragma HLS unroll
        data_out.data(16 * i + 15, 16 * i) = total[i];
        total[i] = 0;
      }
      data_out.eop = input_data_0->eop | input_data_1->eop;
      app_conv_combine_l0_input_data->write(data_out);
    }
    *data_0_ready = false;
    *data_1_ready = false;
  }   
}
