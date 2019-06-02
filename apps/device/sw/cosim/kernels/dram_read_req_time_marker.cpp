#ifndef DRAM_READ_REQ_TIME_MARKER_CPP_
#define DRAM_READ_REQ_TIME_MARKER_CPP_

#include <insider_kernel.h>

void dram_read_req_time_marker(
    ST_Queue<Dram_Read_Req> &before_delay_host_dram_read_req,
    ST_Queue<Dram_Read_Req_With_Time>
        &before_delay_host_dram_read_req_with_time,
    ST_Queue<Dram_Read_Req> &before_delay_device_dram_read_req,
    ST_Queue<Dram_Read_Req_With_Time>
        &before_delay_device_dram_read_req_with_time) {

  unsigned long long time = 0;

  bool valid_host_read_req = false;
  Dram_Read_Req host_read_req;

  bool valid_device_read_req = false;
  Dram_Read_Req device_read_req;

  while (1) {
#pragma HLS pipeline
    if (valid_host_read_req ||
        (valid_host_read_req =
             before_delay_host_dram_read_req.read_nb(host_read_req))) {
      Dram_Read_Req_With_Time read_req_with_time;
      read_req_with_time.req = host_read_req;
      read_req_with_time.time = time;
      valid_host_read_req = true;
      if (before_delay_host_dram_read_req_with_time.write_nb(
              read_req_with_time)) {
        valid_host_read_req = false;
      }
    }

    if (valid_device_read_req ||
        (valid_device_read_req =
             before_delay_device_dram_read_req.read_nb(device_read_req))) {
      Dram_Read_Req_With_Time read_req_with_time;
      read_req_with_time.req = device_read_req;
      read_req_with_time.time = time;
      valid_device_read_req = true;
      if (before_delay_device_dram_read_req_with_time.write_nb(
              read_req_with_time)) {
        valid_device_read_req = false;
      }
    }

    time++;
  }
}

#endif
