#include <insider_kernel.h>
#include "structure.h"
#include "row_reduce.h"

void app_conv_row_reduce_4(
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_8_input_data,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_9_input_data,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_4) {
  Result_Elem total[2 * KERNEL_BATCH_SIZE];
#pragma HLS array_partition variable=total complete dim=0
  APP_Reduce_Data input_data_8;
  APP_Reduce_Data input_data_9;
  bool init_param = false;
    bool data_8_ready = false;
    bool data_9_ready = false;
  while (1) {
#pragma HLS pipeline
    if (!init_param) {
      for (unsigned int i = 0; i < KERNEL_BATCH_SIZE; i++) {
#pragma HLS unroll
        total[i] = 0;
      }
      init_param = true;
    }

    row_reduce(
        app_conv_row_reduce_8_input_data,
        app_conv_row_reduce_9_input_data,
        &app_conv_combine_l0_input_data_4,
        &input_data_8,
        &input_data_9,
        total,
        &data_8_ready,
        &data_9_ready);
  }
}
