#include <insider_kernel.h>
#include "structure.h"
#include "combine_l1.h"

void app_conv_combine_l1_1(
    ST_Queue<APP_Combine_L1_Data> &app_conv_combine_l1_input_data_2,
    ST_Queue<APP_Combine_L1_Data> &app_conv_combine_l1_input_data_3,
    ST_Queue<APP_Combine_L2_Data> &app_conv_combine_l2_input_data_1) {
    APP_Combine_L1_Data input_data_2;
    APP_Combine_L1_Data input_data_3;
    bool data_2_ready = false;
    bool data_3_ready = false;
  while (1) {
#pragma HLS pipeline
    combine_l1(
        app_conv_combine_l1_input_data_2,
        app_conv_combine_l1_input_data_3,
        &app_conv_combine_l2_input_data_1,
        &input_data_2,
        &input_data_3,
        &data_2_ready,
        &data_3_ready);
  }
}
