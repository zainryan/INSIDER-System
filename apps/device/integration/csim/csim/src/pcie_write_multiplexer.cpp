#ifndef PCIE_WRITE_MULTIPLEXER_CPP_
#define PCIE_WRITE_MULTIPLEXER_CPP_

#include <insider_kernel.h>

void pcie_write_multiplexer(
    ST_Queue<PCIe_Write_Req_Data> &host_data_pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &host_data_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &host_rdcmd_fin_pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &host_rdcmd_fin_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &host_wrcmd_fin_pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &host_wrcmd_fin_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &device_pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &device_pcie_write_req_apply) {
  // state = 0: no outstanding pcie write transaction
  // state = 1: outstanding req from host-side data_pcie interface
  // state = 2: outstanding req from host-side rdcmd_fin_pcie interface
  // state = 3: outstanding req from host-side wrcmd_fin_pcie interface
  // state = 4: outstanding req from device-side interface
  unsigned char state = 0;
  unsigned char inflight_device_req_apply_num = 0;

  while (1) {
#pragma HLS pipeline
    PCIe_Write_Req_Data write_data;
    if (state == 0) {
      PCIe_Write_Req_Apply req_apply;
      /* fin interface has higher priority */
      if (host_rdcmd_fin_pcie_write_req_apply.read_nb(req_apply)) {
        state = 2;
        pcie_write_req_apply.write(req_apply);
      } else if (host_wrcmd_fin_pcie_write_req_apply.read_nb(req_apply)) {
        state = 3;
        pcie_write_req_apply.write(req_apply);
      } else if (host_data_pcie_write_req_apply.read_nb(req_apply)) {
        state = 1;
        pcie_write_req_apply.write(req_apply);

      } else if (device_pcie_write_req_apply.read_nb(req_apply)) {
        state = 4;
        pcie_write_req_apply.write(req_apply);
        inflight_device_req_apply_num = req_apply.num;
      }
    }

    if (state == 1) {
      if (host_data_pcie_write_req_data.read_nb(write_data)) {
        pcie_write_req_data.write(write_data);
        if (write_data.last) {
          state = 0;
        }
      }
    } else if (state == 2) {
      if (host_rdcmd_fin_pcie_write_req_data.read_nb(write_data)) {
        pcie_write_req_data.write(write_data);
        if (write_data.last) {
          state = 0;
        }
      }
    } else if (state == 3) {
      if (host_wrcmd_fin_pcie_write_req_data.read_nb(write_data)) {
        pcie_write_req_data.write(write_data);
        if (write_data.last) {
          state = 0;
        }
      }
    } else if (state == 4) {
      if (device_pcie_write_req_data.read_nb(write_data)) {
        inflight_device_req_apply_num--;
        if (!inflight_device_req_apply_num) {
          state = 0;
          write_data.last = true;
        }
        pcie_write_req_data.write(write_data);
      }
    }
  }
}

#endif
