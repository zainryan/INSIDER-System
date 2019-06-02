#include "structure.h"
#include <insider_kernel.h>

void app_conv_mult_dummy(
    ST_Queue<bool> &reset_app_conv_mult_dummy,
    ST_Queue<APP_Param> &app_conv_mult_dummy_param,
    ST_Queue<APP_Mult_Data> &app_conv_mult_dummy_input_data) {

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_mult_dummy.read_nb(dummy))) {
      struct APP_Mult_Data dummy0;
      app_conv_mult_dummy_input_data.read_nb(dummy0);
      struct APP_Param dummy1;
      app_conv_mult_dummy_param.read_nb(dummy1);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      APP_Mult_Data dummy;
      APP_Param dummy_param;
      app_conv_mult_dummy_input_data.read_nb(dummy);
      app_conv_mult_dummy_param.read_nb(dummy_param);
    }
  }
}
