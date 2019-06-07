#include "combine_l1.h"
#include "structure.h"
#include <insider_kernel.h>

void app_conv_combine_l1_1(
    ST_Queue<bool> &reset_app_conv_combine_l1_1,
    ST_Queue<APP_Combine_L1_Data> &app_conv_combine_l1_input_data_2,
    ST_Queue<APP_Combine_L1_Data> &app_conv_combine_l1_input_data_3,
    ST_Queue<APP_Combine_L2_Data> &app_conv_combine_l2_input_data_1) {
  struct APP_Combine_L1_Data input_data_2;
  ;
  struct APP_Combine_L1_Data input_data_3;
  ;
  ;
  _Bool data_2_ready = false;
  ;
  ;
  _Bool data_3_ready = false;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_combine_l1_1.read_nb(dummy))) {
      input_data_2 = APP_Combine_L1_Data();
      input_data_3 = APP_Combine_L1_Data();
      data_2_ready = false;
      data_3_ready = false;
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      combine_l1(app_conv_combine_l1_input_data_2,
                 app_conv_combine_l1_input_data_3,
                 &app_conv_combine_l2_input_data_1, &input_data_2,
                 &input_data_3, &data_2_ready, &data_3_ready);
    }
  }
}
