#ifndef READ_MODE_DRAM_HELPER_APP_CPP_
#define READ_MODE_DRAM_HELPER_APP_CPP_

#include <insider_kernel.h>

void read_mode_dram_helper_app(ST_Queue<unsigned int> &app_file_infos,
                               ST_Queue<bool> &app_is_write_mode,
                               ST_Queue<Dram_Read_Req> &device_dram_read_req,
                               ST_Queue<Dram_Read_Resp> &device_dram_read_resp,
                               ST_Queue<APP_Data> &app_input_data,
                               ST_Queue<bool> &reset_dram_helper_app) {
  bool reset = false;
  unsigned int reset_cnt = 0;

  unsigned long long tmp_start;
  unsigned long long tmp_len;

  // in byte unit
  unsigned long long extents_start_arr[MAX_EXTENT_NUM];
#pragma HLS array_partition variable = extents_start_arr complete
  unsigned long long extents_len_arr[MAX_EXTENT_NUM];
#pragma HLS array_partition variable = extents_len_arr complete
  unsigned char extents_arr_store_idx = 0;
  unsigned char num_extents;

  unsigned long long length;
  bool first_length_half = true;

  unsigned char extents_arr_load_idx = 0;
  ap_uint<DRAM_ADDR_BUS_WIDTH> cur_extent = 0;
  ap_uint<DRAM_ADDR_BUS_WIDTH> cur_extent_len_left = 0;

  unsigned char state = 0;
  bool should_read_start = true;
  bool first_extents_half = true;

  unsigned int num_read = 0;
  unsigned int num_issued = 0;
  bool finish_issuing = false;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset || (reset = reset_dram_helper_app.read_nb(dummy))) {
      extents_arr_store_idx = 0;
      first_length_half = true;
      extents_arr_load_idx = 0;
      cur_extent = 0;
      cur_extent_len_left = 0;
      state = 0;
      should_read_start = true;
      first_extents_half = true;
      num_read = 0;
      num_issued = 0;
      finish_issuing = false;

      unsigned int dummy_0;
      app_file_infos.read_nb(dummy_0);
      Dram_Read_Resp dummy_1;
      device_dram_read_resp.read_nb(dummy_1);
      bool dummy_2;
      app_is_write_mode.read_nb(dummy_2);

      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      if (state == 0) {
        bool is_write_mode;
        if (app_is_write_mode.read_nb(is_write_mode)) {
          if (!is_write_mode) {
            state = 1;
          }
        }
      } else if (state == 1) {
        unsigned int tmp;
        if (app_file_infos.read_nb(tmp)) {
          num_extents = tmp;
          state = 2;
        }
      } else if (state == 2) {
        unsigned int tmp;
        if (app_file_infos.read_nb(tmp)) {
          if (first_length_half) {
            length = tmp;
            first_length_half = false;
          } else {
            length = (length << 32) | tmp;
            state = 3;
          }
        }
      } else if (state == 3) {
        unsigned int tmp;
        if (app_file_infos.read_nb(tmp)) {
          if (should_read_start) {
            if (first_extents_half) {
              tmp_start = tmp;
            } else {
              tmp_start = (tmp_start << 32) | tmp;
              should_read_start = false;
              if (extents_arr_store_idx == 0) {
                cur_extent = tmp_start;
              }
              extents_start_arr[extents_arr_store_idx] = tmp_start;
            }
          } else {
            if (first_extents_half) {
              tmp_len = tmp;
            } else {
              tmp_len = (tmp_len << 32) | tmp;
              should_read_start = true;
              if (extents_arr_store_idx == 0) {
                cur_extent_len_left = tmp_len;
              }
              extents_len_arr[extents_arr_store_idx] = tmp_len;
              extents_arr_store_idx++;
              if (extents_arr_store_idx == num_extents) {
                state = 4;
              }
            }
          }
          first_extents_half ^= 1;
        }
      } else if (state == 4) {
        Dram_Read_Req read_req;

        read_req.addr = cur_extent;
        if ((DRAM_READ_BATCH_NUM << DATA_BUS_WIDTH_LOG2) <
            cur_extent_len_left) {
          read_req.num = DRAM_READ_BATCH_NUM;
          if (device_dram_read_req.write_nb(read_req)) {
            num_issued += read_req.num;
            cur_extent =
                cur_extent + (DRAM_READ_BATCH_NUM << DATA_BUS_WIDTH_LOG2);
            cur_extent_len_left = cur_extent_len_left -
                                  (DRAM_READ_BATCH_NUM << DATA_BUS_WIDTH_LOG2);
          }
        } else {
          read_req.num = (cur_extent_len_left + (DATA_BUS_WIDTH - 1)) >>
                         DATA_BUS_WIDTH_LOG2;
          if (device_dram_read_req.write_nb(read_req)) {
            num_issued += read_req.num;
            cur_extent = extents_start_arr[extents_arr_load_idx + 1];
            cur_extent_len_left = extents_len_arr[extents_arr_load_idx + 1];
            extents_arr_load_idx++;
            if (extents_arr_load_idx == num_extents) {
              finish_issuing = true;
              state = 5;
            }
          }
        }
      }

      if (state) {
        Dram_Read_Resp read_resp;
        if (device_dram_read_resp.read_nb(read_resp)) {
          num_read++;
          APP_Data data;
          data.data = read_resp.data;
          data.eop = false;
          data.len = DATA_BUS_WIDTH;
          if ((num_issued == num_read) && finish_issuing) {
            data.eop = true;
            data.len = length & (DATA_BUS_WIDTH - 1);
            if (data.len == 0) {
              data.len = DATA_BUS_WIDTH;
            }
          }
          app_input_data.write(data);
        }
      }
    }
  }
}

#endif
