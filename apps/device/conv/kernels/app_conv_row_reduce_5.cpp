#include <insider_kernel.h>
#include "structure.h"
#include "row_reduce.h"

void app_conv_row_reduce_5(
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_10_input_data,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_11_input_data,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_5) {
  Result_Elem total[2 * KERNEL_BATCH_SIZE];
#pragma HLS array_partition variable=total complete dim=0
  APP_Reduce_Data input_data_10;
  APP_Reduce_Data input_data_11;
  bool init_param = false;
  bool data_10_ready = false;
  bool data_11_ready = false;
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
        app_conv_row_reduce_10_input_data,
        app_conv_row_reduce_11_input_data,
        &app_conv_combine_l0_input_data_5,
        &input_data_10,
        &input_data_11,
        total,
        &data_10_ready,
        &data_11_ready);
  }
}
