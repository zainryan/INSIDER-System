#ifndef APP_SQL_WR_CPP_
#define APP_SQL_WR_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_sql_writer(ST_Queue<SQL_Record> &app_sql_filter0_output_records,
                    ST_Queue<SQL_Record> &app_sql_filter1_output_records,
                    ST_Queue<APP_Data> &app_output_data) {

  SQL_Record lefting_record;
  bool has_lefting_record = false;

  bool valid_record0 = false;
  bool valid_record1 = false;
  while (1) {
#pragma HLS pipeline
    SQL_Record record0, record1, output_record0, output_record1;
    if (!valid_record0) {
      valid_record0 = app_sql_filter0_output_records.read_nb(record0);
    }
    if (!valid_record1) {
      valid_record1 = app_sql_filter1_output_records.read_nb(record1);
    }

    if (valid_record0 && valid_record1) {
      valid_record0 = valid_record1 = false;
      bool record0_eop_or_valid = record0.valid || record0.eop;
      bool record1_eop_or_valid = record1.valid || record1.eop;

      bool has_full_write = false;
      if (record0_eop_or_valid && record1_eop_or_valid) {
        if (!has_lefting_record) {
          has_full_write = true;
          output_record0 = record0;
          output_record1 = record1;
        } else {
          has_full_write = true;
          output_record0 = lefting_record;
          output_record1 = record0;
          lefting_record = record1;
        }
      } else if (record0_eop_or_valid || record1_eop_or_valid) {
        SQL_Record record;
        if (record0_eop_or_valid) {
          record = record0;
        } else {
          record = record1;
        }

        if (!has_lefting_record) {
          lefting_record = record;
          has_lefting_record = true;
        } else {
          has_full_write = true;
          output_record0 = lefting_record;
          output_record1 = record;
          has_lefting_record = false;
        }
      }

      if (has_full_write) {
        APP_Data output_app_data;
        char output_app_data_in_bytes[64];
        for (int i = 0; i < 12; i++) {
          output_app_data_in_bytes[i] = output_record0.round_name[i];
          output_app_data_in_bytes[32 + i] = output_record1.round_name[i];
        }
        for (int i = 0; i < 12; i++) {
          output_app_data_in_bytes[i + 12] = output_record0.player_name[i];
          output_app_data_in_bytes[32 + 12 + i] = output_record1.player_name[i];
        }
        for (int i = 0; i < 2; i++) {
          output_app_data_in_bytes[i + 12 + 12] = output_record0.score[i];
          output_app_data_in_bytes[32 + 12 + 12 + i] = output_record1.score[i];
        }
        for (int i = 0; i < 2; i++) {
          output_app_data_in_bytes[i + 12 + 12 + 2] = output_record0.month[i];
          output_app_data_in_bytes[32 + 12 + 12 + 2 + i] =
              output_record1.month[i];
        }
        for (int i = 0; i < 2; i++) {
          output_app_data_in_bytes[i + 12 + 12 + 2 + 2] = output_record0.day[i];
          output_app_data_in_bytes[32 + 12 + 12 + 2 + 2 + i] =
              output_record1.day[i];
        }
        for (int i = 0; i < 2; i++) {
          output_app_data_in_bytes[i + 12 + 12 + 2 + 2 + 2] =
              output_record0.year[i];
          output_app_data_in_bytes[32 + 12 + 12 + 2 + 2 + 2 + i] =
              output_record1.year[i];
        }
        output_app_data.eop = output_record0.eop | output_record1.eop;
        output_app_data.len =
            32 * (output_record0.valid + output_record1.valid);
        for (int idx = 0; idx < 512; idx += 8) {
          output_app_data.data(idx + 7, idx) =
              output_app_data_in_bytes[idx / 8];
        }
        app_output_data.write(output_app_data);
      }
    } else if (has_lefting_record) {
      if (lefting_record.eop) {
        has_lefting_record = false;
        APP_Data output_app_data;
        char output_app_data_in_bytes[64];
        output_app_data.eop = true;
        output_app_data.len = lefting_record.valid * 32;
        for (int i = 0; i < 12; i++) {
          output_app_data_in_bytes[i] = lefting_record.round_name[i];
          output_app_data_in_bytes[32 + i] = 0;
        }
        for (int i = 0; i < 12; i++) {
          output_app_data_in_bytes[i + 12] = lefting_record.player_name[i];
          output_app_data_in_bytes[32 + 12 + i] = 0;
        }
        for (int i = 0; i < 2; i++) {
          output_app_data_in_bytes[i + 12 + 12] = lefting_record.score[i];
          output_app_data_in_bytes[32 + 12 + 12 + i] = 0;
        }
        for (int i = 0; i < 2; i++) {
          output_app_data_in_bytes[i + 12 + 12 + 2] = lefting_record.month[i];
          output_app_data_in_bytes[32 + 12 + 12 + 2 + i] = 0;
        }
        for (int i = 0; i < 2; i++) {
          output_app_data_in_bytes[i + 12 + 12 + 2 + 2] = lefting_record.day[i];
          output_app_data_in_bytes[32 + 12 + 12 + 2 + 2 + i] = 0;
        }
        for (int i = 0; i < 2; i++) {
          output_app_data_in_bytes[i + 12 + 12 + 2 + 2 + 2] =
              lefting_record.year[i];
          output_app_data_in_bytes[32 + 12 + 12 + 2 + 2 + 2 + i] = 0;
        }
        for (int idx = 0; idx < 512; idx += 8) {
          output_app_data.data(idx + 7, idx) =
              output_app_data_in_bytes[idx / 8];
        }
        app_output_data.write(output_app_data);
      }
    }
  }
}
#endif
