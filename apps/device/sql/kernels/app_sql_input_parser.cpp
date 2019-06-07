#ifndef APP_SQL_INPUT_PARSER_CPP_
#define APP_SQL_INPUT_PARSER_CPP_
#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_sql_input_parser(ST_Queue<APP_Data> &app_input_data,
                          ST_Queue<unsigned int> &app_input_params,
                          ST_Queue<SQL_Record> &app_sql_filter0_input_records,
                          ST_Queue<SQL_Record> &app_sql_filter1_input_records,
                          ST_Queue<Filter_Params> &app_sql_filter0_params,
                          ST_Queue<Filter_Params> &app_sql_filter1_params) {

  bool valid_filter_params = false;
  while (1) {
#pragma HLS pipeline
    if (!valid_filter_params) {
      unsigned int input_param;
      if (app_input_params.read_nb(input_param)) {
        Filter_Params filter_params;
        filter_params.year_upper_thresh = input_param >> 8;
        filter_params.year_lower_thresh = input_param & 0xFF;
        app_sql_filter0_params.write(filter_params);
        app_sql_filter1_params.write(filter_params);
        valid_filter_params = true;
      }
    } else {
      APP_Data input_app_data;
      if (app_input_data.read_nb(input_app_data)) {
        char input_app_data_in_bytes[64];
        for (int i = 0; i < 64; i++) {
          input_app_data_in_bytes[i] = input_app_data.data(i * 8 + 7, i * 8);
        }
        SQL_Record record0, record1;
        for (int i = 0; i < 12; i++) {
          record0.round_name[i] = input_app_data_in_bytes[i];
          record1.round_name[i] = input_app_data_in_bytes[i + 32];
        }
        for (int i = 0; i < 12; i++) {
          record0.player_name[i] = input_app_data_in_bytes[i + 12];
          record1.player_name[i] = input_app_data_in_bytes[i + 32 + 12];
        }
        for (int i = 0; i < 2; i++) {
          record0.score[i] = input_app_data_in_bytes[i + 12 + 12];
          record1.score[i] = input_app_data_in_bytes[i + 32 + 12 + 12];
        }
        for (int i = 0; i < 2; i++) {
          record0.month[i] = input_app_data_in_bytes[i + 12 + 12 + 2];
          record1.month[i] = input_app_data_in_bytes[i + 32 + 12 + 12 + 2];
        }
        for (int i = 0; i < 2; i++) {
          record0.day[i] = input_app_data_in_bytes[i + 12 + 12 + 2 + 2];
          record1.day[i] = input_app_data_in_bytes[i + 32 + 12 + 12 + 2 + 2];
        }
        for (int i = 0; i < 2; i++) {
          record0.year[i] = input_app_data_in_bytes[i + 12 + 12 + 2 + 2 + 2];
          record1.year[i] =
              input_app_data_in_bytes[i + 32 + 12 + 12 + 2 + 2 + 2];
        }
        record0.eop = false;
        app_sql_filter0_input_records.write(record0);
        record1.eop = input_app_data.eop;
        app_sql_filter1_input_records.write(record1);
      }
    }
  }
}

#endif
