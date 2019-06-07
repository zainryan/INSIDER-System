#pragma once 

#include <insider_kernel.h>
#include "structure.h"

inline void combine_l2(
    ST_Queue<APP_Combine_L2_Data> &app_conv_combine_l2_input_data_0,
    ST_Queue<APP_Combine_L2_Data> &app_conv_combine_l2_input_data_1,
    ST_Queue<APP_Data> *app_conv_write_input_data,
    APP_Combine_L2_Data *input_data_0, 
    APP_Combine_L2_Data *input_data_1,
    bool *data_0_ready,
    bool *data_1_ready
    ) {
#pragma HLS inline
  APP_Data data_out;
  if ((*data_0_ready || (*data_0_ready = app_conv_combine_l2_input_data_0.read_nb(*input_data_0))) &&
      (*data_1_ready || (*data_1_ready = app_conv_combine_l2_input_data_1.read_nb(*input_data_1))) 
     ) {
    data_out.data(255, 0) = input_data_0->data;
    data_out.data(511, 256) = input_data_1->data;

    data_out.eop = input_data_0->eop | input_data_1->eop;
    app_conv_write_input_data->write(data_out);
    *data_0_ready = false;
    *data_1_ready = false;
  }
}   
