#ifndef DRAM_MULTIPLEXER_CPP_
#define DRAM_MULTIPLEXER_CPP_

#include <insider_kernel.h>

void dram_read_req_multiplexer(
    ST_Queue<Dram_Read_Req> &host_dram_read_req,
    ST_Queue<Dram_Read_Req> &device_dram_read_req,
    ST_Queue<Dram_Read_Req> &unified_dram_read_req,
    ST_Queue<bool> &release_device_dram_resp_buf_flits,
    ST_Queue<bool> &dram_read_context_write) {

  unsigned char available_device_dram_resp_buf_flits = 2 * DRAM_READ_BATCH_NUM;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    available_device_dram_resp_buf_flits +=
        release_device_dram_resp_buf_flits.read_nb(dummy);

    Dram_Read_Req req;
    if (host_dram_read_req.read_nb(req)) {
      dram_read_context_write.write(HOST_READ_REQ);
      unified_dram_read_req.write(req);
    } else {
      if (available_device_dram_resp_buf_flits >= DRAM_READ_BATCH_NUM) {
        if (device_dram_read_req.read_nb(req)) {
          dram_read_context_write.write(DEVICE_READ_REQ);
          unified_dram_read_req.write(req);
          available_device_dram_resp_buf_flits -= req.num;
        }
      }
    }
  }
}

#endif
