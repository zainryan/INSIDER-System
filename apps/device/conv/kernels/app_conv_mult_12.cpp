#include <insider_kernel.h>
#include "structure.h"
#include "mult_core.h"

void app_conv_mult_12( // total 8 mult engines
    ST_Queue<APP_Param> &app_conv_mult_12_param,
    ST_Queue<APP_Param> &app_conv_mult_13_param,
    ST_Queue<APP_Mult_Data> &app_conv_mult_12_input_data,
    ST_Queue<APP_Mult_Data> &app_conv_mult_13_input_data,
    ST_Queue<APP_Reduce_Data> &app_conv_row_reduce_12_input_data) {
  Elem kernels[KERNEL_BATCH_SIZE][KERNEL_SIZE]; 
#pragma HLS array_partition variable = kernels complete dim=0
  Result_Elem results[KERNEL_BATCH_SIZE];
#pragma HLS array_partition variable=results complete dim=0
  unsigned char input_kernel_blocks = 0;
  unsigned char input_matrix_blocks = 0;
  unsigned char phase = 0;
  APP_Mult_Data reg_input_data;
  APP_Reduce_Data data_out;
  bool param_ready = false;
  unsigned int input_param_cnt = 0;
  while (1) {
#pragma HLS pipeline
    mult_core(
        app_conv_mult_12_param,
        app_conv_mult_13_param,
        app_conv_mult_12_input_data,
        app_conv_mult_13_input_data,
        app_conv_row_reduce_12_input_data,
        kernels, 
        &reg_input_data,
        results,
        &param_ready,
        &input_param_cnt,
        &input_matrix_blocks,
        &phase,
        12);
  }   
}
