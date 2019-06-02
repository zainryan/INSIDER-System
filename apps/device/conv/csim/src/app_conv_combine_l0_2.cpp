#include "combine_l0.h"
#include "structure.h"
#include <insider_kernel.h>

void app_conv_combine_l0_2(
    ST_Queue<bool> &reset_app_conv_combine_l0_2,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_4,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_5,
    ST_Queue<APP_Combine_L1_Data> &app_conv_combine_l1_input_data_2) {
  struct APP_Combine_L0_Data input_data_4;
  ;
  struct APP_Combine_L0_Data input_data_5;
  ;
  ;
  _Bool data_4_ready = false;
  ;
  ;
  _Bool data_5_ready = false;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_combine_l0_2.read_nb(dummy))) {
      input_data_4 = APP_Combine_L0_Data();
      input_data_5 = APP_Combine_L0_Data();
      data_4_ready = false;
      data_5_ready = false;
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      combine_l0(app_conv_combine_l0_input_data_4,
                 app_conv_combine_l0_input_data_5,
                 &app_conv_combine_l1_input_data_2, &input_data_4,
                 &input_data_5, &data_4_ready, &data_5_ready);
    }
  }
}
