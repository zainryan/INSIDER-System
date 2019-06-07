#include <insider_kernel.h>

#include "structure.h"

#ifdef CSIM
#define reg(x) (x)
#else
#include <hls/utils/x_hls_utils.h>
#endif
#define LEN (64)

void app_rle_expand(ST_Queue<bool> &reset_app_rle_expand,
                    ST_Queue<APP_Expand_Data> &app_rle_expand_input_data,
                    ST_Queue<APP_Data> &app_rle_combine_input_data) {
  struct APP_Expand_Data data_in;
  ;
  struct APP_Data data_out;
  ;
  ;
#pragma HLS array_partition variable = data_in.prefix_sum complete dim = 0
#pragma HLS array_partition variable = data_in.alpha_array complete dim = 0

  _Bool data_valid = false;
  ;
  ;
  unsigned short has_expand = 0;
  ;
  ;
  ; // >= 0

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_rle_expand.read_nb(dummy))) {
      data_in = APP_Expand_Data();
      data_out = APP_Data();
      data_valid = false;
      has_expand = 0;
      struct APP_Expand_Data dummy0;
      app_rle_expand_input_data.read_nb(dummy0);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      if (!(data_valid) && app_rle_expand_input_data.read_nb(data_in)) {
        data_valid = true;
      }

      if (data_valid) {
        int j;
        for (int i = 0; i < LEN; i++) {
#pragma HLS unroll
          for (j = 1; j <= NUM_OF_PAIRS; j++) {
            if (has_expand + i < data_in.prefix_sum[j]) {
              break;
            }
          }
          data_out.data(8 * i + 7, 8 * i) = data_in.alpha_array[j - 1];
        }
        int remain_len = data_in.prefix_sum[NUM_OF_PAIRS] - has_expand;
        if (j > data_in.len) {
          data_out.eop = data_in.eop;
          data_out.len = remain_len;
          has_expand = 0;
          data_valid = false;
        } else if (remain_len <= LEN) {
          data_out.eop = data_in.eop;
          data_out.len = remain_len;
          has_expand = 0;
          data_valid = false;
        } else {
          data_out.len = LEN;
          has_expand += LEN;
          data_out.eop = false;
        }
        app_rle_combine_input_data.write(data_out);
      }
    }
  }
}
