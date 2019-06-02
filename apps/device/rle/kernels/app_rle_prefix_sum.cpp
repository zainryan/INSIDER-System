#include <insider_kernel.h>

#include "structure.h"

#ifdef CSIM
#define reg(x) (x)
#else
#include <hls/utils/x_hls_utils.h>
#endif

void app_rle_prefix_sum(ST_Queue<unsigned int> &app_input_param,
                        ST_Queue<APP_Data> &app_input_data,
                        ST_Queue<APP_Expand_Data> &app_rle_expand_input_data) {
  bool init = false;
  APP_Data data_in;
  APP_Expand_Data data_out;
#pragma HLS array_partition variable = data_out.prefix_sum complete dim = 0
#pragma HLS array_partition variable = data_out.alpha_array complete dim = 0
  while (1) {
#pragma HLS pipeline
    unsigned int dummy;
    app_input_param.read_nb(dummy);

    if (!init) {
      data_out.prefix_sum[0] = 0;
      init = true;
    } else if (app_input_data.read_nb(data_in)) {
      for (int i = 0; i < NUM_OF_PAIRS; i++) {
        if (i < (data_in.len >> 1)) {
          data_out.prefix_sum[i + 1] =
              reg(data_out.prefix_sum[i] + data_in.data(16 * i + 7, 16 * i));
          data_out.alpha_array[i] = data_in.data(16 * i + 15, 16 * i + 8);
        } else {
          data_out.prefix_sum[i + 1] = reg(data_out.prefix_sum[i]);
          data_out.alpha_array[i] = 0;
        }
      }
      data_out.eop = data_in.eop;
      data_out.len = (data_in.len >> 1);
      app_rle_expand_input_data.write(data_out);
    }
  }
}
