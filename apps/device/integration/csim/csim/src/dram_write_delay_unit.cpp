#ifndef DRAM_WRITE_DELAY_CPP_
#define DRAM_WRITE_DELAY_CPP_

#include <insider_kernel.h>

void dram_write_delay_unit(
    ST_Queue<unsigned int> &device_write_delay_cycle_cnts,
    ST_Queue<Dram_Write_Req_Data> &before_delay_dram_write_req_data,
    ST_Queue<Dram_Write_Req_Apply_With_Time>
        &before_delay_dram_write_req_apply_with_time,
    ST_Queue<Dram_Write_Req_Data> &before_throttle_unified_dram_write_req_data,
    ST_Queue<Dram_Write_Req_Apply>
        &before_throttle_unified_dram_write_req_apply) {
  unsigned long long time = 0;
  unsigned long long delay_cycle_cnt = 0;
  Dram_Write_Req_Apply_With_Time write_req_apply_with_time;
  bool valid_write_req_apply_with_time = false;
  bool valid_write_req_data = false;
  Dram_Write_Req_Data write_req_data;

  while (1) {
#pragma HLS pipeline
    unsigned int new_delay_cycle_cnt;
    if (device_write_delay_cycle_cnts.read_nb(new_delay_cycle_cnt)) {
      delay_cycle_cnt = new_delay_cycle_cnt;
    }

    if (valid_write_req_apply_with_time ||
        (valid_write_req_apply_with_time =
             before_delay_dram_write_req_apply_with_time.read_nb(
                 write_req_apply_with_time))) {
      if (write_req_apply_with_time.time + delay_cycle_cnt <= time) {
        if (before_throttle_unified_dram_write_req_apply.write_nb(
                write_req_apply_with_time.req_apply)) {
          valid_write_req_apply_with_time = false;
        }
      }
    }

    if (valid_write_req_data ||
        (valid_write_req_data =
             before_delay_dram_write_req_data.read_nb(write_req_data))) {
      if (before_throttle_unified_dram_write_req_data.write_nb(
              write_req_data)) {
        valid_write_req_data = false;
      }
    }

    time++;
  }
}

#endif
