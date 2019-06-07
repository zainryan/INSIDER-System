#ifndef APP_INTG_MAT_RDC_H_
#define APP_INTG_MAT_RDC_H_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_intg_mat_rdc(
    ST_Queue<APP_Match> &app_intg_mat_rdc_input,
    ST_Queue<APP_Ver_Record> &app_intg_verifier_input_record) {
  while (1) {
#pragma HLS pipeline
    APP_Match match_in;
    APP_Ver_Record record_out;
    if (app_intg_mat_rdc_input.read_nb(match_in)) {
      bool reduced_match = 0;
      for (int i = 0; i < 32; i++) {
#pragma HLS unroll
        reduced_match = reduced_match | match_in.match[i];
      }
      if (reduced_match) {
        for (int i = 0; i < 64; i++) {
          record_out.record[i] = match_in.record[i];
        }
        record_out.eop = match_in.eop;
        record_out.valid = true;
        app_intg_verifier_input_record.write(record_out);

      } else if (match_in.eop) {
        record_out.eop = true;
        record_out.valid = false;
        app_intg_verifier_input_record.write(record_out);
      }
    }
  }
}
#endif
