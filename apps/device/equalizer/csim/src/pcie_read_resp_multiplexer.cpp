#ifndef PCIE_READ_RESP_MULTIPLEXER_CPP_
#define PCIE_READ_RESP_MULTIPLEXER_CPP_

#include <insider_kernel.h>

void pcie_read_resp_multiplexer(ST_Queue<PCIe_Read_Resp> &pcie_read_resp,
                                ST_Queue<PCIe_Read_Resp> &device_pcie_read_resp,
                                ST_Queue<PCIe_Read_Resp> &host_pcie_read_resp,
                                ST_Queue<bool> &pcie_read_mux_context) {
  bool has_context = false;
  bool context;
  while (1) {
#pragma HLS pipeline
    if (has_context || (has_context = pcie_read_mux_context.read_nb(context))) {
      PCIe_Read_Resp read_resp;
      if (pcie_read_resp.read_nb(read_resp)) {
        if (context == 0) {
          device_pcie_read_resp.write(read_resp);
        } else {
          host_pcie_read_resp.write(read_resp);
        }
        if (read_resp.last) {
          has_context = false;
        }
      }
    }
  }
}

#endif
