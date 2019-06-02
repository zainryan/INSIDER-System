#ifndef APP_INTG_VER_CPP_
#define APP_INTG_VER_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_intg_verifier(
    ST_Queue<bool> &reset_app_intg_verifier,
    ST_Queue<APP_Ver_Param> &app_intg_verifier_input_param,
    ST_Queue<APP_Ver_Record> &app_intg_verifier_input_record,
    ST_Queue<APP_Reduce_Record32> &app_intg_rdc_input_32to16) {
  _Bool valid_param = false;
  ;
  unsigned int thres;
  ;
  ;
  unsigned char query[32];
  ;
  ;
  ;
#pragma HLS array_partition variable = query complete

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_intg_verifier.read_nb(dummy))) {
      valid_param = false;
      struct APP_Ver_Param dummy0;
      app_intg_verifier_input_param.read_nb(dummy0);
      struct APP_Ver_Record dummy1;
      app_intg_verifier_input_record.read_nb(dummy1);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
#pragma HLS pipeline
      if (!valid_param) {
        APP_Ver_Param param;
        if (app_intg_verifier_input_param.read_nb(param)) {
          for (int i = 0; i < APP_QUERY_LENGTH; i++) {
#pragma HLS unroll
            query[i] = param.query[i];
          }
          thres = param.thres;
          valid_param = true;
        }
      } else {
        APP_Ver_Record record_in;
        APP_Reduce_Record32 record_out;
        if (app_intg_verifier_input_record.read_nb(record_in)) {
          if (record_in.valid == false &&
              record_in.eop == true) { // Avoid useless comparison
            record_out.valid = false;
            record_out.eop = true;
            app_intg_rdc_input_32to16.write(record_out);
          } else {
            for (int i = 0; i < APP_QUERY_LENGTH; i++) {
#pragma HLS unroll
              record_out.overlap[i] = 0;
              for (int j = 0; j < APP_QUERY_LENGTH; j++) {
#pragma HLS unroll
                record_out.overlap[i] +=
                    (unsigned char)(query[i] == record_in.record[j]);
              }
            }
            for (int i = 0; i < 64; i++) {
#pragma HLS unroll
              record_out.record[i] = record_in.record[i];
            }
            record_out.eop = record_in.eop;
            record_out.valid = record_in.valid;
            app_intg_rdc_input_32to16.write(record_out);
          }
        }
      }
    }
  }
}
#endif
