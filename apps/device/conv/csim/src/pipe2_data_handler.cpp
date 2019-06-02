#ifndef PIPE2_DATA_HANDLER_CPP_
#define PIPE2_DATA_HANDLER_CPP_

#include <insider_kernel.h>

void pipe2_data_handler(
    ST_Queue<Request> &rd_reqs_pipe2_read,
    ST_Queue<Request> &wr_reqs_pipe2_read,
    ST_Queue<bool> &rd_data_valid_pipe2_read,
    ST_Queue<bool> &wr_data_valid_pipe2_read,
    ST_Queue<unsigned long long> &rd_kbuf_addr_pipe2_read,
    ST_Queue<unsigned long long> &wr_kbuf_addr_pipe2_read,
    ST_Queue<Data> &data_pipe2,
    ST_Queue<PCIe_Write_Req_Apply> &host_data_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &host_data_pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &host_rdcmd_fin_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &host_rdcmd_fin_pcie_write_req_data,
    ST_Queue<Dram_Write_Req_Apply> &host_dram_write_req_apply,
    ST_Queue<unsigned long long> &wrcmd_kbuf_addrs) {
  Request rd_pipe2_req;
  unsigned long long rd_kbuf_addr;
  bool rd_first = true;
  bool rd_has_reqs = false;
  bool rd_has_data_valid = false;
  bool rd_has_kbuf_addr = false;
  bool rd_reset = false;

  Request wr_pipe2_req;
  unsigned long long wr_kbuf_addr;
  bool wr_first = true;
  bool wr_has_reqs = false;
  bool wr_has_data_valid = false;
  bool wr_has_kbuf_addr = false;
  bool wr_reset = false;

  while (1) {
#pragma HLS pipeline
    if (rd_reset) {
      rd_first = true;
      rd_has_reqs = rd_has_data_valid = rd_has_kbuf_addr = rd_reset = false;
    }

    if (wr_reset) {
      wr_first = true;
      wr_has_reqs = wr_has_data_valid = wr_has_kbuf_addr = wr_reset = false;
    }

    if (!(rd_has_reqs && rd_has_data_valid && rd_has_kbuf_addr)) {
      bool dummy;
      if (!rd_has_reqs) {
        rd_has_reqs = rd_reqs_pipe2_read.read_nb(rd_pipe2_req);
      }
      if (!rd_has_data_valid) {
        rd_has_data_valid = rd_data_valid_pipe2_read.read_nb(dummy);
      }
      if (!rd_has_kbuf_addr) {
        rd_has_kbuf_addr = rd_kbuf_addr_pipe2_read.read_nb(rd_kbuf_addr);
      }
    }

    if (!(wr_has_reqs && wr_has_data_valid && wr_has_kbuf_addr)) {
      bool dummy;
      if (!wr_has_reqs) {
        wr_has_reqs = wr_reqs_pipe2_read.read_nb(wr_pipe2_req);
      }
      if (!wr_has_data_valid) {
        wr_has_data_valid = wr_data_valid_pipe2_read.read_nb(dummy);
      }
      if (!wr_has_kbuf_addr) {
        wr_has_kbuf_addr = wr_kbuf_addr_pipe2_read.read_nb(wr_kbuf_addr);
      }
    }

    if (rd_has_reqs && rd_has_data_valid && rd_has_kbuf_addr) {
      Data data;
      if (data_pipe2.read_nb(data)) {
        if (rd_first) {
          PCIe_Write_Req_Apply write_apply;
          write_apply.addr = rd_kbuf_addr;
          write_apply.num = rd_pipe2_req.sector_num << 3;
          host_data_pcie_write_req_apply.write(write_apply);
          rd_first = false;
        }
        PCIe_Write_Req_Data write_data;
        write_data.last = data.last;
        write_data.data = data.data;
        host_data_pcie_write_req_data.write(write_data);
        if (data.last) {
          rd_reset = true;
          PCIe_Write_Req_Apply fin_write_apply;
          fin_write_apply.num = 1;
          fin_write_apply.addr = rd_kbuf_addr + PHYSICAL_SECTOR_SIZE;
          host_rdcmd_fin_pcie_write_req_apply.write(fin_write_apply);
          PCIe_Write_Req_Data fin_write_data;
          fin_write_data.last = true;
          for (int idx = 0; idx < 512; idx += 8) {
#pragma HLS unroll
            fin_write_data.data(idx + 7, idx) = 0xFF;
          }
          host_rdcmd_fin_pcie_write_req_data.write(fin_write_data);
        }
      }
    }

    if (wr_has_reqs && wr_has_data_valid && wr_has_kbuf_addr) {
      Dram_Write_Req_Apply write_apply;
      write_apply.addr = ((unsigned long long)wr_pipe2_req.sector_off) << 9;
      write_apply.num = wr_pipe2_req.sector_num << 3;
      wrcmd_kbuf_addrs.write(wr_kbuf_addr);
      host_dram_write_req_apply.write(write_apply);
      wr_reset = true;
    }
  }
}

#endif
