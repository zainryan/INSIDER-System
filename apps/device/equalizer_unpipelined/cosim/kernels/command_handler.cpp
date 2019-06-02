#ifndef COMMAND_HANDLER_CPP_
#define COMMAND_HANDLER_CPP_

#include <insider_kernel.h>

void command_handler(ST_Queue<Poke_Info> &poke,
                     ST_Queue<Request> &reqs_incoming,
                     ST_Queue<unsigned int> &kbuf_addrs,
                     ST_Queue<unsigned int> &dma_read_throttle_params,
                     ST_Queue<unsigned int> &dma_write_throttle_params,
                     ST_Queue<unsigned int> &device_read_delay_cycle_cnts,
                     ST_Queue<unsigned int> &device_write_delay_cycle_cnts,
                     ST_Queue<unsigned int> &drive_read_throttle_params,
                     ST_Queue<unsigned int> &drive_write_throttle_params,
                     ST_Queue<unsigned int> &app_file_infos_0,
                     ST_Queue<unsigned int> &app_file_infos_1,
                     ST_Queue<unsigned int> &read_mode_app_buf_addrs,
                     ST_Queue<unsigned int> &write_mode_app_buf_addrs,
                     ST_Queue<bool> &app_free_buf,
                     ST_Queue<unsigned long long> &app_write_total_len,
                     ST_Queue<unsigned int> &app_input_params,
                     ST_Queue<unsigned int> &app_commit_write_buf,
                     ST_Queue<bool> &app_is_write_mode_0,
                     ST_Queue<bool> &app_is_write_mode_1,
                     ST_Queue<bool> &app_is_write_mode_2,
                     ST_Queue<bool> &reset_sigs) {
  unsigned int tag = 0;
  Request req;
  bool is_read_host_throttle_params = true;
  bool is_read_device_throttle_params = true;
  bool has_half_write_total_len = false;
  unsigned long long write_total_len = 0;

  while (1) {
#pragma HLS pipeline
    Poke_Info info;
    int state = 0;
    if (poke.read_nb(info)) {
      if (info.tag >= TAG(WRITE_CMD_TAG_OFFSET)) {
        state = 1;
      } else if (info.tag >= TAG(READ_CMD_TAG_OFFSET)) {
        state = 2;
      } else if (info.tag == TAG(KBUF_ADDR_TAG)) {
        state = 3;
      } else if (info.tag == TAG(DMA_THROTTLE_PARAM_TAG)) {
        state = 5;
      } else if (info.tag == TAG(DRIVE_DELAY_CYCLE_TAG)) {
        state = 6;
      } else if (info.tag == TAG(DRIVE_THROTTLE_PARAM_TAG)) {
        state = 7;
      } else if (info.tag == TAG(APP_FILE_INFO_TAG)) {
        state = 9;
      } else if (info.tag == TAG(APP_BUF_ADDRS_TAG)) {
        state = 10;
      } else if (info.tag == TAG(APP_FREE_BUF_TAG)) {
        state = 11;
      } else if (info.tag == TAG(APP_INPUT_PARAM_TAG)) {
        state = 12;
      } else if (info.tag == TAG(RESET_TAG)) {
        state = 13;
      } else if (info.tag == TAG(APP_COMMIT_WRITE_BUF_TAG)) {
        state = 14;
      } else if (info.tag == TAG(APP_WRITE_TOTAL_LEN_TAG)) {
        state = 15;
      } else if (info.tag == TAG(APP_IS_WRITE_MODE_TAG)) {
        state = 16;
      }
    }

    if (state == 0) {
      // do nothing
    } else if (state == 1) {
      Request req;
      req.sector_off = info.data >> 3;
      req.sector_num = (info.data & 7) + 1;
      req.rw = WRITE_CMD;
      req.tag = (info.tag >> 2) - WRITE_CMD_TAG_OFFSET;
      reqs_incoming.write(req);
    } else if (state == 2) {
      Request req;
      req.sector_off = info.data >> 3;
      req.sector_num = (info.data & 7) + 1;
      req.rw = READ_CMD;
      req.tag = (info.tag >> 2) - READ_CMD_TAG_OFFSET;
      reqs_incoming.write(req);
    } else if (state == 3) {
      kbuf_addrs.write(info.data);
    } else if (state == 5) {
      if (is_read_host_throttle_params) {
        is_read_host_throttle_params = 0;
        dma_read_throttle_params.write(info.data);
      } else {
        is_read_host_throttle_params = 1;
        dma_write_throttle_params.write(info.data);
      }
    } else if (state == 6) {
      device_read_delay_cycle_cnts.write(info.data);
      device_write_delay_cycle_cnts.write(info.data);
    } else if (state == 7) {
      if (is_read_device_throttle_params) {
        is_read_device_throttle_params = 0;
        drive_read_throttle_params.write(info.data);
      } else {
        is_read_device_throttle_params = 1;
        drive_write_throttle_params.write(info.data);
      }
    } else if (state == 9) {
      app_file_infos_0.write_nb(info.data);
      app_file_infos_1.write_nb(info.data);
    } else if (state == 10) {
      read_mode_app_buf_addrs.write(info.data);
      write_mode_app_buf_addrs.write(info.data);
    } else if (state == 11) {
      app_free_buf.write(0);
    } else if (state == 12) {
      app_input_params.write(info.data);
    } else if (state == 13) {
      reset_sigs.write(0);
    } else if (state == 14) {
      app_commit_write_buf.write(info.data);
    } else if (state == 15) {
      if (!has_half_write_total_len) {
        write_total_len = ((unsigned long long)info.data) << 32;
        has_half_write_total_len = true;
      } else {
        write_total_len |= info.data;
        has_half_write_total_len = false;
        app_write_total_len.write(write_total_len);
      }
    } else if (state == 16) {
      app_is_write_mode_0.write(info.data);
      app_is_write_mode_1.write(info.data);
      app_is_write_mode_2.write(info.data);
    }
  }
}

#endif
