#ifndef RESET_PROPAGANDA_CPP_
#define RESET_PROPAGANDA_CPP_

#include <insider_kernel.h>

void reset_propaganda(
    ST_Queue<bool> &reset_app_rle_combine, ST_Queue<bool> &reset_app_rle_expand,
    ST_Queue<bool> &reset_app_rle_prefix_sum, ST_Queue<bool> &reset_sigs,
    ST_Queue<bool> &reset_read_mode_dram_helper_app,
    ST_Queue<bool> &reset_write_mode_dram_helper_app,
    ST_Queue<bool> &reset_read_mode_pcie_helper_app,
    ST_Queue<bool> &reset_write_mode_pcie_helper_app,
    ST_Queue<bool> &reset_pcie_data_splitter_app,
    ST_Queue<bool> &reset_app_output_data_demux,
    ST_Queue<bool> &reset_app_input_data_mux,
    ST_Queue<bool> &reset_write_mode_app_output_data_caching,
    ST_Queue<bool> &reset_app_input_data_merger,
    ST_Queue<bool> &reset_write_mode_pre_merged_app_input_data_forwarder) {
  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset_sigs.read_nb(dummy)) {
      reset_read_mode_dram_helper_app.write(0);
      reset_write_mode_dram_helper_app.write(0);
      reset_read_mode_pcie_helper_app.write(0);
      reset_write_mode_pcie_helper_app.write(0);
      reset_pcie_data_splitter_app.write(0);
      reset_app_output_data_demux.write(0);
      reset_app_input_data_mux.write(0);
      reset_write_mode_app_output_data_caching.write(0);
      reset_app_input_data_merger.write(0);
      reset_write_mode_pre_merged_app_input_data_forwarder.write(0);
      reset_app_rle_combine.write(0);
      reset_app_rle_expand.write(0);
      reset_app_rle_prefix_sum.write(0);
    }
  }
}
#endif