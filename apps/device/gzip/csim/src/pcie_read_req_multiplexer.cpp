#ifndef PCIE_READ_REQ_MULTIPLEXER_CPP_
#define PCIE_READ_REQ_MULTIPLEXER_CPP_

#include <insider_kernel.h>

void pcie_read_req_multiplexer(ST_Queue<PCIe_Read_Req> &pcie_read_req,
                               ST_Queue<PCIe_Read_Req> &device_pcie_read_req,
                               ST_Queue<PCIe_Read_Req> &host_pcie_read_req,
                               ST_Queue<bool> &pcie_read_mux_context) {

  while (1) {
#pragma HLS pipeline
    bool has_read_req = false;
    PCIe_Read_Req read_req;
    bool context;
    if (device_pcie_read_req.read_nb(read_req)) {
      has_read_req = true;
      context = 0;
    } else if(host_pcie_read_req.read_nb(read_req)) {
      has_read_req = true;
      context = 1;
    }

    if (has_read_req) {
      pcie_read_mux_context.write(context);
      pcie_read_req.write(read_req);
    }
  }
}

#endif
