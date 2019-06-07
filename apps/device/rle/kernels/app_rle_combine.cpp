#include <insider_kernel.h>

#include "structure.h"

#ifdef CSIM
#define reg(x) (x)
#else
#include <hls/utils/x_hls_utils.h>
#endif
#define LEN (64)

void app_rle_combine(ST_Queue<APP_Data> &app_rle_combine_input_data,
                     ST_Queue<APP_Data> &app_output_data) {
  char buffer[2][64];
#pragma HLS array_partition variable = buffer complete dim = 0
  char data_tmp[64];
#pragma HLS array_partition variable = data_tmp complete dim = 0
  unsigned char remain_data_num = 0;
  bool last_write = false;
  while (1) {
#pragma HLS pipeline
    APP_Data data_in;
    APP_Data data_out;
    if (app_rle_combine_input_data.read_nb(data_in)) {

      for (unsigned char i = 0; i < 64; i++) {
#pragma HLS unroll
        data_tmp[i] = data_in.data(8 * i + 7, 8 * i);
      }

      unsigned char total_num = remain_data_num + data_in.len;
      if (total_num < 64) {
        for (unsigned char i = 0; i < 64; i++) {
#pragma HLS unroll
          unsigned char uplimit = remain_data_num + data_in.len;
          unsigned char idx = i - remain_data_num;
          if (i >= remain_data_num && i < uplimit)
            buffer[0][i] = data_tmp[idx];
        }
        remain_data_num += data_in.len;
        if (data_in.eop)
          last_write = true;

      } else {
        unsigned char used = 64 - remain_data_num;
        for (unsigned char i = 0; i < 64; i++) {
#pragma HLS unroll
          unsigned char idx1 = i - remain_data_num;
          if (i < remain_data_num)
            buffer[1][i] = buffer[0][i];
          else
            buffer[1][i] = data_tmp[idx1];

          unsigned char idx2 = used + i;
          if (idx2 < data_in.len)
            buffer[0][i] = data_tmp[idx2];
        }

        remain_data_num = data_in.len - used;
        for (unsigned char i = 0; i < 64; i++) {
#pragma HLS unroll
          data_out.data(8 * i + 7, 8 * i) = buffer[1][i];
        }
        if (data_in.eop && remain_data_num > 0) {
          data_out.eop = false;
          last_write = true;
        } else {
          data_out.eop = data_in.eop;
        }
        data_out.len = 64;
        app_output_data.write(data_out);
      }
    } else if (last_write) {
      last_write = false;
      for (unsigned char i = 0; i < 64; i++) {
#pragma HLS unroll
        data_out.data(8 * i + 7, 8 * i) = buffer[0][i];
      }
      data_out.eop = true;
      data_out.len = remain_data_num;
      app_output_data.write(data_out);
    }
  }
}
