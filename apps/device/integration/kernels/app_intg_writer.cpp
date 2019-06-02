#ifndef APP_INTG_WR_CPP_
#define APP_INTG_WR_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_intg_writer(ST_Queue<unsigned int> &app_intg_writer_input_param,
                     ST_Queue<APP_Reduce_Record2> &app_intg_writer_input_record,
                     ST_Queue<APP_Data> &app_intg_output_data) {
  bool valid_param = false;
  unsigned int thres = 0;
  while (1) {
#pragma HLS pipeline
    if (!valid_param) {
      unsigned int param;
      if (app_intg_writer_input_param.read_nb(param)) {
        valid_param = true;
        thres = param;
      }
    } else {
      APP_Reduce_Record2 record_in;
      APP_Data data_out;
      if (app_intg_writer_input_record.read_nb(record_in)) {
        if (record_in.valid == false && record_in.eop == true) {
          data_out.len = 0;
          data_out.eop = true;
          app_intg_output_data.write(data_out);
        } else {
          unsigned int reduced_overlap =
              record_in.overlap[0] + record_in.overlap[1];
          if (reduced_overlap > APP_QUERY_LENGTH - thres) {
            for (int i = 0; i < 64; i++) {
              data_out.data(8 * i + 7, 8 * i) = record_in.record[i];
            }
            data_out.len = 64;
            data_out.eop = record_in.eop;
            app_intg_output_data.write(data_out);
          } else if (record_in.eop) {
            data_out.len = 0;
            data_out.eop = true;
            app_intg_output_data.write(data_out);
          }
        }
      }
    }
  }
}
#endif
