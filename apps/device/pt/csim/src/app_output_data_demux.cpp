#ifndef APP_OUTPUT_DATA_DEMUX_CPP_
#define APP_OUTPUT_DATA_DEMUX_CPP_

#include <insider_kernel.h>

void app_output_data_demux(ST_Queue<bool> &app_is_write_mode,
                           ST_Queue<APP_Data> &app_output_data,
                           ST_Queue<APP_Data> &read_mode_app_output_data,
                           ST_Queue<APP_Data> &write_mode_app_output_data,
                           ST_Queue<bool> &reset_app_output_data_demux) {
  bool has_is_write_mode = false;
  bool is_write_mode;

  bool reset = false;
  unsigned int reset_cnt = 0;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset || (reset = reset_app_output_data_demux.read_nb(dummy))) {
      has_is_write_mode = false;
      bool dummy_0;
      app_is_write_mode.read_nb(dummy_0);
      APP_Data dummy_1;
      app_output_data.read_nb(dummy_1);

      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      if (has_is_write_mode ||
          (has_is_write_mode = app_is_write_mode.read_nb(is_write_mode))) {
        APP_Data app_data;
        if (app_output_data.read_nb(app_data)) {
          if (is_write_mode) {
            write_mode_app_output_data.write(app_data);
          } else {
            read_mode_app_output_data.write(app_data);
          }
        }
      }
    }
  }
}

#endif
