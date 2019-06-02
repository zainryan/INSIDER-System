#include "row_reduce.h"
#include "structure.h"
#include <insider_kernel.h>

void app_conv_row_reduce_7(
    ST_Queue<bool> &reset_app_conv_row_reduce_7,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_14_input_data,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_15_input_data,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_7) {
  Result_Elem total[4];
  ;
  struct APP_Reduce_Data input_data_14;
  ;
  ;
#pragma HLS array_partition variable = total complete dim = 0
  struct APP_Reduce_Data input_data_15;
  ;
  ;
  _Bool init_param = false;
  ;
  ;
  _Bool data_14_ready = false;
  ;
  ;
  _Bool data_15_ready = false;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_row_reduce_7.read_nb(dummy))) {
      input_data_14 = APP_Reduce_Data();
      input_data_15 = APP_Reduce_Data();
      init_param = false;
      data_14_ready = false;
      data_15_ready = false;
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

      row_reduce(app_conv_row_reduce_14_input_data,
                 app_conv_row_reduce_15_input_data,
                 &app_conv_combine_l0_input_data_7, &input_data_14,
                 &input_data_15, total, &data_14_ready, &data_15_ready);
    }
  }
}
