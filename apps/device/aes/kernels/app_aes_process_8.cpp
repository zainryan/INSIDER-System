#include <insider_kernel.h>

#include "structure.h"
#include "subbytes.h"
#include "shiftrows.h"
#include "mixcols.h"
#include "addkeys.h"

void app_aes_process_8 (
    ST_Queue<unsigned int> &app_aes_proc_8_params,
    ST_Queue<unsigned int> &app_aes_end_params,
    ST_Queue<APP_Data> &app_aes_proc_8_input_data,
    ST_Queue<APP_Data> &app_aes_end_input_data) {
  bool valid_param = false;
  unsigned int input_param_num = 0;
  Elem s_table[64][256];
#pragma HLS array_partition variable=s_table complete dim=1
#pragma HLS array_partition variable=s_table cyclic factor=4 dim=2
  Matrix mix_table;
  Matrix key_table;
  while (1) {
#pragma HLS pipeline
    unsigned int param;
    APP_Data data_in;
    APP_Data data_out; 
    if ((!valid_param) && app_aes_proc_8_params.read_nb(param)) {
      // param for subbytes
      if (input_param_num < 64) {
        unsigned char idx = ((input_param_num) & 3);
        unsigned char id = ((input_param_num) >> 2);
        for (int i = 0; i < 64; i++) {
#pragma HLS unroll
          s_table[i][16 * id + 4 * idx + 0] = (Elem) ((param >> 24) & 0xff);
          s_table[i][16 * id + 4 * idx + 1] = (Elem) ((param >> 16) & 0xff);
          s_table[i][16 * id + 4 * idx + 2] = (Elem) ((param >> 8) & 0xff);
          s_table[i][16 * id + 4 * idx + 3] = (Elem) ((param >> 0) & 0xff);
        }
        // param for mixcols 
      } else if (input_param_num < 68) {
        unsigned char idx = input_param_num - 64;
        mix_table(32 * idx + 31, 32 * idx) =  param; 
        // param for addkeys 
      } else if (input_param_num < 72) {
        unsigned char idx = input_param_num - 68;
        key_table(32 * idx + 31, 32 * idx) = param;
        if (input_param_num == 72 - 1) valid_param = true;
      }
      input_param_num++;
      app_aes_end_params.write(param);
    } else if (valid_param && app_aes_proc_8_input_data.read_nb(data_in)) {
      for (int i = 0; i < 4; i++) {
#pragma HLS unroll
        Matrix data = data_in.data(128 * i + 127, 128 * i);  
        subbytes(data, s_table, i);  
        shiftrows(data);  
        mixcols(data, mix_table);  
        addkeys(data, key_table);  
        data_out.data(128 * i + 127, 128 * i) = data;
      }
      data_out.eop = data_in.eop;
      data_out.len = 64;
      app_aes_end_input_data.write(data_out);

    }
  }
}

