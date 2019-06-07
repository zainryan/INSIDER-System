#ifndef PIPE0_DATA_HANDLER_CPP_
#define PIPE0_DATA_HANDLER_CPP_

#include <insider_kernel.h>

void pipe0_data_handler(
    ST_Queue<unsigned int> &kbuf_addrs, ST_Queue<Request> &reqs_incoming,
    ST_Queue<Dram_Read_Req> &host_dram_read_req,
    ST_Queue<PCIe_Read_Req> &pcie_read_req,
    ST_Queue<Request> &rd_reqs_pipe0_write,
    ST_Queue<Request> &wr_reqs_pipe0_write,
    ST_Queue<unsigned long long> &rd_kbuf_addr_pipe0_write,
    ST_Queue<unsigned long long> &wr_kbuf_addr_pipe0_write) {
  bool start = false;
  unsigned long long tmp_kbuf_addr;
  unsigned long long kbuf_addr_arr[DEVICE_REQ_QUEUE_DEPTH];
  unsigned int kbuf_addr_idx = 0;
  bool kbuf_addr_high_part = 1;
  Request pipe0_req;
  bool has_inflight = false;

  while (1) {
#pragma HLS pipeline
    if (!start) {
      unsigned int kbuf_addr;
      if (kbuf_addrs.read_nb(kbuf_addr)) {
        if (kbuf_addr_high_part) {
          tmp_kbuf_addr = ((unsigned long long)kbuf_addr) << 32;
          kbuf_addr_high_part = 0;
        } else {
          tmp_kbuf_addr |= kbuf_addr;
          kbuf_addr_arr[kbuf_addr_idx] = tmp_kbuf_addr;
          kbuf_addr_high_part = 1;
          kbuf_addr_idx++;
        }
      }
      if (kbuf_addr_idx == DEVICE_REQ_QUEUE_DEPTH) {
        start = true;
      }
    } else {
      if (has_inflight || (has_inflight = reqs_incoming.read_nb(pipe0_req))) {
        if (pipe0_req.rw == READ_CMD) {
          Dram_Read_Req read_req;
          read_req.addr = ((unsigned long long)pipe0_req.sector_off) << 9;
          read_req.num = pipe0_req.sector_num << 3;
          host_dram_read_req.write(read_req);
          rd_kbuf_addr_pipe0_write.write(kbuf_addr_arr[pipe0_req.tag]);
          rd_reqs_pipe0_write.write(pipe0_req);
        } else if (pipe0_req.rw == WRITE_CMD) {
          PCIe_Read_Req read_req;
          read_req.addr = kbuf_addr_arr[pipe0_req.tag];
          read_req.num = pipe0_req.sector_num << 3;
          pcie_read_req.write(read_req);
          wr_kbuf_addr_pipe0_write.write(kbuf_addr_arr[pipe0_req.tag]);
          wr_reqs_pipe0_write.write(pipe0_req);
        }
        has_inflight = false;
      }
    }
  }
}

#endif
