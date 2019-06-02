#ifndef PCIE_HELPER_APP_CPP_
#define PCIE_HELPER_APP_CPP_

#include <insider_kernel.h>

void pcie_helper_app(
    ST_Queue<unsigned int> &app_buf_addrs_chan,
    ST_Queue<PCIe_Write_Req_Apply> &device_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &device_pcie_write_req_data,
    ST_Queue<APP_Data> &app_output_data_splitted,
    ST_Queue<APP_Data_Meta> &app_output_data_meta, ST_Queue<bool> &app_free_buf,
    ST_Queue<bool> &reset_pcie_helper_app) {
  // state = 0: prepare
  // state = 1: trans_write_apply & data
  // state = 2: trans_write_meta
  int state = 0;
  bool is_addr_high_part = true;
  unsigned int read_app_buf_addr_cnt = 0;
  unsigned long long app_buf_addrs[ALLOCATED_BUF_NUM];
  unsigned long long addr;
  unsigned char app_buf_ptr = 0;
  unsigned char free_buf_num_apply = ALLOCATED_BUF_NUM;
  unsigned char free_buf_num_data = ALLOCATED_BUF_NUM;
  APP_Data_Meta app_data_meta;
  ap_uint<APP_BUF_SIZE_LOG2 + 1> app_buf_issuing_apply_size = 0;
  int app_buf_issuing_data_size = 0;
  bool app_buf_finish_issuing_apply = false;
  bool app_buf_finish_issuing_data = false;
  unsigned long long cur_buf_addr;
  unsigned long long cur_buf_total_size = 0;
  bool reset = false;
  unsigned int reset_cnt = 0;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset || (reset = reset_pcie_helper_app.read_nb(dummy))) {
      state = 0;
      is_addr_high_part = true;
      read_app_buf_addr_cnt = 0;
      app_buf_ptr = 0;
      free_buf_num_apply = free_buf_num_data = ALLOCATED_BUF_NUM;
      app_buf_issuing_apply_size = 0;
      app_buf_issuing_data_size = 0;
      app_buf_finish_issuing_apply = false;
      app_buf_finish_issuing_data = false;
      cur_buf_total_size = 0;
      bool dummy0;
      app_free_buf.read_nb(dummy0);
      unsigned int dummy1;
      app_buf_addrs_chan.read_nb(dummy1);
      APP_Data_Meta dummy2;
      app_output_data_meta.read_nb(dummy2);
      APP_Data dummy3;
      app_output_data_splitted.read_nb(dummy3);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      bool has_free_buf = app_free_buf.read_nb(dummy);
      free_buf_num_apply += has_free_buf;
      free_buf_num_data += has_free_buf;
      if (state == 0) {
        unsigned int addr_part;
        if (app_buf_addrs_chan.read_nb(addr_part)) {
          if (is_addr_high_part) {
            addr = addr_part;
          } else {
            addr = (addr << 32) | addr_part;
            app_buf_addrs[read_app_buf_addr_cnt++] = addr;
            if (read_app_buf_addr_cnt == ALLOCATED_BUF_NUM) {
              state = 1;
            }
          }
          is_addr_high_part ^= 1;
        }
      } else if (state == 1) {
        if (app_buf_finish_issuing_data && app_buf_finish_issuing_apply) {
          state = 2;
        } else {
          if (free_buf_num_apply) {
            if (!app_buf_finish_issuing_apply &&
                app_output_data_meta.read_nb(app_data_meta)) {
              PCIe_Write_Req_Apply write_req_apply;
              cur_buf_addr = app_buf_addrs[app_buf_ptr];
              write_req_apply.addr = cur_buf_addr + app_buf_issuing_apply_size;
              write_req_apply.num =
                  (app_data_meta.num + (DATA_BUS_WIDTH - 1)) >>
                  DATA_BUS_WIDTH_LOG2;
              cur_buf_total_size += app_data_meta.num;
              app_buf_issuing_apply_size += app_data_meta.num;
              if (!(app_buf_issuing_apply_size & (APP_BUF_SIZE - 1)) ||
                  app_data_meta.eop) {
                app_buf_issuing_apply_size = 0;
                app_buf_finish_issuing_apply = true;
                app_buf_ptr = (app_buf_ptr + 1) & (ALLOCATED_BUF_NUM - 1);
                free_buf_num_apply--;
              }
              if (app_data_meta.num) {
                device_pcie_write_req_apply.write(write_req_apply);
              }
            }
          }

          if (free_buf_num_data) {
            APP_Data app_data;
            if (!app_buf_finish_issuing_data &&
                app_output_data_splitted.read_nb(app_data)) {
              app_buf_issuing_data_size += DATA_BUS_WIDTH;
              PCIe_Write_Req_Data write_req_data;
              write_req_data.data = app_data.data;
              if (!(app_buf_issuing_data_size &
                    ((PCIE_WRITE_BATCH_NUM << DATA_BUS_WIDTH_LOG2) - 1)) ||
                  app_data.eop) {
                write_req_data.last = true;
              } else {
                write_req_data.last = false;
              }
              if (!(app_buf_issuing_data_size & (APP_BUF_SIZE - 1)) ||
                  app_data.eop) {
                app_buf_issuing_data_size = 0;
                app_buf_finish_issuing_data = true;
                free_buf_num_data--;
              }
              if (app_data.len) {
                device_pcie_write_req_data.write(write_req_data);
              }
            }
          }
        }
      } else if (state == 2) {
        PCIe_Write_Req_Apply write_req_apply;
        write_req_apply.addr = cur_buf_addr + APP_BUF_SIZE;
        write_req_apply.num = 1;
        device_pcie_write_req_apply.write(write_req_apply);
        PCIe_Write_Req_Data write_req_data;
        for (int idx = 0; idx < 512; idx += 32) {
#pragma HLS unroll
          write_req_data.data(idx + 31, idx) =
              (cur_buf_total_size << 1) | app_data_meta.eop;
        }
        write_req_data.last = true;
        device_pcie_write_req_data.write(write_req_data);
        cur_buf_total_size = 0;
        app_buf_finish_issuing_apply = false;
        app_buf_finish_issuing_data = false;
        state = 1;
      }
    }
  }
}

#endif
