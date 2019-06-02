#include <insider_kernel.h>
#include "structure.h"

#ifndef CSIM
#include <hls/utils/x_hls_utils.h>
#else
#define reg(x) (x)
#endif

void app_conv_dist(ST_Queue<unsigned int> &app_params,
    ST_Queue<APP_Data> &app_input_data,
    ST_Queue<APP_Param> &app_conv_mult_0_param,
    ST_Queue<APP_Mult_Data> &app_conv_mult_0_input_data
    ) {
  bool param_ready = false;
  long long count = 0;
  while (1) {
#pragma HLS pipeline
    APP_Data input_data;
    APP_Mult_Data data_out;
    unsigned int param;
    APP_Param param_out;

    if ((!param_ready) && app_params.read_nb(param)) {
      param_out.idx_blk = ((param >> 24) & 0xFF);
      param_out.idx_dim_0 = ((param >> 16) & 0xFF);
      param_out.idx_dim_1 = ((param >> 8) & 0xFF);
      param_out.val = ((param >> 0) & 0xFF);
      app_conv_mult_0_param.write(param_out);
      if ((++count) == NUM_OF_KERNELS * KERNEL_SIZE) {
        param_ready = true;         
      }

    } else if (param_ready && app_input_data.read_nb(input_data)) {
      data_out.eop = input_data.eop;
      data_out.data = input_data.data;
      app_conv_mult_0_input_data.write(reg(data_out));
    }
  }
}
