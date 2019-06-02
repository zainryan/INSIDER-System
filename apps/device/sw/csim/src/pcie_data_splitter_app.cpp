#ifndef PCIE_DATA_SPLITTER_APP_CPP_
#define PCIE_DATA_SPLITTER_APP_CPP_

#include <insider_kernel.h>

/*
  split the contiguous data into small chunks of PCIE_WRITE_BATCH_NUM size
*/
void pcie_data_splitter_app(ST_Queue<APP_Data> &app_output_data,
                            ST_Queue<APP_Data> &app_output_data_splitted,
                            ST_Queue<APP_Data_Meta> &app_output_data_meta,
                            ST_Queue<bool> &reset_pcie_data_splitter_app) {
  unsigned int num = 0;
  bool reset = false;
  unsigned int reset_cnt = 0;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset || (reset = reset_pcie_data_splitter_app.read_nb(dummy))) {
      num = 0;
      APP_Data dummy0;
      app_output_data.read_nb(dummy0);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      APP_Data app_data;
      if (app_output_data.read_nb(app_data)) {
        if (!app_data.eop) {
          num += DATA_BUS_WIDTH;
        } else {
          num += app_data.len;
        }
        if (num == (PCIE_WRITE_BATCH_NUM << DATA_BUS_WIDTH_LOG2) ||
            app_data.eop) {
          APP_Data_Meta app_data_meta;
          app_data_meta.num = num;
          app_data_meta.eop = app_data.eop;
          app_output_data_meta.write(app_data_meta);
          num = 0;
        }
        app_output_data_splitted.write(app_data);
      }
    }
  }
}

#endif
