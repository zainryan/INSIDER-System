#ifndef PIPE1_DATA_HANDLER_CPP_
#define PIPE1_DATA_HANDLER_CPP_

#include <insider_kernel.h>

void pipe1_data_handler(
    ST_Queue<Request> &rd_reqs_pipe1_read,
    ST_Queue<Request> &wr_reqs_pipe1_read,
    ST_Queue<Dram_Read_Resp> &host_dram_read_resp,
    ST_Queue<PCIe_Read_Resp> &pcie_read_resp,
    ST_Queue<Request> &rd_reqs_pipe1_write,
    ST_Queue<Request> &wr_reqs_pipe1_write,
    ST_Queue<bool> &rd_data_valid_pipe1_write,
    ST_Queue<bool> &wr_data_valid_pipe1_write, ST_Queue<Data> &data_pipe2,
    ST_Queue<Dram_Write_Req_Data> &host_dram_write_req_data) {
  Request wr_pipe1_req;
  bool has_wr_req = false;
  Request rd_pipe1_req;
  bool has_rd_req = false;
  unsigned int wr_req_cnt = 0;

  while (1) {
#pragma HLS pipeline

    if (has_rd_req || (has_rd_req = rd_reqs_pipe1_read.read_nb(rd_pipe1_req))) {
      Dram_Read_Resp read_resp;
      if (host_dram_read_resp.read_nb(read_resp)) {
        Data data;
        data.last = read_resp.last;
        data.data = read_resp.data;
        data_pipe2.write(data);
        if (read_resp.last) {
          rd_reqs_pipe1_write.write(rd_pipe1_req);
          has_rd_req = false;
          rd_data_valid_pipe1_write.write(true);
        }
      }
    }

    if (has_wr_req || (has_wr_req = wr_reqs_pipe1_read.read_nb(wr_pipe1_req))) {
      if (wr_req_cnt) {
        wr_req_cnt--;
        wr_reqs_pipe1_write.write(wr_pipe1_req);
        has_wr_req = false;
        wr_data_valid_pipe1_write.write(true);
      }
    }

    PCIe_Read_Resp read_resp;
    if (pcie_read_resp.read_nb(read_resp)) {
      Dram_Write_Req_Data data;
      data.last = read_resp.last;
      data.data = read_resp.data;
      host_dram_write_req_data.write(data);
      if (read_resp.last) {
        wr_req_cnt++;
      }
    }
  }
}

#endif
