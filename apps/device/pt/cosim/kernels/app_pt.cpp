#ifndef APP_PT_CPP_
#define APP_PT_CPP_
#include <insider_kernel.h>

void app_pt(ST_Queue<bool> &reset_app_pt, ST_Queue<APP_Data> &app_input_data,
            ST_Queue<APP_Data> &app_output_data,
            ST_Queue<unsigned int> &app_input_params) {

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_pt.read_nb(dummy))) {
      unsigned int dummy0;
      app_input_params.read_nb(dummy0);
      struct APP_Data dummy1;
      app_input_data.read_nb(dummy1);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      unsigned int dummy;
      app_input_params.read_nb(dummy);

      APP_Data data;
      if (app_input_data.read_nb(data)) {
        app_output_data.write(data);
      }
    }
  }
}

#endif
