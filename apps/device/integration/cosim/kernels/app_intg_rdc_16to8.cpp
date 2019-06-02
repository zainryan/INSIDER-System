#ifndef APP_INTG_RDC16_CPP_
#define APP_INTG_RDC16_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_intg_rdc_16to8(ST_Queue<bool> &reset_app_intg_rdc_16to8,
                        ST_Queue<APP_Reduce_Record16> &app_intg_rdc_input_16to8,
                        ST_Queue<APP_Reduce_Record8> &app_intg_rdc_input_8to4) {

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_intg_rdc_16to8.read_nb(dummy))) {
      struct APP_Reduce_Record16 dummy0;
      app_intg_rdc_input_16to8.read_nb(dummy0);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      APP_Reduce_Record16 record_in;
#pragma HLS array_partition variable = record_in.record complete
      APP_Reduce_Record8 record_out;
#pragma HLS array_partition variable = record_out.record complete
      if (app_intg_rdc_input_16to8.read_nb(record_in)) {
        if (record_in.valid) {
          for (int i = 0; i < 16; i += 2) {
#pragma HLS unroll
            record_out.overlap[i / 2] =
                record_in.overlap[i] + record_in.overlap[i + 1];
          }
          for (int i = 0; i < 64; i++) {
#pragma HLS unroll
            record_out.record[i] = record_in.record[i];
          }
        }
        record_out.eop = record_in.eop;
        record_out.valid = record_in.valid;
        app_intg_rdc_input_8to4.write(record_out);
      }
    }
  }
}
#endif
