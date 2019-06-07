#include "row_reduce.h"
#include "structure.h"
#include <insider_kernel.h>

void app_conv_row_reduce_6(
    ST_Queue<bool> &reset_app_conv_row_reduce_6,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_12_input_data,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_13_input_data,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_6) {
  Result_Elem total[4];
  ;
  struct APP_Reduce_Data input_data_12;
  ;
  ;
#pragma HLS array_partition variable = total complete dim = 0
  struct APP_Reduce_Data input_data_13;
  ;
  ;
  _Bool init_param = false;
  ;
  ;
  _Bool data_12_ready = false;
  ;
  ;
  _Bool data_13_ready = false;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_row_reduce_6.read_nb(dummy))) {
      input_data_12 = APP_Reduce_Data();
      input_data_13 = APP_Reduce_Data();
      init_param = false;
      data_12_ready = false;
      data_13_ready = false;
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      if (!init_param) {
        for (unsigned int i = 0; i < KERNEL_BATCH_SIZE; i++) {
#pragma HLS unroll
          total[i] = 0;
        }
        init_param = true;
      }

      row_reduce(app_conv_row_reduce_12_input_data,
                 app_conv_row_reduce_13_input_data,
                 &app_conv_combine_l0_input_data_6, &input_data_12,
                 &input_data_13, total, &data_12_ready, &data_13_ready);
    }
  }
}
