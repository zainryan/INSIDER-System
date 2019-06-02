#ifndef APP_INTG_RDC4_CPP_
#define APP_INTG_RDC4_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_intg_rdc_4to2(
    ST_Queue<APP_Reduce_Record4> &app_intg_rdc_input_4to2,
    ST_Queue<APP_Reduce_Record2> &app_intg_writer_input_record) {
  while (1) {
#pragma HLS pipeline
    APP_Reduce_Record4 record_in;
#pragma HLS array_partition variable = record_in.record complete
    APP_Reduce_Record2 record_out;
#pragma HLS array_partition variable = record_out.record complete
    if (app_intg_rdc_input_4to2.read_nb(record_in)) {
      if (record_in.valid) {
        for (int i = 0; i < 8; i += 2) {
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
      app_intg_writer_input_record.write(record_out);
    }
  }
}
#endif
