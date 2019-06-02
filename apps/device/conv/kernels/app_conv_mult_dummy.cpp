#include <insider_kernel.h>
#include "structure.h"

void app_conv_mult_dummy( 
    ST_Queue<APP_Param> &app_conv_mult_dummy_param,
    ST_Queue<APP_Mult_Data> &app_conv_mult_dummy_input_data
    ) {
  while (1) {
#pragma HLS pipeline
    APP_Mult_Data dummy;
    APP_Param dummy_param;
    app_conv_mult_dummy_input_data.read_nb(dummy);
    app_conv_mult_dummy_param.read_nb(dummy_param);
  }   
}
