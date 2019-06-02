#ifndef WRITE_MODE_APP_OUTPUT_DATA_CACHING_CPP_
#define WRITE_MODE_APP_OUTPUT_DATA_CACHING_CPP_

#include <insider_kernel.h>

void write_mode_app_output_data_caching(
    ST_Queue<APP_Data> &app_output_data,
    ST_Queue<APP_Data> &cached_app_output_data,
    ST_Queue<Dram_Write_Req_Apply> &cached_device_dram_write_req_apply,
    ST_Queue<bool> &reset_write_mode_app_output_data_caching) {
  unsigned char cached_app_output_data_flits = 0;

  bool reset = false;
  unsigned int reset_cnt = 0;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset ||
        (reset = reset_write_mode_app_output_data_caching.read_nb(dummy))) {
      cached_app_output_data_flits = 0;
      APP_Data dummy_0;
      app_output_data.read_nb(dummy_0);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      APP_Data app_data;
      if (app_output_data.read_nb(app_data)) {
        cached_app_output_data.write(app_data);
        cached_app_output_data_flits++;
        if (cached_app_output_data_flits == DRAM_WRITE_BATCH_NUM ||
            app_data.eop) {
          Dram_Write_Req_Apply write_req_apply;
          // Don't care here. Will be correctly handled at
          // write_mode_dram_helper_app.cpp.
          write_req_apply.addr = 0;
          write_req_apply.num = cached_app_output_data_flits;
          cached_device_dram_write_req_apply.write(write_req_apply);
          cached_app_output_data_flits = 0;
        }
      }
    }
  }
}

#endif
