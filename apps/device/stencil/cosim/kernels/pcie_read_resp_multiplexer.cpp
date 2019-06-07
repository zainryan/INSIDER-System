#ifndef PCIE_READ_RESP_MULTIPLEXER_CPP_
#define PCIE_READ_RESP_MULTIPLEXER_CPP_

#include <insider_kernel.h>

void pcie_read_resp_multiplexer(
    ST_Queue<PCIe_Read_Resp> &pcie_read_resp,
    ST_Queue<PCIe_Read_Resp> &device_pcie_read_resp,
    ST_Queue<PCIe_Read_Resp> &host_pcie_read_resp,
    ST_Queue<Dram_Read_Resp> &cosim_dramA_read_resp,
    ST_Queue<Dram_Read_Resp> &cosim_dramB_read_resp,
    ST_Queue<Dram_Read_Resp> &cosim_dramC_read_resp,
    ST_Queue<Dram_Read_Resp> &cosim_dramD_read_resp,
    ST_Queue<unsigned char> &pcie_read_mux_context) {
  bool has_context = false;
  unsigned char context;
  while (1) {
#pragma HLS pipeline
    if (has_context || (has_context = pcie_read_mux_context.read_nb(context))) {
      PCIe_Read_Resp read_resp;
      if (pcie_read_resp.read_nb(read_resp)) {
        if (context == 0) {
          device_pcie_read_resp.write(read_resp);
        } else if (context == 1) {
          host_pcie_read_resp.write(read_resp);
        } else {
          Dram_Read_Resp dram_read_resp;
          if (context == 2) {
            dram_read_resp.last = read_resp.last;
            dram_read_resp.data = read_resp.data;
            cosim_dramA_read_resp.write(dram_read_resp);
          } else if (context == 3) {
            dram_read_resp.last = read_resp.last;
            dram_read_resp.data = read_resp.data;
            cosim_dramB_read_resp.write(dram_read_resp);
          } else if (context == 4) {
            dram_read_resp.last = read_resp.last;
            dram_read_resp.data = read_resp.data;
            cosim_dramC_read_resp.write(dram_read_resp);
          } else if (context == 5) {
            dram_read_resp.last = read_resp.last;
            dram_read_resp.data = read_resp.data;
            cosim_dramD_read_resp.write(dram_read_resp);
          }
        }
        if (read_resp.last) {
          has_context = false;
        }
      }
    }
  }
}

#endif
