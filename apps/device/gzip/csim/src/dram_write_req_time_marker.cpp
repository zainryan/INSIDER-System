#ifndef DRAM_WRITE_REQ_TIME_MARKER_CPP_
#define DRAM_WRITE_REQ_TIME_MARKER_CPP_

#include <insider_kernel.h>

void dram_write_req_time_marker(
    ST_Queue<Dram_Write_Req_Data> &host_dram_write_req_data,
    ST_Queue<Dram_Write_Req_Apply> &host_dram_write_req_apply,
    ST_Queue<Dram_Write_Req_Data> &before_delay_dram_write_req_data,
    ST_Queue<Dram_Write_Req_Apply_With_Time>
        &before_delay_dram_write_req_apply_with_time) {
  unsigned long long time = 0;
  bool valid_write_req_apply = false;
  Dram_Write_Req_Apply write_req_apply;
  bool valid_write_req_data = false;
  Dram_Write_Req_Data write_req_data;

  while (1) {
#pragma HLS pipeline
    if (valid_write_req_apply ||
        (valid_write_req_apply =
             host_dram_write_req_apply.read_nb(write_req_apply))) {
      Dram_Write_Req_Apply_With_Time write_req_apply_with_time;
      write_req_apply_with_time.req_apply = write_req_apply;
      write_req_apply_with_time.time = time;
      valid_write_req_apply = true;
      if (before_delay_dram_write_req_apply_with_time.write_nb(
              write_req_apply_with_time)) {
        valid_write_req_apply = false;
      }
    }

    if (valid_write_req_data ||
        (valid_write_req_data =
             host_dram_write_req_data.read_nb(write_req_data))) {
      if (before_delay_dram_write_req_data.write_nb(write_req_data)) {
        valid_write_req_data = false;
      }
    }

    time++;
  }
}

#endif
