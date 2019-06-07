#ifndef APP_INPUT_DATA_MUX_CPP_
#define APP_INPUT_DATA_MUX_CPP_

#include <insider_kernel.h>

void app_input_data_mux(ST_Queue<APP_Data> &read_mode_app_input_data,
                        ST_Queue<APP_Data> &write_mode_app_input_data,
                        ST_Queue<APP_Data> &app_input_data,
                        ST_Queue<unsigned int> &peek_virt_written_bytes_req,
                        ST_Queue<unsigned int> &peek_virt_written_bytes_resp,
                        ST_Queue<bool> &reset_app_input_data_mux) {
  bool reset = false;
  unsigned int reset_cnt = 0;
  unsigned long long virt_written_bytes = 0;
  bool peek_first_half = true;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset || (reset = reset_app_input_data_mux.read_nb(dummy))) {
      APP_Data dummy_0;
      read_mode_app_input_data.read_nb(dummy_0);
      APP_Data dummy_1;
      write_mode_app_input_data.read_nb(dummy_1);
      unsigned int dummy_2;
      peek_virt_written_bytes_req.read_nb(dummy_2);
      virt_written_bytes = 0;
      peek_first_half = true;
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      APP_Data data;
      if (read_mode_app_input_data.read_nb(data)) {
        app_input_data.write(data);
      } else if (write_mode_app_input_data.read_nb(data)) {
        app_input_data.write(data);
        virt_written_bytes += data.len;
      }

      unsigned int req;
      if (peek_virt_written_bytes_req.read_nb(req)) {
        if (peek_first_half) {
          peek_virt_written_bytes_resp.write(virt_written_bytes >> 32);
        } else {
          peek_virt_written_bytes_resp.write(virt_written_bytes & 0xFFFFFFFF);
        }
        peek_first_half ^= 1;
      }
    }
  }
}

#endif
