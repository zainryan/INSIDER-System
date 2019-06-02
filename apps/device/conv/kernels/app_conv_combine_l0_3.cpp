#include <insider_kernel.h>
#include "structure.h"
#include "combine_l0.h"

void app_conv_combine_l0_3(
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_6,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_7,
    ST_Queue<APP_Combine_L1_Data> &app_conv_writer_input_data_3) {
    APP_Combine_L0_Data input_data_6;
    APP_Combine_L0_Data input_data_7;
    bool data_6_ready = false;
    bool data_7_ready = false;
  while (1) {
#pragma HLS pipeline
    combine_l0(
        app_conv_combine_l0_input_data_6,
        app_conv_combine_l0_input_data_7,
        &app_conv_writer_input_data_3,
        &input_data_6,
        &input_data_7,
        &data_6_ready,
        &data_7_ready);
  }
}
