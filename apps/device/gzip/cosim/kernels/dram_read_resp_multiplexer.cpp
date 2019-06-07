#ifndef DRAM_READ_RESP_MULTIPLEXER_CPP_
#define DRAM_READ_RESP_MULTIPLEXER_CPP_

#include <insider_kernel.h>

void dram_read_resp_multiplexer(
    ST_Queue<Dram_Read_Resp> &host_dram_read_resp,
    ST_Queue<Dram_Read_Resp> &device_dram_read_resp,
    ST_Queue<bool> &release_device_dram_resp_buf_flits,
    ST_Queue<Dram_Read_Resp> &after_throttle_unified_dram_read_resp,
    ST_Queue<bool> &dram_read_context_read) {
  bool valid_state = 0;
  bool data_state;
  bool valid_read_resp = 0;
  Dram_Read_Resp data_read_resp;

  while (1) {
#pragma HLS pipeline
    if (!valid_state) {
      valid_state = dram_read_context_read.read_nb(data_state);
    }
    if (!valid_read_resp) {
      valid_read_resp =
          after_throttle_unified_dram_read_resp.read_nb(data_read_resp);
    }

    if (valid_state && valid_read_resp) {
      valid_read_resp = false;
      if (data_state == HOST_READ_REQ) {
        host_dram_read_resp.write(data_read_resp);
      } else {
        device_dram_read_resp.write(data_read_resp);
        release_device_dram_resp_buf_flits.write(0);
      }
      if (data_read_resp.last) {
        valid_state = false;
      }
    }
  }
}

#endif
