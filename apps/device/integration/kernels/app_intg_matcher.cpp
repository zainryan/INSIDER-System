#ifndef APP_INTG_MATCHER_CPP_
#define APP_INTG_MATCHER_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"
void app_intg_matcher(ST_Queue<unsigned int> &app_intg_input_params,
                      ST_Queue<APP_Data> &app_intg_input_data,
                      ST_Queue<unsigned int> &app_intg_writer_input_param,
                      ST_Queue<APP_Ver_Param> &app_intg_verifier_input_param,
                      ST_Queue<APP_Match> &app_intg_mat_rdc_input) {
  bool valid_param = false;
  bool get_thresParam = false;
  char readParamCnt = 0;
  unsigned int query_thres = 0;
  unsigned int record_thres = 0;
  unsigned char query[APP_QUERY_LENGTH];
#pragma HLS array_partition variable = query complete
  unsigned char record[APP_COL_NUM];
  while (1) {
#pragma HLS pipeline
    if (!valid_param) {
      // state 0 -- get thres
      // state 1~8 -- get (8) 4B of query
      APP_Ver_Param query_out;
#pragma HLS array_partition variable = query_out.query complete
      unsigned int params;
      if (app_intg_input_params.read_nb(params)) {
        if (!get_thresParam) {
          query_thres = params & 0xFFFF;          // low 16bits
          record_thres = (params >> 16) & 0xFFFF; // high 16bits
          get_thresParam = true;
        } else {
          query[readParamCnt * 4 + 0] = (params >> 0) & 0xFF;
          query[readParamCnt * 4 + 1] = (params >> 8) & 0xFF;
          query[readParamCnt * 4 + 2] = (params >> 16) & 0xFF;
          query[readParamCnt * 4 + 3] = (params >> 24) & 0xFF;
          query_out.query[readParamCnt * 4 + 0] = (params >> 0) & 0xFF;
          query_out.query[readParamCnt * 4 + 1] = (params >> 8) & 0xFF;
          query_out.query[readParamCnt * 4 + 2] = (params >> 16) & 0xFF;
          query_out.query[readParamCnt * 4 + 3] = (params >> 24) & 0xFF;
          readParamCnt++;
          if (readParamCnt == 8) {
            valid_param = true;
            query_out.thres = query_thres;
            app_intg_verifier_input_param.write(query_out);
            app_intg_writer_input_param.write(query_thres);
          }
        }
      }
    } else {
      APP_Data read_data;
      if (app_intg_input_data.read_nb(read_data)) {
        for (int i = 0; i < 64; i++) {
#pragma HLS unroll
          record[i] = read_data.data(8 * i + 7, 8 * i);
        }
        bool match_vec[APP_QUERY_LENGTH];
        for (int i = 0; i < APP_QUERY_LENGTH; i++) {
#pragma HLS unroll
          match_vec[i] = false;
          for (int j = 0; j < APP_COL_NUM; j++) {
#pragma HLS unroll
            if (i < query_thres && j < record_thres) {
              if (query[i] == record[j]) {
                match_vec[i] = true;
              }
            }
          }
        }
        APP_Match match_out;
        for (int i = 0; i < 64; i++) {
#pragma HLS unroll
          match_out.record[i] = record[i];
        }
        for (int i = 0; i < 32; i++) {
#pragma HLS unroll
          match_out.match[i] = match_vec[i];
        }
        match_out.eop = read_data.eop;
        app_intg_mat_rdc_input.write(match_out);
      }
    }
  }
}

#endif
