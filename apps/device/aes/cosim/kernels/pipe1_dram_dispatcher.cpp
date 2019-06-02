#ifndef PIPE1_DRAM_DISPATCHER_CPP_
#define PIPE1_DRAM_DISPATCHER_CPP_

#include <insider_kernel.h>

void pipe1_dram_dispatcher(
    ST_Queue<Dram_Read_Resp> &unified_dram_read_resp,
    ST_Queue<Dram_Write_Req_Data> &unified_dram_write_req_data,
    ST_Queue<Dram_Read_Resp> &dramA_read_resp,
    ST_Queue<Dram_Write_Req_Data> &dramA_write_req_data,
    ST_Queue<Dram_Read_Resp> &dramB_read_resp,
    ST_Queue<Dram_Write_Req_Data> &dramB_write_req_data,
    ST_Queue<Dram_Read_Resp> &dramC_read_resp,
    ST_Queue<Dram_Write_Req_Data> &dramC_write_req_data,
    ST_Queue<Dram_Read_Resp> &dramD_read_resp,
    ST_Queue<Dram_Write_Req_Data> &dramD_write_req_data,
    ST_Queue<Dram_Dispatcher_Write_Req> &unified_dram_dispatcher_write_context,
    ST_Queue<Dram_Dispatcher_Read_Req> &unified_dram_dispatcher_read_context,
    ST_Queue<PCIe_Write_Req_Data> &host_wrcmd_fin_pcie_write_req_data,
    ST_Queue<PCIe_Write_Req_Apply> &host_wrcmd_fin_pcie_write_req_apply,
    ST_Queue<unsigned long long> &wrcmd_kbuf_addrs) {
  unsigned int read_bank_id;
  unsigned int write_bank_id;
  unsigned int read_end_bank_id;
  unsigned int write_end_bank_id;

  ap_uint<PHYSICAL_SECTOR_SIZE_LOG2 - DATA_BUS_WIDTH_LOG2 + 1> dram_read_times =
      0;
  ap_uint<PHYSICAL_SECTOR_SIZE_LOG2 - DATA_BUS_WIDTH_LOG2 + 1>
      dram_write_times = 0;

  ap_uint<PHYSICAL_SECTOR_SIZE_LOG2 - DATA_BUS_WIDTH_LOG2 + 1>
      before_boundry_len = 0; // only used for write

  bool read_is_before_boundry = true;
  bool write_is_before_boundry = true;
  bool has_read_req = false;
  bool has_write_req = false;

  ap_uint<PHYSICAL_SECTOR_SIZE_LOG2 - DATA_BUS_WIDTH_LOG2 + 1> write_cmd_num =
      0;
  ap_uint<PHYSICAL_SECTOR_SIZE_LOG2 - DATA_BUS_WIDTH_LOG2 + 1> read_cmd_num = 0;

  while (1) {
#pragma HLS pipeline
    Dram_Read_Resp read_resp;
    Dram_Dispatcher_Read_Req read_cmd;

    if (!has_read_req) {
      if (unified_dram_dispatcher_read_context.read_nb(read_cmd)) {
        read_bank_id = read_cmd.bank_id;
        read_end_bank_id = read_cmd.end_bank_id;
        read_cmd_num = read_cmd.cmd_num;

        read_is_before_boundry = true;
        has_read_req = true;
        read_is_before_boundry = true;
      }
    }
    if (has_read_req) {
      if ((read_bank_id == 0 && read_is_before_boundry == true) ||
          (read_end_bank_id == 0 && read_is_before_boundry == false)) {
        if (dramA_read_resp.read_nb(read_resp)) {
          dram_read_times++;
          if (read_resp.last == true && dram_read_times < read_cmd_num) {
            read_is_before_boundry = false;
            read_resp.last = false;
          } else if (read_resp.last == true &&
                     dram_read_times == read_cmd_num) {
            has_read_req = false;
            read_is_before_boundry = true;
            dram_read_times = 0;
          }
          unified_dram_read_resp.write(read_resp);
        }
      } else if ((read_bank_id == 1 && read_is_before_boundry == true) ||
                 (read_end_bank_id == 1 && read_is_before_boundry == false)) {
        if (dramB_read_resp.read_nb(read_resp)) {
          dram_read_times++;
          if (read_resp.last == true && dram_read_times < read_cmd_num) {
            read_is_before_boundry = false;
            read_resp.last = false;
          } else if (read_resp.last == true &&
                     dram_read_times == read_cmd_num) {
            has_read_req = false;
            read_is_before_boundry = true;
            dram_read_times = 0;
          }
          unified_dram_read_resp.write(read_resp);
        }
      } else if ((read_bank_id == 2 && read_is_before_boundry == true) ||
                 (read_end_bank_id == 2 && read_is_before_boundry == false)) {
        if (dramC_read_resp.read_nb(read_resp)) {
          dram_read_times++;
          if (read_resp.last == true && dram_read_times < read_cmd_num) {
            read_is_before_boundry = false;
            read_resp.last = false;
          } else if (read_resp.last == true &&
                     dram_read_times == read_cmd_num) {
            has_read_req = false;
            read_is_before_boundry = true;
            dram_read_times = 0;
          }
          unified_dram_read_resp.write(read_resp);
        }
      } else if ((read_bank_id == 3 && read_is_before_boundry == true) ||
                 (read_end_bank_id == 3 && read_is_before_boundry == false)) {
        if (dramD_read_resp.read_nb(read_resp)) {
          dram_read_times++;
          if (read_resp.last == true && dram_read_times < read_cmd_num) {
            read_is_before_boundry = false;
            read_resp.last = false;
          } else if (read_resp.last == true &&
                     dram_read_times == read_cmd_num) {
            has_read_req = false;
            read_is_before_boundry = true;
            dram_read_times = 0;
          }
          unified_dram_read_resp.write(read_resp);
        }
      }
    }
    Dram_Write_Req_Data write_data;
    Dram_Dispatcher_Write_Req write_cmd;

    if (!has_write_req) {
      if (unified_dram_dispatcher_write_context.read_nb(write_cmd)) {
        write_bank_id = write_cmd.bank_id;
        write_end_bank_id = write_cmd.end_bank_id;
        before_boundry_len = write_cmd.before_boundry_num;
        write_cmd_num = write_cmd.cmd_num;
        has_write_req = true;
        dram_write_times = 0;
        write_is_before_boundry = true;
      }
    }
    if (has_write_req) {
      if (unified_dram_write_req_data.read_nb(write_data)) {
        if (write_data.last) {
          unsigned long long kbuf_addr;
          wrcmd_kbuf_addrs.read_nb(kbuf_addr);
          PCIe_Write_Req_Apply fin_write_apply;
          fin_write_apply.num = 1;
          fin_write_apply.addr = kbuf_addr + PHYSICAL_SECTOR_SIZE;
          host_wrcmd_fin_pcie_write_req_apply.write(fin_write_apply);
          PCIe_Write_Req_Data fin_write_data;
          fin_write_data.last = true;
          for (int idx = 0; idx < 512; idx += 8) {
#pragma HLS unroll
            fin_write_data.data(idx + 7, idx) = 0xFF;
          }
          host_wrcmd_fin_pcie_write_req_data.write(fin_write_data);
        }

        dram_write_times++;
        if ((write_bank_id == 0 && write_is_before_boundry == true) ||
            (write_end_bank_id == 0 && write_is_before_boundry == false)) {
          if (write_data.last == true && dram_write_times == write_cmd_num) {
            has_write_req = false;
            write_is_before_boundry = true;
            dram_write_times = 0;
          } else if (write_is_before_boundry &&
                     dram_write_times == before_boundry_len) {
            write_data.last = true;
            write_is_before_boundry = false;
          }
          dramA_write_req_data.write(write_data);
        } else if ((write_bank_id == 1 && write_is_before_boundry == true) ||
                   (write_end_bank_id == 1 &&
                    write_is_before_boundry == false)) {
          if (write_data.last == true && dram_write_times == write_cmd_num) {
            has_write_req = false;
            write_is_before_boundry = true;
            dram_write_times = 0;

          } else if (write_is_before_boundry &&
                     dram_write_times == before_boundry_len) {
            write_data.last = true;
            write_is_before_boundry = false;
          }
          dramB_write_req_data.write(write_data);
        } else if ((write_bank_id == 2 && write_is_before_boundry == true) ||
                   (write_end_bank_id == 2 &&
                    write_is_before_boundry == false)) {
          if (write_data.last == true && dram_write_times == write_cmd_num) {
            has_write_req = false;
            write_is_before_boundry = true;
            dram_write_times = 0;
          } else if (write_is_before_boundry &&
                     dram_write_times == before_boundry_len) {
            write_data.last = true;
            write_is_before_boundry = false;
          }
          dramC_write_req_data.write(write_data);
        } else if ((write_bank_id == 3 && write_is_before_boundry == true) ||
                   (write_end_bank_id == 3 &&
                    write_is_before_boundry == false)) {
          if (write_data.last == true && dram_write_times == write_cmd_num) {
            has_write_req = false;
            write_is_before_boundry = true;
            dram_write_times = 0;
          } else if (write_is_before_boundry &&
                     dram_write_times == before_boundry_len) {
            write_data.last = true;
            write_is_before_boundry = false;
          }
          dramD_write_req_data.write(write_data);
        }
      }
    }
  }
}

#endif
