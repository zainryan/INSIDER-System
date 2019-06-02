#include "row_reduce.h"
#include "structure.h"
#include <insider_kernel.h>

void app_conv_row_reduce_5(
    ST_Queue<bool> &reset_app_conv_row_reduce_5,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_10_input_data,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_11_input_data,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_5) {
  Result_Elem total[4];
  ;
  struct APP_Reduce_Data input_data_10;
  ;
  ;
#pragma HLS array_partition variable = total complete dim = 0
  struct APP_Reduce_Data input_data_11;
  ;
  ;
  _Bool init_param = false;
  ;
  ;
  _Bool data_10_ready = false;
  ;
  ;
  _Bool data_11_ready = false;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_row_reduce_5.read_nb(dummy))) {
      input_data_10 = APP_Reduce_Data();
      input_data_11 = APP_Reduce_Data();
      init_param = false;
      data_10_ready = false;
      data_11_ready = false;
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

      row_reduce(app_conv_row_reduce_10_input_data,
                 app_conv_row_reduce_11_input_data,
                 &app_conv_combine_l0_input_data_5, &input_data_10,
                 &input_data_11, total, &data_10_ready, &data_11_ready);
    }
  }
}
