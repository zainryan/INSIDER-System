#ifndef APP_PT_CPP_
#define APP_PT_CPP_
#include <insider_kernel.h>

void app_pt(ST_Queue<APP_Data> &app_input_data,
             ST_Queue<APP_Data> &app_output_data,
             ST_Queue<unsigned int> &app_input_params) {
  while (1) {
#pragma HLS pipeline
    unsigned int dummy;
    app_input_params.read_nb(dummy);

    APP_Data data;
    if (app_input_data.read_nb(data)) {
      app_output_data.write(data);
    }
  }
}

#endif
