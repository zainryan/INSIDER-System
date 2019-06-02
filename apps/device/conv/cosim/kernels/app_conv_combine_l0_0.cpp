#include "combine_l0.h"
#include "structure.h"
#include <insider_kernel.h>

void app_conv_combine_l0_0(
    ST_Queue<bool> &reset_app_conv_combine_l0_0,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_0,
    ST_Queue<APP_Combine_L0_Data> &app_conv_combine_l0_input_data_1,
    ST_Queue<APP_Combine_L1_Data> &app_conv_combine_l1_input_data_0) {
  struct APP_Combine_L0_Data input_data_0;
  ;
  struct APP_Combine_L0_Data input_data_1;
  ;
  ;
  _Bool data_0_ready = false;
  ;
  ;
  _Bool data_1_ready = false;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_combine_l0_0.read_nb(dummy))) {
      input_data_0 = APP_Combine_L0_Data();
      input_data_1 = APP_Combine_L0_Data();
      data_0_ready = false;
      data_1_ready = false;
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      combine_l0(app_conv_combine_l0_input_data_0,
                 app_conv_combine_l0_input_data_1,
                 &app_conv_combine_l1_input_data_0, &input_data_0,
                 &input_data_1, &data_0_ready, &data_1_ready);
    }
  }
}
