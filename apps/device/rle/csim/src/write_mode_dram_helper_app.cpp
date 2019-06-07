#ifndef WRITE_MODE_DRAM_HELPER_APP_CPP_
#define WRITE_MODE_DRAM_HELPER_APP_CPP_

#include <insider_kernel.h>

void write_mode_dram_helper_app(
    ST_Queue<unsigned int> &peek_real_written_bytes_req,
    ST_Queue<unsigned int> &peek_real_written_bytes_resp,
    ST_Queue<unsigned int> &peek_write_finished_req,
    ST_Queue<unsigned int> &peek_write_finished_resp,
    ST_Queue<unsigned int> &app_file_infos, ST_Queue<bool> &app_is_write_mode,
    ST_Queue<APP_Data> &cached_app_output_data_chan,
    ST_Queue<Dram_Write_Req_Apply> &cached_device_dram_write_req_apply,
    ST_Queue<Dram_Write_Req_Apply> &device_dram_write_req_apply,
    ST_Queue<Dram_Write_Req_Data> &device_dram_write_req_data,
    ST_Queue<bool> &reset_dram_helper_app) {
  bool reset = false;
  unsigned int reset_cnt = 0;

  ap_uint<40> tmp_start;
  ap_uint<40> tmp_len;

  // in byte unit
  unsigned int extents_start_arr[MAX_EXTENT_NUM];
#pragma HLS array_partition variable = extents_start_arr complete
  unsigned int extents_len_arr[MAX_EXTENT_NUM];
#pragma HLS array_partition variable = extents_len_arr complete
  unsigned char extents_arr_store_idx = 0;
  unsigned char num_extents;

  ap_uint<40> length;
  bool first_length_half = true;

  unsigned char state = 0;
  bool should_read_start = true;
  bool first_extents_half = true;

  Dram_Write_Req_Apply cached_dram_write_req_apply;

  unsigned char extents_arr_load_idx = 0;
  unsigned int cur_extents_offset = 0;

  unsigned int cur_extents_len;
  unsigned int cur_extents_start;
  unsigned int cur_extents_left_space;

  unsigned long long real_written_bytes = 0;
  bool peek_real_written_bytes_resp_first_half = true;

  bool has_cached_app_output_data = false;
  APP_Data cached_app_output_data;

  bool write_finished = false;

  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset || (reset = reset_dram_helper_app.read_nb(dummy))) {
      extents_arr_store_idx = 0;
      first_length_half = true;
      state = 0;
      should_read_start = true;
      first_extents_half = true;
      extents_arr_load_idx = 0;
      cur_extents_offset = 0;
      real_written_bytes = 0;
      has_cached_app_output_data = false;
      peek_real_written_bytes_resp_first_half = true;
      write_finished = false;

      unsigned int dummy_0;
      app_file_infos.read_nb(dummy_0);
      bool dummy_1;
      app_is_write_mode.read_nb(dummy_1);
      APP_Data dummy_2;
      cached_app_output_data_chan.read_nb(dummy_2);
      Dram_Write_Req_Apply dummy_3;
      cached_device_dram_write_req_apply.read_nb(dummy_3);
      unsigned int dummy_4;
      peek_real_written_bytes_req.read_nb(dummy_4);
      unsigned int dummy_5;
      peek_write_finished_req.read_nb(dummy_5);

      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      unsigned int req;
      if (peek_real_written_bytes_req.read_nb(req)) {
        if (peek_real_written_bytes_resp_first_half) {
          peek_real_written_bytes_resp.write(real_written_bytes >> 32);
        } else {
          peek_real_written_bytes_resp.write(real_written_bytes & 0xFFFFFFFF);
        }
        peek_real_written_bytes_resp_first_half ^= 1;
      }

      if (peek_write_finished_req.read_nb(req)) {
        peek_write_finished_resp.write(write_finished);
      }

      if (state == 0) {
        bool is_write_mode;
        if (app_is_write_mode.read_nb(is_write_mode)) {
          if (is_write_mode) {
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
              extents_start_arr[extents_arr_store_idx] =
                  tmp_start >> DATA_BUS_WIDTH_LOG2;
            }
          } else {
            if (first_extents_half) {
              tmp_len = tmp;
            } else {
              tmp_len = (tmp_len << 32) | tmp;
              should_read_start = true;
              extents_len_arr[extents_arr_store_idx] =
                  tmp_len >> DATA_BUS_WIDTH_LOG2;
              extents_arr_store_idx++;
              if (extents_arr_store_idx == num_extents) {
                state = 4;
              }
            }
          }
          first_extents_half ^= 1;
        }
      } else if (state == 4) {
        if (cached_device_dram_write_req_apply.read_nb(
                cached_dram_write_req_apply)) {
          state = 5;
          cur_extents_len = extents_len_arr[extents_arr_load_idx];
          cur_extents_start = extents_start_arr[extents_arr_load_idx];
          cur_extents_left_space = cur_extents_len - cur_extents_offset;
        }
      } else if (state == 5) {
        if ((((unsigned long long)cur_extents_left_space)
             << DATA_BUS_WIDTH_LOG2) >
            ((unsigned long long)cached_dram_write_req_apply.num)
                << DATA_BUS_WIDTH_LOG2) {
          Dram_Write_Req_Apply write_req_apply;
          write_req_apply.num = cached_dram_write_req_apply.num;
          write_req_apply.addr =
              ((unsigned long long)(cur_extents_offset + cur_extents_start))
              << DATA_BUS_WIDTH_LOG2;
          if (device_dram_write_req_apply.write_nb(write_req_apply)) {
            cur_extents_offset +=
                (((unsigned long long)cached_dram_write_req_apply.num)
                 << DATA_BUS_WIDTH_LOG2) >>
                DATA_BUS_WIDTH_LOG2;
            state = 4;
          }
        } else {
          Dram_Write_Req_Apply write_req_apply;
          write_req_apply.num = ((((unsigned long long)cur_extents_left_space)
                                  << DATA_BUS_WIDTH_LOG2) >>
                                 DATA_BUS_WIDTH_LOG2);
          write_req_apply.addr =
              ((unsigned long long)(cur_extents_offset + cur_extents_start))
              << DATA_BUS_WIDTH_LOG2;
          if (device_dram_write_req_apply.write_nb(write_req_apply)) {
            cached_dram_write_req_apply.num -=
                ((((unsigned long long)cur_extents_left_space)
                  << DATA_BUS_WIDTH_LOG2) >>
                 DATA_BUS_WIDTH_LOG2);
            if (!cached_dram_write_req_apply.num) {
              state = 4;
            }
            extents_arr_load_idx++;
            cur_extents_offset = 0;
            cur_extents_len = extents_len_arr[extents_arr_load_idx];
            cur_extents_start = extents_start_arr[extents_arr_load_idx];
            cur_extents_left_space = cur_extents_len;
          }
        }
      }

      if (has_cached_app_output_data ||
          (has_cached_app_output_data =
               cached_app_output_data_chan.read_nb(cached_app_output_data))) {
        write_finished = cached_app_output_data.eop;
        Dram_Write_Req_Data req_data;
        // Potentially has partial write issue. Since dram write is
        // 64-byte aligned.
        req_data.data = cached_app_output_data.data;
        // The last signal has been later maintained by dram_write_mux.
        if (device_dram_write_req_data.write_nb(req_data)) {
          has_cached_app_output_data = false;
          real_written_bytes += DATA_BUS_WIDTH;
        }
      }
    }
  }
}

#endif
