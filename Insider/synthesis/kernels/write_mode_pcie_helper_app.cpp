#ifndef WRITE_MODE_PCIE_HELPER_APP_CPP_
#define WRITE_MODE_PCIE_HELPER_APP_CPP_

#include <insider_kernel.h>

#ifndef CSIM
#include <hls/utils/x_hls_utils.h>
#else
#define reg(x) (x)
#endif

void write_mode_pcie_helper_app(
    ST_Queue<unsigned int> &app_buf_addrs_chan,
    ST_Queue<unsigned int> &app_commit_write_buf,
    ST_Queue<PCIe_Read_Req> &device_pcie_read_req,
    ST_Queue<PCIe_Read_Resp> &device_pcie_read_resp,
    ST_Queue<Write_Mode_PCIe_Read_Req_Context>
        &buffered_device_pcie_read_req_context,
    ST_Queue<PCIe_Read_Resp> &buffered_device_pcie_read_resp,
    ST_Queue<bool> &release_buffered_device_pcie_read_resp,
    ST_Queue<bool> &reset_pcie_helper_app) {
  int state = 0;
  bool is_addr_high_part = true;
  unsigned char app_buf_addrs_store_idx = 0;
  unsigned char app_buf_addrs_load_idx = 0;
  ap_uint<40> app_buf_addrs[ALLOCATED_BUF_NUM];
#pragma HLS array_partition variable = app_buf_addrs complete
  unsigned long long addr;
  unsigned char app_buf_idx = 0;

  bool has_commit_write_len = false;
  unsigned int commit_write_len = 0;

  ap_uint<40> read_req_addr;

  // The initial space of channel buffered_device_pcie_read_resp should
  // be larger than the initial value of this variable.
  // THe depth of buffered_device_pcie_read_resp should be larger than
  // the PCIe RTT (in cycles).
  ap_uint<10> available_buf_space = 512;

  bool reset = false;
  unsigned int reset_cnt = 0;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset || (reset = reset_pcie_helper_app.read_nb(dummy))) {
      state = 0;
      is_addr_high_part = true;
      app_buf_addrs_store_idx = 0;
      app_buf_addrs_load_idx = 0;
      app_buf_idx = 0;
      available_buf_space = 512;
      has_commit_write_len = false;
      commit_write_len = 0;
      unsigned int dummy_0;
      app_buf_addrs_chan.read_nb(dummy_0);
      unsigned int dummy_1;
      app_commit_write_buf.read_nb(dummy_1);
      PCIe_Read_Resp dummy_2;
      device_pcie_read_resp.read_nb(dummy_2);
      bool dummy_3;
      release_buffered_device_pcie_read_resp.read_nb(dummy_3);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      if (state == 0) {
        unsigned int addr_part;
        if (app_buf_addrs_chan.read_nb(addr_part)) {
          if (is_addr_high_part) {
            addr = addr_part;
          } else {
            addr = (addr << 32) | addr_part;
            app_buf_addrs[app_buf_addrs_store_idx++] = addr;
            if (app_buf_addrs_store_idx == ALLOCATED_BUF_NUM) {
              read_req_addr = app_buf_addrs[0];
              state = 1;
            }
          }
          is_addr_high_part ^= 1;
        }
      } else if (state == 1) {

        if (has_commit_write_len) {
          PCIe_Read_Req read_req;
          read_req.num = PCIE_READ_BATCH_NUM;
          read_req.addr = read_req_addr;
          if (available_buf_space >= PCIE_READ_BATCH_NUM) {
            if (device_pcie_read_req.write_nb(read_req)) {
              available_buf_space -= PCIE_READ_BATCH_NUM;
              read_req_addr += PCIE_READ_BATCH_NUM << DATA_BUS_WIDTH_LOG2;
              ap_uint<40> cur_app_buf_upper_addr =
                  app_buf_addrs[app_buf_idx] + commit_write_len;
              if (read_req_addr >= cur_app_buf_upper_addr) {
                Write_Mode_PCIe_Read_Req_Context context;
                context.len = commit_write_len %
                              ((PCIE_READ_BATCH_NUM) << DATA_BUS_WIDTH_LOG2);
                if (context.len == 0) {
                  context.len = (PCIE_READ_BATCH_NUM) << DATA_BUS_WIDTH_LOG2;
                }
                context.last = true;
                context.metadata_addr =
                    app_buf_addrs[app_buf_idx] + APP_BUF_SIZE;
                buffered_device_pcie_read_req_context.write(context);
                app_buf_idx = (app_buf_idx + 1) % ALLOCATED_BUF_NUM;
                read_req_addr = app_buf_addrs[app_buf_idx];
                has_commit_write_len = false;
              } else {
                Write_Mode_PCIe_Read_Req_Context context;
                context.len = (PCIE_READ_BATCH_NUM) << DATA_BUS_WIDTH_LOG2;
                context.last = false;
                buffered_device_pcie_read_req_context.write(context);
              }
            }
          }
        } else {
          has_commit_write_len = app_commit_write_buf.read_nb(commit_write_len);
        }
      }

      bool dummy;
      available_buf_space +=
          release_buffered_device_pcie_read_resp.read_nb(dummy);

      PCIe_Read_Resp read_resp_0;
      if (device_pcie_read_resp.read_nb(read_resp_0)) {
        buffered_device_pcie_read_resp.write(read_resp_0);
      }
    }
  }
}

#endif
