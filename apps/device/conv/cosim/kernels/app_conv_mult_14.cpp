#include "mult_core.h"
#include "structure.h"
#include <insider_kernel.h>

void app_conv_mult_14( // total 8 mult engines
    ST_Queue<bool> &reset_app_conv_mult_14,
    ST_Queue<APP_Param> &app_conv_mult_14_param,
    ST_Queue<APP_Param> &app_conv_mult_15_param,
    ST_Queue<APP_Mult_Data> &app_conv_mult_14_input_data,
    ST_Queue<APP_Mult_Data> &app_conv_mult_15_input_data,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_14_input_data) {
  Elem kernels[2][64];
  ;
  Result_Elem results[2];
  ;
  ;
#pragma HLS array_partition variable = kernels complete dim = 0
  unsigned char input_kernel_blocks = 0;
  ;
  ;
#pragma HLS array_partition variable = results complete dim = 0
  unsigned char input_matrix_blocks = 0;
  ;
  ;
  unsigned char phase = 0;
  ;
  ;
  struct APP_Mult_Data reg_input_data;
  ;
  ;
  struct APP_Reduce_Data data_out;
  ;
  ;
  _Bool param_ready = false;
  ;
  ;
  unsigned int input_param_cnt = 0;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_mult_14.read_nb(dummy))) {
      input_kernel_blocks = 0;
      input_matrix_blocks = 0;
      phase = 0;
      reg_input_data = APP_Mult_Data();
      data_out = APP_Reduce_Data();
      param_ready = false;
      input_param_cnt = 0;
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      mult_core(app_conv_mult_14_param, app_conv_mult_15_param,
                app_conv_mult_14_input_data, app_conv_mult_15_input_data,
                app_conv_row_reduce_14_input_data, kernels, &reg_input_data,
                results, &param_ready, &input_param_cnt, &input_matrix_blocks,
                &phase, 14);
    }
  }
}
