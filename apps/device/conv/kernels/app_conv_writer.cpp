#include <insider_kernel.h>
#include "structure.h"

void app_conv_writer( 
    ST_Queue<APP_Data> &app_conv_writer_input_data,
    ST_Queue<APP_Data> &app_output_data) {
  APP_Data input_data;
  APP_Data data_out;
  while (1) {
#pragma HLS pipeline
    if (app_conv_writer_input_data.read_nb(input_data)) {
      data_out.data = input_data.data; 
      data_out.len = 64;
      data_out.eop = input_data.eop;
      app_output_data.write(data_out);
    }
  }
}
