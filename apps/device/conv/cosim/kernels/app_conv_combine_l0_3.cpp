#include "combine_l0.h"
#include "structure.h"
#include <insider_kernel.h>

void app_conv_combine_l0_3(
    ST_Queue<bool> &reset_app_conv_combine_l0_3,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_6,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_7,
    ST_Queue<APP_Combine_L1_Data> &app_conv_writer_input_data_3) {
  struct APP_Combine_L0_Data input_data_6;
  ;
  struct APP_Combine_L0_Data input_data_7;
  ;
  ;
  _Bool data_6_ready = false;
  ;
  ;
  _Bool data_7_ready = false;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_combine_l0_3.read_nb(dummy))) {
      input_data_6 = APP_Combine_L0_Data();
      input_data_7 = APP_Combine_L0_Data();
      data_6_ready = false;
      data_7_ready = false;
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      combine_l0(app_conv_combine_l0_input_data_6,
                 app_conv_combine_l0_input_data_7,
                 &app_conv_writer_input_data_3, &input_data_6, &input_data_7,
                 &data_6_ready, &data_7_ready);
    }
  }
}
