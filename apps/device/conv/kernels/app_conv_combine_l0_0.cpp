#include <insider_kernel.h>
#include "structure.h"
#include "combine_l0.h"

void app_conv_combine_l0_0(
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_0,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_1,
    ST_Queue<APP_Combine_L1_Data> &app_conv_combine_l1_input_data_0) {
    APP_Combine_L0_Data input_data_0;
    APP_Combine_L0_Data input_data_1;
    bool data_0_ready = false;
    bool data_1_ready = false;
  while (1) {
#pragma HLS pipeline
    combine_l0(
        app_conv_combine_l0_input_data_0,
        app_conv_combine_l0_input_data_1,
        &app_conv_combine_l1_input_data_0,
        &input_data_0,
        &input_data_1,
        &data_0_ready,
        &data_1_ready);
  }
}
