#ifndef DRAM_DATA_CACHING_CPP_
#define DRAM_DATA_CACHING_CPP_

#include <insider_kernel.h>

void dram_data_caching(
    ST_Queue<Dram_Read_Resp> &dramA_read_resp,
    ST_Queue<Dram_Read_Resp> &dramB_read_resp,
    ST_Queue<Dram_Read_Resp> &dramC_read_resp,
    ST_Queue<Dram_Read_Resp> &dramD_read_resp,
    ST_Queue<Dram_Read_Resp> &cached_dramA_read_resp,
    ST_Queue<Dram_Read_Resp> &cached_dramB_read_resp,
    ST_Queue<Dram_Read_Resp> &cached_dramC_read_resp,
    ST_Queue<Dram_Read_Resp> &cached_dramD_read_resp,
    ST_Queue<Dram_Write_Req_Data> &dramA_write_req_data,
    ST_Queue<Dram_Write_Req_Data> &dramB_write_req_data,
    ST_Queue<Dram_Write_Req_Data> &dramC_write_req_data,
    ST_Queue<Dram_Write_Req_Data> &dramD_write_req_data,
    ST_Queue<Dram_Write_Req_Data> &cached_dramA_write_req_data,
    ST_Queue<Dram_Write_Req_Data> &cached_dramB_write_req_data,
    ST_Queue<Dram_Write_Req_Data> &cached_dramC_write_req_data,
    ST_Queue<Dram_Write_Req_Data> &cached_dramD_write_req_data) {
  bool has_dramA_read_resp = false;
  bool has_dramB_read_resp = false;
  bool has_dramC_read_resp = false;
  bool has_dramD_read_resp = false;
  Dram_Read_Resp data_dramA_read_resp;
  Dram_Read_Resp data_dramB_read_resp;
  Dram_Read_Resp data_dramC_read_resp;
  Dram_Read_Resp data_dramD_read_resp;

  bool has_cached_dramA_write_req_data = false;
  bool has_cached_dramB_write_req_data = false;
  bool has_cached_dramC_write_req_data = false;
  bool has_cached_dramD_write_req_data = false;
  Dram_Write_Req_Data data_cached_dramA_write_req_data;
  Dram_Write_Req_Data data_cached_dramB_write_req_data;
  Dram_Write_Req_Data data_cached_dramC_write_req_data;
  Dram_Write_Req_Data data_cached_dramD_write_req_data;

  while (1) {
#pragma HLS pipeline
    if (has_dramA_read_resp ||
        (has_dramA_read_resp = dramA_read_resp.read_nb(data_dramA_read_resp))) {
      if (cached_dramA_read_resp.write_nb(data_dramA_read_resp)) {
        has_dramA_read_resp = false;
      }
    }

    if (has_dramB_read_resp ||
        (has_dramB_read_resp = dramB_read_resp.read_nb(data_dramB_read_resp))) {
      if (cached_dramB_read_resp.write_nb(data_dramB_read_resp)) {
        has_dramB_read_resp = false;
      }
    }

    if (has_dramC_read_resp ||
        (has_dramC_read_resp = dramC_read_resp.read_nb(data_dramC_read_resp))) {
      if (cached_dramC_read_resp.write_nb(data_dramC_read_resp)) {
        has_dramC_read_resp = false;
      }
    }

    if (has_dramD_read_resp ||
        (has_dramD_read_resp = dramD_read_resp.read_nb(data_dramD_read_resp))) {
      if (cached_dramD_read_resp.write_nb(data_dramD_read_resp)) {
        has_dramD_read_resp = false;
      }
    }

    if (has_cached_dramA_write_req_data ||
        (has_cached_dramA_write_req_data =
             cached_dramA_write_req_data.read_nb(data_cached_dramA_write_req_data))) {
      if (dramA_write_req_data.write_nb(data_cached_dramA_write_req_data)) {
        has_cached_dramA_write_req_data = false;
      }
    }

    if (has_cached_dramB_write_req_data ||
        (has_cached_dramB_write_req_data =
             cached_dramB_write_req_data.read_nb(data_cached_dramB_write_req_data))) {
      if (dramB_write_req_data.write_nb(data_cached_dramB_write_req_data)) {
        has_cached_dramB_write_req_data = false;
      }
    }

    if (has_cached_dramC_write_req_data ||
        (has_cached_dramC_write_req_data =
             cached_dramC_write_req_data.read_nb(data_cached_dramC_write_req_data))) {
      if (dramC_write_req_data.write_nb(data_cached_dramC_write_req_data)) {
        has_cached_dramC_write_req_data = false;
      }
    }

    if (has_cached_dramD_write_req_data ||
        (has_cached_dramD_write_req_data =
             cached_dramD_write_req_data.read_nb(data_cached_dramD_write_req_data))) {
      if (dramD_write_req_data.write_nb(data_cached_dramD_write_req_data)) {
        has_cached_dramD_write_req_data = false;
      }
    }
  }
}

#endif
