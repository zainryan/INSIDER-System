#ifndef RESET_PROPAGANDA_CPP_
#define RESET_PROPAGANDA_CPP_

#include <insider_kernel.h>

void reset_propaganda(
    ST_Queue<bool> &reset_app_conv_combine_l0_0,
    ST_Queue<bool> &reset_app_conv_combine_l0_1,
    ST_Queue<bool> &reset_app_conv_combine_l0_2,
    ST_Queue<bool> &reset_app_conv_combine_l0_3,
    ST_Queue<bool> &reset_app_conv_combine_l1_0,
    ST_Queue<bool> &reset_app_conv_combine_l1_1,
    ST_Queue<bool> &reset_app_conv_combine_l2_0,
    ST_Queue<bool> &reset_app_conv_dist, ST_Queue<bool> &reset_app_conv_mult_0,
    ST_Queue<bool> &reset_app_conv_mult_10,
    ST_Queue<bool> &reset_app_conv_mult_11,
    ST_Queue<bool> &reset_app_conv_mult_12,
    ST_Queue<bool> &reset_app_conv_mult_13,
    ST_Queue<bool> &reset_app_conv_mult_14,
    ST_Queue<bool> &reset_app_conv_mult_15,
    ST_Queue<bool> &reset_app_conv_mult_1,
    ST_Queue<bool> &reset_app_conv_mult_2,
    ST_Queue<bool> &reset_app_conv_mult_3,
    ST_Queue<bool> &reset_app_conv_mult_4,
    ST_Queue<bool> &reset_app_conv_mult_5,
    ST_Queue<bool> &reset_app_conv_mult_6,
    ST_Queue<bool> &reset_app_conv_mult_7,
    ST_Queue<bool> &reset_app_conv_mult_8,
    ST_Queue<bool> &reset_app_conv_mult_9,
    ST_Queue<bool> &reset_app_conv_mult_dummy,
    ST_Queue<bool> &reset_app_conv_row_reduce_0,
    ST_Queue<bool> &reset_app_conv_row_reduce_1,
    ST_Queue<bool> &reset_app_conv_row_reduce_2,
    ST_Queue<bool> &reset_app_conv_row_reduce_3,
    ST_Queue<bool> &reset_app_conv_row_reduce_4,
    ST_Queue<bool> &reset_app_conv_row_reduce_5,
    ST_Queue<bool> &reset_app_conv_row_reduce_6,
    ST_Queue<bool> &reset_app_conv_row_reduce_7,
    ST_Queue<bool> &reset_app_conv_writer, ST_Queue<bool> &reset_sigs,
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
      reset_app_conv_combine_l0_0.write(0);
      reset_app_conv_combine_l0_1.write(0);
      reset_app_conv_combine_l0_2.write(0);
      reset_app_conv_combine_l0_3.write(0);
      reset_app_conv_combine_l1_0.write(0);
      reset_app_conv_combine_l1_1.write(0);
      reset_app_conv_combine_l2_0.write(0);
      reset_app_conv_dist.write(0);
      reset_app_conv_mult_0.write(0);
      reset_app_conv_mult_10.write(0);
      reset_app_conv_mult_11.write(0);
      reset_app_conv_mult_12.write(0);
      reset_app_conv_mult_13.write(0);
      reset_app_conv_mult_14.write(0);
      reset_app_conv_mult_15.write(0);
      reset_app_conv_mult_1.write(0);
      reset_app_conv_mult_2.write(0);
      reset_app_conv_mult_3.write(0);
      reset_app_conv_mult_4.write(0);
      reset_app_conv_mult_5.write(0);
      reset_app_conv_mult_6.write(0);
      reset_app_conv_mult_7.write(0);
      reset_app_conv_mult_8.write(0);
      reset_app_conv_mult_9.write(0);
      reset_app_conv_mult_dummy.write(0);
      reset_app_conv_row_reduce_0.write(0);
      reset_app_conv_row_reduce_1.write(0);
      reset_app_conv_row_reduce_2.write(0);
      reset_app_conv_row_reduce_3.write(0);
      reset_app_conv_row_reduce_4.write(0);
      reset_app_conv_row_reduce_5.write(0);
      reset_app_conv_row_reduce_6.write(0);
      reset_app_conv_row_reduce_7.write(0);
      reset_app_conv_writer.write(0);
    }
  }
}
#endif