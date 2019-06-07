#ifndef PCIE_READ_RESP_PASSER_CPP_
#define PCIE_READ_RESP_PASSER_CPP_

#include <insider_kernel.h>

void pcie_read_resp_passer(
    ST_Queue<PCIe_Read_Resp> &pcie_read_resp,
    ST_Queue<PCIe_Read_Resp> &before_throttle_pcie_read_resp) {
  while (1) {
#pragma HLS pipeline
    PCIe_Read_Resp read_resp;
    if (pcie_read_resp.read_nb(read_resp)) {
      before_throttle_pcie_read_resp.write(read_resp);
    }
  }
}

#endif
