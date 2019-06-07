#include <insider_kernel.h>
#include "structure.h"
#include "combine_l0.h"

void app_conv_combine_l0_2(
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_4,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_5,
    ST_Queue<APP_Combine_L1_Data> &app_conv_combine_l1_input_data_2) {
    APP_Combine_L0_Data input_data_4;
    APP_Combine_L0_Data input_data_5;
    bool data_4_ready = false;
    bool data_5_ready = false;
  while (1) {
#pragma HLS pipeline
    combine_l0(
        app_conv_combine_l0_input_data_4,
        app_conv_combine_l0_input_data_5,
        &app_conv_combine_l1_input_data_2,
        &input_data_4,
        &input_data_5,
        &data_4_ready,
        &data_5_ready);
  }
}
