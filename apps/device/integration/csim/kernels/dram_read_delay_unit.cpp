#ifndef DRAM_READ_DELAY_UNIT_CPP_
#define DRAM_READ_DELAY_UNIT_CPP_

#include <insider_kernel.h>

void dram_read_delay_unit(
    ST_Queue<unsigned int> &device_read_delay_cycle_cnts,
    ST_Queue<Dram_Read_Req_With_Time>
        &before_delay_unified_dram_read_req_with_time,
    ST_Queue<Dram_Read_Req> &after_delay_unified_dram_read_req) {
  unsigned long long time = 0;
  unsigned long long delay_cycle_cnt = 0;
  Dram_Read_Req_With_Time read_req_with_time;
  bool valid_read_req_with_time = false;
  Dram_Write_Req_Apply_With_Time write_req_apply_with_time;
  bool valid_write_req_apply_with_time = false;

  while (1) {
#pragma HLS pipeline
    unsigned int new_delay_cycle_cnt;
    if (device_read_delay_cycle_cnts.read_nb(new_delay_cycle_cnt)) {
      delay_cycle_cnt = new_delay_cycle_cnt;
    }

    if (valid_read_req_with_time ||
        (valid_read_req_with_time =
             before_delay_unified_dram_read_req_with_time.read_nb(
                 read_req_with_time))) {
      if (read_req_with_time.time + delay_cycle_cnt <= time) {
        if (after_delay_unified_dram_read_req.write_nb(
                read_req_with_time.req)) {
          valid_read_req_with_time = false;
        }
      }
    }
    time++;
  }
}

#endif
