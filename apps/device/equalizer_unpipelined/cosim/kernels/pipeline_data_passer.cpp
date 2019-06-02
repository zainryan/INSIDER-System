#ifndef PIPELINE_DATA_PASSER_CPP_
#define PIPELINE_DATA_PASSER_CPP_

#include <insider_kernel.h>

void pipeline_data_passer(
    ST_Queue<Request> &rd_reqs_pipe0_write,
    ST_Queue<Request> &wr_reqs_pipe0_write,
    ST_Queue<Request> &rd_reqs_pipe1_read,
    ST_Queue<Request> &wr_reqs_pipe1_read,
    ST_Queue<Request> &rd_reqs_pipe1_write,
    ST_Queue<Request> &wr_reqs_pipe1_write,
    ST_Queue<Request> &rd_reqs_pipe2_read,
    ST_Queue<Request> &wr_reqs_pipe2_read,
    ST_Queue<bool> &rd_data_valid_pipe1_write,
    ST_Queue<bool> &wr_data_valid_pipe1_write,
    ST_Queue<bool> &rd_data_valid_pipe2_read,
    ST_Queue<bool> &wr_data_valid_pipe2_read,
    ST_Queue<unsigned long long> &rd_kbuf_addr_pipe0_write,
    ST_Queue<unsigned long long> &wr_kbuf_addr_pipe0_write,
    ST_Queue<unsigned long long> &rd_kbuf_addr_pipe2_read,
    ST_Queue<unsigned long long> &wr_kbuf_addr_pipe2_read) {
  bool valid_rd_req0 = false;
  Request data_rd_req0;
  bool valid_wr_req0 = false;
  Request data_wr_req0;

  bool valid_rd_req1 = false;
  Request data_rd_req1;
  bool valid_wr_req1 = false;
  Request data_wr_req1;

  bool valid_rd_valid = false;
  bool data_rd_valid = false;
  bool valid_wr_valid = false;
  bool data_wr_valid = false;

  bool valid_rd_addr = false;
  unsigned long long data_rd_addr;
  bool valid_wr_addr = false;
  unsigned long long data_wr_addr;
  while (1) {
#pragma HLS pipeline
    if (valid_rd_req0 ||
        (valid_rd_req0 = rd_reqs_pipe0_write.read_nb(data_rd_req0))) {
      if (rd_reqs_pipe1_read.write_nb(data_rd_req0)) {
        valid_rd_req0 = false;
      }
    }
    if (valid_wr_req0 ||
        (valid_wr_req0 = wr_reqs_pipe0_write.read_nb(data_wr_req0))) {
      if (wr_reqs_pipe1_read.write_nb(data_wr_req0)) {
        valid_wr_req0 = false;
      }
    }
    if (valid_rd_req1 ||
        (valid_rd_req1 = rd_reqs_pipe1_write.read_nb(data_rd_req1))) {
      if (rd_reqs_pipe2_read.write_nb(data_rd_req1)) {
        valid_rd_req1 = false;
      }
    }
    if (valid_wr_req1 ||
        (valid_wr_req1 = wr_reqs_pipe1_write.read_nb(data_wr_req1))) {
      if (wr_reqs_pipe2_read.write_nb(data_wr_req1)) {
        valid_wr_req1 = false;
      }
    }
    if (valid_rd_valid ||
        (valid_rd_valid = rd_data_valid_pipe1_write.read_nb(data_rd_valid))) {
      if (rd_data_valid_pipe2_read.write_nb(data_rd_valid)) {
        valid_rd_valid = false;
      }
    }
    if (valid_wr_valid ||
        (valid_wr_valid = wr_data_valid_pipe1_write.read_nb(data_wr_valid))) {
      if (wr_data_valid_pipe2_read.write_nb(data_wr_valid)) {
        valid_wr_valid = false;
      }
    }
    if (valid_rd_addr ||
        (valid_rd_addr = rd_kbuf_addr_pipe0_write.read_nb(data_rd_addr))) {
      if (rd_kbuf_addr_pipe2_read.write_nb(data_rd_addr)) {
        valid_rd_addr = false;
      }
    }
    if (valid_wr_addr ||
        (valid_wr_addr = wr_kbuf_addr_pipe0_write.read_nb(data_wr_addr))) {
      if (wr_kbuf_addr_pipe2_read.write_nb(data_wr_addr)) {
        valid_wr_addr = false;
      }
    }
  }
}

#endif
