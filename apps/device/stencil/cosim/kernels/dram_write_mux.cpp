#ifndef DRAM_WRITE_MUX_CPP_
#define DRAM_WRITE_MUX_CPP_

#include <insider_kernel.h>

void process_state_1(ST_Queue<Dram_Write_Req_Data> &host_dram_write_req_data,
                     ST_Queue<Dram_Write_Req_Data> &dram_write_req_data,
                     unsigned char &state) {
  Dram_Write_Req_Data write_req_data;
  if (host_dram_write_req_data.read_nb(write_req_data)) {
    dram_write_req_data.write(write_req_data);
    if (write_req_data.last) {
      state = 0;
    }
  }
}

void process_state_2(ST_Queue<Dram_Write_Req_Data> &device_dram_write_req_data,
                     ST_Queue<Dram_Write_Req_Data> &dram_write_req_data,
                     unsigned char &state, unsigned char &pending_flits) {
  Dram_Write_Req_Data write_req_data;
  if (device_dram_write_req_data.read_nb(write_req_data)) {
    write_req_data.last = (pending_flits == 1);
    pending_flits--;
    dram_write_req_data.write(write_req_data);
    if (write_req_data.last) {
      state = 0;
    }
  }
}

void dram_write_mux(ST_Queue<Dram_Write_Req_Apply> &dram_write_req_apply,
                    ST_Queue<Dram_Write_Req_Data> &dram_write_req_data,
                    ST_Queue<Dram_Write_Req_Apply> &host_dram_write_req_apply,
                    ST_Queue<Dram_Write_Req_Data> &host_dram_write_req_data,
                    ST_Queue<Dram_Write_Req_Apply> &device_dram_write_req_apply,
                    ST_Queue<Dram_Write_Req_Data> &device_dram_write_req_data) {
  unsigned char state = 0;
  unsigned char pending_flits = 0;

  while (1) {
#pragma HLS pipeline
    if (state == 0) {
      Dram_Write_Req_Apply write_req_apply;
      if (host_dram_write_req_apply.read_nb(write_req_apply)) {
        dram_write_req_apply.write(write_req_apply);
        state = 1;
        process_state_1(host_dram_write_req_data, dram_write_req_data, state);
      } else if (device_dram_write_req_apply.read_nb(write_req_apply)) {
        dram_write_req_apply.write(write_req_apply);
        pending_flits = write_req_apply.num;
        state = 2;
        process_state_2(device_dram_write_req_data, dram_write_req_data, state,
                        pending_flits);
      }
    } else if (state == 1) {
      process_state_1(host_dram_write_req_data, dram_write_req_data, state);
    } else if (state == 2) {
      process_state_2(device_dram_write_req_data, dram_write_req_data, state,
                      pending_flits);
    }
  }
}

#endif
