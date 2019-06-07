#ifndef DRAM_WRITE_THROTTLE_UNIT_CPP_
#define DRAM_WRITE_THROTTLE_UNIT_CPP_

#include <insider_kernel.h>

void dram_write_throttle_unit(
    ST_Queue<unsigned int> &drive_write_throttle_params,
    ST_Queue<Dram_Write_Req_Data> &before_throttle_unified_dram_write_req_data,
    ST_Queue<Dram_Write_Req_Apply>
        &before_throttle_unified_dram_write_req_apply,
    ST_Queue<Dram_Write_Req_Data> &after_throttle_unified_dram_write_req_data,
    ST_Queue<Dram_Write_Req_Apply>
        &after_throttle_unified_dram_write_req_apply) {
  unsigned int throttle_threshold = 0;
  unsigned int throttle_cnt = 0;

  bool write_req_stall = false;
  ap_uint<MAX_THROTTLE_CONTIGUOUS_LOG2 + 1> write_req_contiguous_times = 0;
  int write_req_stall_times = 0;
  Dram_Write_Req_Data write_req_data;
  bool valid_write_req_data = false;

  while (1) {
#pragma HLS pipeline
    unsigned int new_param;
    if (drive_write_throttle_params.read_nb(new_param)) {
      throttle_threshold = new_param >> 16;
      throttle_cnt = new_param & 0xFFFF;
      write_req_stall = false;
      write_req_contiguous_times = write_req_stall_times = 0;
    }

    Dram_Write_Req_Apply write_req_apply;
    if (before_throttle_unified_dram_write_req_apply.read_nb(write_req_apply)) {
      after_throttle_unified_dram_write_req_apply.write(write_req_apply);
    }

    if (!valid_write_req_data) {
      if (write_req_stall) {
        write_req_stall_times++;
        if (write_req_stall_times == throttle_cnt) {
          write_req_stall = false;
          write_req_stall_times = 0;
        }
      } else {
        if (before_throttle_unified_dram_write_req_data.read_nb(
                write_req_data)) {
          valid_write_req_data = true;
          write_req_contiguous_times++;
          if (throttle_threshold != 0 && throttle_cnt != 0 &&
              write_req_contiguous_times == throttle_threshold) {
            write_req_stall = true;
            write_req_contiguous_times = 0;
          }
        } else {
          write_req_contiguous_times = 0;
        }
      }
    }
    if (valid_write_req_data) {
      if (after_throttle_unified_dram_write_req_data.write_nb(write_req_data)) {
        valid_write_req_data = false;
      }
    }
  }
}

#endif
