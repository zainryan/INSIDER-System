#ifndef INSIDER_TYPES_H_
#define INSIDER_TYPES_H_

#include <staccel_type.h>

struct Request {
  unsigned int sector_off;
  unsigned int sector_num;
  unsigned int tag;
  bool rw; // 0 read, 1 write
};

struct Data {
  bool last;
  ap_uint<512> data;
};

struct Dram_Read_Req_With_Time {
  Dram_Read_Req req;
  unsigned long long time;
};

struct Dram_Write_Req_Apply_With_Time {
  Dram_Write_Req_Apply req_apply;
  unsigned long long time;
};

struct Dram_Dispatcher_Write_Req {
  unsigned char bank_id;
  unsigned char end_bank_id;
  unsigned int before_boundry_num;
  unsigned int cmd_num;
};

struct Dram_Dispatcher_Read_Req {
  unsigned char bank_id;
  unsigned char end_bank_id;
  unsigned int cmd_num;
};

struct APP_Data {
  ap_uint<512> data;
  unsigned short len;
  bool eop;
};

struct APP_Data_Meta {
  unsigned int num;
  bool eop;
};

struct Write_Mode_PCIe_Read_Req_Context {
  unsigned short len;
  bool last;
  unsigned long long metadata_addr;
};

#endif
