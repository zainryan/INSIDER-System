#ifndef PEEK_HANDLER_CPP_
#define PEEK_HANDLER_CPP_

#include <insider_kernel.h>

void peek_handler(ST_Queue<unsigned int> &peek_req,
                  ST_Queue<unsigned int> &peek_resp,
                  ST_Queue<unsigned int> &peek_real_written_bytes_req,
                  ST_Queue<unsigned int> &peek_real_written_bytes_resp,
                  ST_Queue<unsigned int> &peek_write_finished_req,
                  ST_Queue<unsigned int> &peek_write_finished_resp,
                  ST_Queue<unsigned int> &peek_virt_written_bytes_req,
                  ST_Queue<unsigned int> &peek_virt_written_bytes_resp) {
  bool has_inflight_req = false;
  unsigned char inflight_req_state;

  while (1) {
#pragma HLS pipeline
    unsigned int req;
    unsigned char state = 0;

    if (!has_inflight_req) {
      if (peek_req.read_nb(req)) {
        if (req == TAG(APP_REAL_WRITTEN_BYTES_TAG)) {
          state = 2;
        } else if (req == TAG(APP_WRITE_FINISHED_TAG)) {
          state = 3;
        } else if (req == TAG(APP_VIRT_WRITTEN_BYTES_TAG)) {
          state = 4;
        } else {
          state = 1;
        }
      }

      if (state) {
        if (state == 1) {
          peek_resp.write(0);
        } else {
          has_inflight_req = true;
          inflight_req_state = state;
          if (state == 2) {
            peek_real_written_bytes_req.write(req);
          } else if (state == 3) {
            peek_write_finished_req.write(req);
          } else if (state == 4) {
            peek_virt_written_bytes_req.write(req);
          }
        }
      }
    } else {
      unsigned int resp;
      if (inflight_req_state == 2) {
        if (peek_real_written_bytes_resp.read_nb(resp)) {
          peek_resp.write(resp);
          has_inflight_req = false;
        }
      } else if (inflight_req_state == 3) {
        if (peek_write_finished_resp.read_nb(resp)) {
          peek_resp.write(resp);
          has_inflight_req = false;
        }
      } else if (inflight_req_state == 4) {
        if (peek_virt_written_bytes_resp.read_nb(resp)) {
          peek_resp.write(resp);
          has_inflight_req = false;
        }
      }
    }
  }
}

#endif

