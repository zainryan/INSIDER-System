#ifndef APP_INPUT_DATA_MERGER_CPP_
#define APP_INPUT_DATA_MERGER_CPP_

#include <insider_kernel.h>

// The LBA is block-aligned which implies that it is also 64B-aligned.
void app_input_data_merger(ST_Queue<APP_Data> &pre_merged_app_input_data,
                           ST_Queue<APP_Data> &app_input_data,
                           ST_Queue<bool> &reset_app_input_data_merger) {
  APP_Data delayed_app;
  bool has_delayed_app = false;

  bool reset = false;
  unsigned int reset_cnt = 0;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset || (reset = reset_app_input_data_merger.read_nb(dummy))) {
      has_delayed_app = false;
      APP_Data dummy_0;
      pre_merged_app_input_data.read_nb(dummy_0);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      APP_Data app_data;
      if (pre_merged_app_input_data.read_nb(app_data)) {
        if (!has_delayed_app) {
          has_delayed_app = true;
          delayed_app = app_data;
        } else {
          if (app_data.eop) {
            // app_data must be an empty flit which only has eop set.
            delayed_app.eop = true;
            app_input_data.write(delayed_app);
          } else {
            app_input_data.write(delayed_app);
            delayed_app = app_data;
          }
        }
      }
    }
  }
}

#endif
