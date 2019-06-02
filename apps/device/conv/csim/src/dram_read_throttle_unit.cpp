#ifndef DRAM_READ_THROTTLE_UNIT_CPP_
#define DRAM_READ_THROTTLE_UNIT_CPP_

#include <insider_kernel.h>

void dram_read_throttle_unit(
    ST_Queue<unsigned int> &drive_read_throttle_params,
    ST_Queue<Dram_Read_Resp> &before_throttle_unified_dram_read_resp,
    ST_Queue<Dram_Read_Resp> &after_throttle_unified_dram_read_resp) {
  unsigned int throttle_threshold = 0;
  unsigned int throttle_cnt = 0;

  bool read_resp_stall = false;
  ap_uint<MAX_THROTTLE_CONTIGUOUS_LOG2 + 1> read_resp_contiguous_times = 0;
  int read_resp_stall_times = 0;
  Dram_Read_Resp read_resp;
  bool valid_read_resp = false;

  while (1) {
#pragma HLS pipeline
    unsigned int new_param;
    if (drive_read_throttle_params.read_nb(new_param)) {
      throttle_threshold = new_param >> 16;
      throttle_cnt = new_param & 0xFFFF;
      read_resp_stall = false;
      read_resp_contiguous_times = read_resp_stall_times = 0;
    }

    if (!valid_read_resp) {
      if (read_resp_stall) {
        read_resp_stall_times++;
        if (read_resp_stall_times == throttle_cnt) {
          read_resp_stall = false;
          read_resp_stall_times = 0;
        }
      } else {
        if (before_throttle_unified_dram_read_resp.read_nb(read_resp)) {
          valid_read_resp = true;
          read_resp_contiguous_times++;
          if (throttle_threshold != 0 && throttle_cnt != 0 &&
              read_resp_contiguous_times == throttle_threshold) {
            read_resp_stall = true;
            read_resp_contiguous_times = 0;
          }
        } else {
          read_resp_contiguous_times = 0;
        }
      }
    }

    if (valid_read_resp) {
      if (after_throttle_unified_dram_read_resp.write_nb(read_resp)) {
        valid_read_resp = false;
      }
    }
  }
}

#endif
