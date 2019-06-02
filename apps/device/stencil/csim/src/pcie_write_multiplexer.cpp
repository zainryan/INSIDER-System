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
    ST_Queue<PCIe_Write_Req_Data> &read_mode_device_pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &read_mode_device_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &write_mode_device_pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &write_mode_device_pcie_write_req_apply) {
  // state = 0: no outstanding pcie write transaction
  // state = 1: outstanding req from host-side data_pcie interface
  // state = 2: outstanding req from host-side rdcmd_fin_pcie interface
  // state = 3: outstanding req from host-side wrcmd_fin_pcie interface
  // state = 4: outstanding req from read-mode device-side interface
  // state = 5: outstanding req from write-mode device-side interface
  // state = 6: waiting to read host_rdcmd_fin_pcie_write_req_apply
  unsigned char state = 0;
  unsigned char inflight_device_req_apply_num = 0;

  while (1) {
#pragma HLS pipeline
    PCIe_Write_Req_Data write_data;
    if (state == 0) {
      PCIe_Write_Req_Apply req_apply;
      // They have different priorities.
      if (host_wrcmd_fin_pcie_write_req_apply.read_nb(req_apply)) {
        state = 3;
        pcie_write_req_apply.write(req_apply);
      } else if (host_data_pcie_write_req_apply.read_nb(req_apply)) {
        state = 1;
        pcie_write_req_apply.write(req_apply);
      } else if (read_mode_device_pcie_write_req_apply.read_nb(req_apply)) {
        state = 4;
        pcie_write_req_apply.write(req_apply);
        inflight_device_req_apply_num = req_apply.num;
      } else if (write_mode_device_pcie_write_req_apply.read_nb(req_apply)) {
        state = 5;
        pcie_write_req_apply.write(req_apply);
        inflight_device_req_apply_num = req_apply.num;
      }
    } else if (state == 6) {
      PCIe_Write_Req_Apply req_apply;
      if (host_rdcmd_fin_pcie_write_req_apply.read_nb(req_apply)) {
        state = 2;
        pcie_write_req_apply.write(req_apply);
      }
    }

    if (state == 1) {
      if (host_data_pcie_write_req_data.read_nb(write_data)) {
        pcie_write_req_data.write(write_data);
        if (write_data.last) {
          state = 6;
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
      if (read_mode_device_pcie_write_req_data.read_nb(write_data)) {
        if (inflight_device_req_apply_num == 1) {
          state = 0;
          write_data.last = true;
        }
        inflight_device_req_apply_num--;
        pcie_write_req_data.write(write_data);
      }
    } else if (state == 5) {
      if (write_mode_device_pcie_write_req_data.read_nb(write_data)) {
        if (inflight_device_req_apply_num == 1) {
          state = 0;
          write_data.last = true;
        }
        inflight_device_req_apply_num--;
        pcie_write_req_data.write(write_data);
      }
    }
  }
}

#endif
