#ifndef PCIE_READ_REQ_MULTIPLEXER_CPP_
#define PCIE_READ_REQ_MULTIPLEXER_CPP_

#include <insider_kernel.h>

void pcie_read_req_multiplexer(ST_Queue<PCIe_Read_Req> &pcie_read_req,
                               ST_Queue<PCIe_Read_Req> &device_pcie_read_req,
                               ST_Queue<PCIe_Read_Req> &host_pcie_read_req,
                               ST_Queue<Dram_Read_Req> &cosim_dramA_read_req,
                               ST_Queue<Dram_Read_Req> &cosim_dramB_read_req,
                               ST_Queue<Dram_Read_Req> &cosim_dramC_read_req,
                               ST_Queue<Dram_Read_Req> &cosim_dramD_read_req,
                               ST_Queue<unsigned char> &pcie_read_mux_context) {
  unsigned char last_context = 2;
  while (1) {
#pragma HLS pipeline
    bool has_read_req = false;
    PCIe_Read_Req read_req;
    unsigned char context;
    if (device_pcie_read_req.read_nb(read_req)) {
      has_read_req = true;
      context = 0;
    } else if (host_pcie_read_req.read_nb(read_req)) {
      has_read_req = true;
      context = 1;
    } else {
      Dram_Read_Req dram_read_req;
      if (last_context == 2) {
        if (cosim_dramB_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 3;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMB_ADDR_OFFSET;
        } else if (cosim_dramC_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 4;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMC_ADDR_OFFSET;
        } else if (cosim_dramD_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 5;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMD_ADDR_OFFSET;
        } else if (cosim_dramA_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 2;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMA_ADDR_OFFSET;
        }
      } else if (last_context == 3) {
        if (cosim_dramC_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 4;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMC_ADDR_OFFSET;
        } else if (cosim_dramD_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 5;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMD_ADDR_OFFSET;
        } else if (cosim_dramA_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 2;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMA_ADDR_OFFSET;
        } else if (cosim_dramB_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 3;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMB_ADDR_OFFSET;
        }
      } else if (last_context == 4) {
        if (cosim_dramD_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 5;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMD_ADDR_OFFSET;
        } else if (cosim_dramA_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 2;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMA_ADDR_OFFSET;
        } else if (cosim_dramB_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 3;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMB_ADDR_OFFSET;
        } else if (cosim_dramC_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 4;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMC_ADDR_OFFSET;
        }
      } else if (last_context == 5) {
        if (cosim_dramA_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 2;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMA_ADDR_OFFSET;
        } else if (cosim_dramB_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 3;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMB_ADDR_OFFSET;
        } else if (cosim_dramC_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 4;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMC_ADDR_OFFSET;
        } else if (cosim_dramD_read_req.read_nb(dram_read_req)) {
          has_read_req = true;
          context = last_context = 5;
          read_req.num = dram_read_req.num;
          read_req.addr = dram_read_req.addr + COSIM_DRAMD_ADDR_OFFSET;
        }
      }
    }

    if (has_read_req) {
      pcie_read_mux_context.write(context);
      pcie_read_req.write(read_req);
    }
  }
}

#endif
