#ifndef STACCEL_TYPE_
#define STACCEL_TYPE_

#define _Bool bool

#include <ap_int.h>
#ifdef STACCEL_SYN
#include <hls_stream.h>
#define ST_Queue hls::stream
#else
#include <boost/lockfree/queue.hpp>
template <typename T> class ST_Queue {
private:
  boost::lockfree::queue<T, boost::lockfree::fixed_sized<true>> queue;

public:
  ST_Queue(size_t depth) : queue(depth) {}

  bool read_nb(T &data) { return queue.pop(data); }

  bool write_nb(T data) { return queue.push(data); }

  void read(T &data) {
    while (!queue.pop(data))
      ;
  }

  void write(T data) {
    while (!queue.push(data))
      ;
  }
};
#endif

typedef unsigned int Peek_Info;

struct Poke_Info {
  unsigned int tag;
  unsigned int data;
};

struct PCIe_Write_Req_Data {
  bool last;
  ap_uint<512> data;
};

struct PCIe_Write_Req_Apply {
  unsigned char num;
  unsigned long long addr;
};

struct PCIe_Read_Resp {
  bool last;
  ap_uint<512> data;
};

struct PCIe_Read_Req {
  unsigned char num;
  unsigned long long addr;
};

struct Dram_Write_Req_Data {
  bool last;
  ap_uint<512> data;
};

struct Dram_Write_Req_Apply {
  unsigned char num;
  unsigned long long addr;
};

struct Dram_Read_Resp {
  bool last;
  ap_uint<512> data;
};

struct Dram_Read_Req {
  unsigned char num;
  unsigned long long addr;
};

#endif
