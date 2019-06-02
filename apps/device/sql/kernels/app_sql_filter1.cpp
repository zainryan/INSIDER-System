#ifndef APP_SQL_FILTER1_CPP_
#define APP_SQL_FILTER1_CPP_
#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"
void app_sql_filter1(ST_Queue<Filter_Params> &app_sql_filter1_params,
                     ST_Queue<SQL_Record> &app_sql_filter1_input_records,
                     ST_Queue<SQL_Record> &app_sql_filter1_output_records) {

  bool valid_filter_params = false;
  unsigned char year_upper_thresh;
  unsigned char year_lower_thresh;

  while (1) {
#pragma HLS pipeline
    if (!valid_filter_params) {
      Filter_Params filter_params;
      if (app_sql_filter1_params.read_nb(filter_params)) {
        year_upper_thresh = filter_params.year_upper_thresh;
        year_lower_thresh = filter_params.year_lower_thresh;
        valid_filter_params = true;
      }
    } else {
      SQL_Record record;
      if (app_sql_filter1_input_records.read_nb(record)) {
        unsigned char year =
            (record.year[1] - '0') + (record.year[0] - '0') * 10;
        record.valid = (year >= year_lower_thresh && year <= year_upper_thresh);
        app_sql_filter1_output_records.write(record);
      }
    }
  }
}
#endif
