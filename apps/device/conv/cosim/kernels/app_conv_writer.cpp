#include "structure.h"
#include <insider_kernel.h>

void app_conv_writer(ST_Queue<bool> &reset_app_conv_writer,
                     ST_Queue<APP_Data> &app_conv_writer_input_data,
                     ST_Queue<APP_Data> &app_output_data) {
  struct APP_Data input_data;
  ;
  struct APP_Data data_out;
  ;
  ;
  ;

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_conv_writer.read_nb(dummy))) {
      input_data = APP_Data();
      data_out = APP_Data();
      struct APP_Data dummy0;
      app_conv_writer_input_data.read_nb(dummy0);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      if (app_conv_writer_input_data.read_nb(input_data)) {
        data_out.data = input_data.data;
        data_out.len = 64;
        data_out.eop = input_data.eop;
        app_output_data.write(data_out);
      }
    }
  }
}
