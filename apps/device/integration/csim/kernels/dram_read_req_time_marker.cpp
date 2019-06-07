#ifndef DRAM_READ_REQ_TIME_MARKER_CPP_
#define DRAM_READ_REQ_TIME_MARKER_CPP_

#include <insider_kernel.h>

void dram_read_req_time_marker(
    ST_Queue<Dram_Read_Req> &before_delay_unified_dram_read_req,
    ST_Queue<Dram_Read_Req_With_Time>
        &before_delay_unified_dram_read_req_with_time) {
  unsigned long long time = 0;
  bool valid_read_req = false;
  Dram_Read_Req read_req;

  while (1) {
#pragma HLS pipeline
    if (valid_read_req ||
        (valid_read_req =
             before_delay_unified_dram_read_req.read_nb(read_req))) {
      Dram_Read_Req_With_Time read_req_with_time;
      read_req_with_time.req = read_req;
      read_req_with_time.time = time;
      valid_read_req = true;
      if (before_delay_unified_dram_read_req_with_time.write_nb(
              read_req_with_time)) {
        valid_read_req = false;
      }
    }
    time++;
  }
}

#endif
