#ifndef RESET_PROPAGANDA_CPP_
#define RESET_PROPAGANDA_CPP_

#include <insider_kernel.h>

void reset_propaganda(ST_Queue<bool> &reset_app_intg_matcher,
                      ST_Queue<bool> &reset_app_intg_mat_rdc,
                      ST_Queue<bool> &reset_app_intg_rdc_16to8,
                      ST_Queue<bool> &reset_app_intg_rdc_32to16,
                      ST_Queue<bool> &reset_app_intg_rdc_4to2,
                      ST_Queue<bool> &reset_app_intg_rdc_8to4,
                      ST_Queue<bool> &reset_app_intg_verifier,
                      ST_Queue<bool> &reset_app_intg_writer,
                      ST_Queue<bool> &reset_sigs,
                      ST_Queue<bool> &reset_dram_helper_app,
                      ST_Queue<bool> &reset_pcie_helper_app,
                      ST_Queue<bool> &reset_pcie_data_splitter_app) {
  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset_sigs.read_nb(dummy)) {
      reset_dram_helper_app.write(0);
      reset_pcie_helper_app.write(0);
      reset_pcie_data_splitter_app.write(0);

      reset_app_intg_matcher.write(0);
      reset_app_intg_mat_rdc.write(0);
      reset_app_intg_rdc_16to8.write(0);
      reset_app_intg_rdc_32to16.write(0);
      reset_app_intg_rdc_4to2.write(0);
      reset_app_intg_rdc_8to4.write(0);
      reset_app_intg_verifier.write(0);
      reset_app_intg_writer.write(0);
    }
  }
}
#endif