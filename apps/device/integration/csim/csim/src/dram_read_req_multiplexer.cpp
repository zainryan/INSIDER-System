#ifndef DRAM_MULTIPLEXER_CPP_
#define DRAM_MULTIPLEXER_CPP_

#include <insider_kernel.h>

void dram_read_req_multiplexer(
    ST_Queue<Dram_Read_Req> &host_dram_read_req,
    ST_Queue<Dram_Read_Req> &device_dram_read_req,
    ST_Queue<Dram_Read_Req> &before_delay_unified_dram_read_req,
    ST_Queue<bool> &dram_read_context_write) {
  while (1) {
#pragma HLS pipeline
    Dram_Read_Req req;
    if (host_dram_read_req.read_nb(req)) {
      dram_read_context_write.write(HOST_READ_REQ);
      before_delay_unified_dram_read_req.write(req);
    } else if (device_dram_read_req.read_nb(req)) {
      dram_read_context_write.write(DEVICE_READ_REQ);
      before_delay_unified_dram_read_req.write(req);
    }
  }
}

#endif
