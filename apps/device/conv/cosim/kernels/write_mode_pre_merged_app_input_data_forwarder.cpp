#ifndef WRITE_MODE_PRE_MERGED_APP_INPUT_DATA_FORWARDER_CPP_
#define WRITE_MODE_PRE_MERGED_APP_INPUT_DATA_FORWARDER_CPP_

#include <insider_kernel.h>

void process_when_zeroed_context_len(
    PCIe_Read_Resp &read_resp, bool &has_context,
    Write_Mode_PCIe_Read_Req_Context &context,
    ST_Queue<PCIe_Write_Req_Apply> &write_mode_device_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &write_mode_device_pcie_write_req_data) {
#pragma HLS INLINE
  if (read_resp.last) {
    has_context = false;
    if (context.last) {
      PCIe_Write_Req_Apply write_req_apply;
      write_req_apply.addr = context.metadata_addr;
      write_req_apply.num = 1;
      write_mode_device_pcie_write_req_apply.write(write_req_apply);
      PCIe_Write_Req_Data write_req_data;
      write_req_data.data = 0;
      write_mode_device_pcie_write_req_data.write(write_req_data);
    }
  }
}

void process_context(
    ST_Queue<PCIe_Read_Resp> &buffered_device_pcie_read_resp,
    ST_Queue<bool> &release_buffered_device_pcie_read_resp,
    ST_Queue<APP_Data> &pre_merged_app_input_data,
    ST_Queue<PCIe_Write_Req_Apply> &write_mode_device_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &write_mode_device_pcie_write_req_data,
    unsigned long long &has_sent_bytes,
    Write_Mode_PCIe_Read_Req_Context &context, bool &has_context) {
#pragma HLS INLINE
  {
    PCIe_Read_Resp read_resp;
    if (buffered_device_pcie_read_resp.read_nb(read_resp)) {
      release_buffered_device_pcie_read_resp.write(0);
      if (context.len) {
        APP_Data data;
        data.data = read_resp.data;
        if (context.len > DATA_BUS_WIDTH) {
          data.len = DATA_BUS_WIDTH;
          context.len -= DATA_BUS_WIDTH;
        } else {
          data.len = context.len;
          context.len = 0;
          process_when_zeroed_context_len(
              read_resp, has_context, context,
              write_mode_device_pcie_write_req_apply,
              write_mode_device_pcie_write_req_data);
        }
        has_sent_bytes += data.len;
        data.eop = false;
        pre_merged_app_input_data.write(data);
      } else {
        process_when_zeroed_context_len(read_resp, has_context, context,
                                        write_mode_device_pcie_write_req_apply,
                                        write_mode_device_pcie_write_req_data);
      }
    }
  }
}

void write_mode_pre_merged_app_input_data_forwarder(
    ST_Queue<PCIe_Read_Resp> &buffered_device_pcie_read_resp,
    ST_Queue<bool> &release_buffered_device_pcie_read_resp,
    ST_Queue<Write_Mode_PCIe_Read_Req_Context>
        &buffered_device_pcie_read_req_context,
    ST_Queue<APP_Data> &pre_merged_app_input_data,
    ST_Queue<unsigned long long> &app_write_total_len,
    ST_Queue<PCIe_Write_Req_Apply> &write_mode_device_pcie_write_req_apply,
    ST_Queue<PCIe_Write_Req_Data> &write_mode_device_pcie_write_req_data,
    ST_Queue<bool> &reset_write_mode_pre_merged_app_input_data_forwarder) {
  bool has_context = false;
  Write_Mode_PCIe_Read_Req_Context context;

  unsigned long long has_sent_bytes = 0;

  bool has_write_total_len = false;
  unsigned long long write_total_len;

  bool reset = false;
  unsigned int reset_cnt = 0;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset ||
        (reset = reset_write_mode_pre_merged_app_input_data_forwarder.read_nb(
             dummy))) {
      has_context = false;
      has_sent_bytes = 0;
      has_write_total_len = false;
      Write_Mode_PCIe_Read_Req_Context dummy_0;
      buffered_device_pcie_read_req_context.read_nb(dummy_0);
      PCIe_Read_Resp dummy_1;
      buffered_device_pcie_read_resp.read_nb(dummy_1);
      unsigned long long dummy_2;
      app_write_total_len.read_nb(dummy_2);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      if (!has_context) {
        if ((has_context =
                 buffered_device_pcie_read_req_context.read_nb(context))) {
          process_context(buffered_device_pcie_read_resp,
                          release_buffered_device_pcie_read_resp,
                          pre_merged_app_input_data,
                          write_mode_device_pcie_write_req_apply,
                          write_mode_device_pcie_write_req_data, has_sent_bytes,
                          context, has_context);
        } else {
          if (!has_write_total_len) {
            has_write_total_len = app_write_total_len.read_nb(write_total_len);
          } else {
            if (has_sent_bytes == write_total_len) {
              APP_Data data;
              data.eop = true;
              data.len = 0;
              pre_merged_app_input_data.write(data);
              has_write_total_len = false;
            }
          }
        }
      } else {
        process_context(buffered_device_pcie_read_resp,
                        release_buffered_device_pcie_read_resp,
                        pre_merged_app_input_data,
                        write_mode_device_pcie_write_req_apply,
                        write_mode_device_pcie_write_req_data, has_sent_bytes,
                        context, has_context);
      }
    }
  }
}

#endif
