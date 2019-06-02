#ifndef PIPE0_DRAM_DISPATCHER_CPP_
#define PIPE0_DRAM_DISPATCHER_CPP_

#include <insider_kernel.h>

void pipe0_dram_dispatcher(
    ST_Queue<Dram_Read_Req> &after_delay_unified_dram_read_req,
    ST_Queue<Dram_Write_Req_Apply> &after_throttle_unified_dram_write_req_apply,
    ST_Queue<Dram_Read_Req> &dramA_read_req,
    ST_Queue<Dram_Write_Req_Apply> &dramA_write_req_apply,
    ST_Queue<Dram_Read_Req> &dramB_read_req,
    ST_Queue<Dram_Write_Req_Apply> &dramB_write_req_apply,
    ST_Queue<Dram_Read_Req> &dramC_read_req,
    ST_Queue<Dram_Write_Req_Apply> &dramC_write_req_apply,
    ST_Queue<Dram_Read_Req> &dramD_read_req,
    ST_Queue<Dram_Write_Req_Apply> &dramD_write_req_apply,
    ST_Queue<Dram_Dispatcher_Write_Req> &unified_dram_dispatcher_write_context,
    ST_Queue<Dram_Dispatcher_Read_Req> &unified_dram_dispatcher_read_context) {
  while (1) {
#pragma HLS pipeline
    Dram_Read_Req read_req;
    Dram_Read_Req ddr_read_req[4];
    Dram_Dispatcher_Read_Req read_cmd;

    unsigned char read_bank_id;
    unsigned char read_end_bank_id;
    bool has_read_req = after_delay_unified_dram_read_req.read_nb(read_req);

    if (has_read_req) {
      has_read_req = true;
      read_bank_id = (read_req.addr >> 12) & 3;
      unsigned long long read_bank_idx = read_req.addr >> 14;
      unsigned long long dram_start_addr =
          (read_bank_idx << 12) + (read_req.addr & 4095);
      ap_uint<DRAM_ADDR_BUS_WIDTH> end_addr =
          read_req.addr + (read_req.num << DATA_BUS_WIDTH_LOG2) - 1;
      read_end_bank_id = (end_addr >> 12) & 3;
      unsigned long long read_end_bank_idx = end_addr >> 14;
      unsigned long long end_dram_start_addr = read_end_bank_idx << 12;
      if (read_bank_id == read_end_bank_id) {
        ddr_read_req[read_bank_id].addr = dram_start_addr;
        ddr_read_req[read_bank_id].num = read_req.num;
        read_cmd.bank_id = read_bank_id;
        read_cmd.end_bank_id = read_end_bank_id;
        read_cmd.cmd_num = read_req.num;
        unified_dram_dispatcher_read_context.write(read_cmd);
      } else {
        ddr_read_req[read_bank_id].addr = dram_start_addr;
        ddr_read_req[read_bank_id].num =
            (4096 - (read_req.addr & 4095)) >> DATA_BUS_WIDTH_LOG2;

        ddr_read_req[read_end_bank_id].addr = end_dram_start_addr;
        ddr_read_req[read_end_bank_id].num =
            ((end_addr + 1) & 4095) >> DATA_BUS_WIDTH_LOG2;

        read_cmd.bank_id = read_bank_id;
        read_cmd.end_bank_id = read_end_bank_id;
        read_cmd.cmd_num = read_req.num;

        unified_dram_dispatcher_read_context.write(read_cmd);
      }
    }
    if ((read_bank_id == 0 || read_end_bank_id == 0) && has_read_req) {
      dramA_read_req.write(ddr_read_req[0]);
    }
    if ((read_bank_id == 1 || read_end_bank_id == 1) && has_read_req) {
      dramB_read_req.write(ddr_read_req[1]);
    }
    if ((read_bank_id == 2 || read_end_bank_id == 2) && has_read_req) {
      dramC_read_req.write(ddr_read_req[2]);
    }
    if ((read_bank_id == 3 || read_end_bank_id == 3) && has_read_req) {
      dramD_read_req.write(ddr_read_req[3]);
    }

    Dram_Write_Req_Apply write_apply;
    Dram_Write_Req_Apply ddr_write_apply[4];
    Dram_Dispatcher_Write_Req write_cmd;
    unsigned char write_bank_id;
    unsigned char write_end_bank_id;
    bool has_write_req =
        after_throttle_unified_dram_write_req_apply.read_nb(write_apply);

    if (has_write_req) {
      has_write_req = true;
      write_bank_id = (write_apply.addr >> 12) & 3;
      unsigned long long write_bank_idx = write_apply.addr >> 14;
      unsigned long long dram_start_addr =
          (write_bank_idx << 12) + (write_apply.addr & 4095);
      ap_uint<DRAM_ADDR_BUS_WIDTH> end_addr =
          write_apply.addr + (write_apply.num << DATA_BUS_WIDTH_LOG2) - 1;
      write_end_bank_id = (end_addr >> 12) & 3;
      unsigned long long write_end_bank_idx = end_addr >> 14;
      unsigned long long end_dram_start_addr = (write_end_bank_idx << 12);
      unsigned int before_boundry_len;

      if (write_bank_id == write_end_bank_id) {
        before_boundry_len = write_apply.num;
        ddr_write_apply[write_bank_id].addr = dram_start_addr;
        ddr_write_apply[write_bank_id].num = write_apply.num;
        write_cmd.bank_id = write_bank_id;
        write_cmd.end_bank_id = write_end_bank_id;
        write_cmd.before_boundry_num = before_boundry_len;
        write_cmd.cmd_num = write_apply.num;
        unified_dram_dispatcher_write_context.write(write_cmd);
      } else {
        ddr_write_apply[write_bank_id].addr = dram_start_addr;
        ddr_write_apply[write_bank_id].num =
            (4096 - (write_apply.addr & 4095)) >> DATA_BUS_WIDTH_LOG2;
        ddr_write_apply[write_end_bank_id].addr = end_dram_start_addr;
        ddr_write_apply[write_end_bank_id].num =
            ((end_addr + 1) & 4095) >> DATA_BUS_WIDTH_LOG2;
        before_boundry_len =
            (4096 - (write_apply.addr & 4095)) >> DATA_BUS_WIDTH_LOG2;
        write_cmd.bank_id = write_bank_id;
        write_cmd.end_bank_id = write_end_bank_id;
        write_cmd.before_boundry_num = before_boundry_len;
        write_cmd.cmd_num = write_apply.num;
        unified_dram_dispatcher_write_context.write(write_cmd);
      }
    }
    if ((write_bank_id == 0 || write_end_bank_id == 0) && has_write_req) {
      dramA_write_req_apply.write(ddr_write_apply[0]);
    }
    if ((write_bank_id == 1 || write_end_bank_id == 1) && has_write_req) {
      dramB_write_req_apply.write(ddr_write_apply[1]);
    }
    if ((write_bank_id == 2 || write_end_bank_id == 2) && has_write_req) {
      dramC_write_req_apply.write(ddr_write_apply[2]);
    }
    if ((write_bank_id == 3 || write_end_bank_id == 3) && has_write_req) {
      dramD_write_req_apply.write(ddr_write_apply[3]);
    }
  }
}

#endif
