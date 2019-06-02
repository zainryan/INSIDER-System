#ifndef STACCEL_ITC_H_
#define STACCEL_ITC_H_

#include "staccel_type.h"
#include "staccel_fifo.h"

#ifndef STACCEL_SYN
#include <map>
#include <mutex>
#include <thread>

std::map<unsigned long long, unsigned char> pcie_space;
std::map<unsigned long long, unsigned char> dramA_space;
std::map<unsigned long long, unsigned char> dramB_space;
std::map<unsigned long long, unsigned char> dramC_space;
std::map<unsigned long long, unsigned char> dramD_space;
static std::mutex pcie_space_mutex;
static std::mutex dramA_space_mutex;
static std::mutex dramB_space_mutex;
static std::mutex dramC_space_mutex;
static std::mutex dramD_space_mutex;
static std::mutex receive_ctrl_msg_mutex;

inline void dramA_space_write(unsigned long long addr, unsigned char data) {
  dramA_space_mutex.lock();
  dramA_space[addr] = data;
  dramA_space_mutex.unlock();
}

inline unsigned char dramA_space_read(unsigned long long addr) {
  unsigned char data;
  dramA_space_mutex.lock();
  data = dramA_space[addr];
  dramA_space_mutex.unlock();
  return data;
}

inline void dramB_space_write(unsigned long long addr, unsigned char data) {
  dramB_space_mutex.lock();
  dramB_space[addr] = data;
  dramB_space_mutex.unlock();
}

inline unsigned char dramB_space_read(unsigned long long addr) {
  unsigned char data;
  dramB_space_mutex.lock();
  data = dramB_space[addr];
  dramB_space_mutex.unlock();
  return data;
}

inline void dramC_space_write(unsigned long long addr, unsigned char data) {
  dramC_space_mutex.lock();
  dramC_space[addr] = data;
  dramC_space_mutex.unlock();
}

inline unsigned char dramC_space_read(unsigned long long addr) {
  unsigned char data;
  dramC_space_mutex.lock();
  data = dramC_space[addr];
  dramC_space_mutex.unlock();
  return data;
}

inline void dramD_space_write(unsigned long long addr, unsigned char data) {
  dramD_space_mutex.lock();
  dramD_space[addr] = data;
  dramD_space_mutex.unlock();
}

inline unsigned char dramD_space_read(unsigned long long addr) {
  unsigned char data;
  dramD_space_mutex.lock();
  data = dramD_space[addr];
  dramD_space_mutex.unlock();
  return data;
}

inline void pcie_space_write(unsigned long long addr, unsigned char data) {
  pcie_space_mutex.lock();
  pcie_space[addr] = data;
  pcie_space_mutex.unlock();
}

inline unsigned char pcie_space_read(unsigned long long addr) {
  unsigned char data;
  pcie_space_mutex.lock();
  data = pcie_space[addr];
  pcie_space_mutex.unlock();
  return data;
}

inline void send_control_msg(uint32_t tag, uint32_t msg) {
  Poke_Info info;
  info.tag = tag;
  info.data = msg;
  poke.write(info);
}

inline uint32_t receive_control_msg(uint32_t tag) {
  uint32_t msg;
  receive_ctrl_msg_mutex.lock();
  peek_req.write(tag);
  peek_resp.read(msg);
  receive_ctrl_msg_mutex.unlock();
  return msg;
}

void host_pcie_simulator() {
  int cnt = 0;
  while (1) {
    bool has_pcie_read_req, has_pcie_write_req;
    PCIe_Read_Req read_req;
    PCIe_Write_Req_Apply write_req_apply;
    has_pcie_read_req = pcie_read_req.read_nb(read_req);
    if (has_pcie_read_req) {
      PCIe_Read_Resp read_resp;
      for (int i = 0; i < read_req.num; i++) {
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = read_req.addr + i * 64 + j;
          pcie_space_mutex.lock();
          read_resp.data(8 * j + 7, 8 * j) = pcie_space[addr];
          pcie_space_mutex.unlock();
        }
        read_resp.last = (i == read_req.num - 1);
        pcie_read_resp.write(read_resp);
      }
    }
    has_pcie_write_req = pcie_write_req_apply.read_nb(write_req_apply);
    if (has_pcie_write_req) {
      PCIe_Write_Req_Data write_req_data;
      pcie_space_mutex.lock();
      for (int i = 0; i < write_req_apply.num; i++) {
        pcie_write_req_data.read(write_req_data);
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = write_req_apply.addr + i * 64 + j;
          pcie_space[addr] = write_req_data.data(8 * j + 7, 8 * j);
        }
      }
      pcie_space_mutex.unlock();
    }
  }
}

void fpga_dramA_simulator() {
  int cnt = 0;
  while (1) {
    bool has_dram_read_req, has_dram_write_req;
    Dram_Read_Req read_req;
    Dram_Write_Req_Apply write_req_apply;
    has_dram_read_req = dramA_read_req.read_nb(read_req);
    if (has_dram_read_req) {
      Dram_Read_Resp read_resp;
      for (int i = 0; i < read_req.num; i++) {
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = read_req.addr + i * 64 + j;
          dramA_space_mutex.lock();
          read_resp.data(8 * j + 7, 8 * j) = dramA_space[addr];
          dramA_space_mutex.unlock();
        }
        read_resp.last = (i == read_req.num - 1);
        dramA_read_resp.write(read_resp);
      }
    }
    has_dram_write_req = dramA_write_req_apply.read_nb(write_req_apply);
    if (has_dram_write_req) {
      Dram_Write_Req_Data write_req_data;
      for (int i = 0; i < write_req_apply.num; i++) {
        dramA_write_req_data.read(write_req_data);
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = write_req_apply.addr + i * 64 + j;
          dramA_space_mutex.lock();
          dramA_space[addr] = write_req_data.data(8 * j + 7, 8 * j);
          dramA_space_mutex.unlock();
        }
      }
    }
  }
}

void fpga_dramB_simulator() {
  int cnt = 0;
  while (1) {
    bool has_dram_read_req, has_dram_write_req;
    Dram_Read_Req read_req;
    Dram_Write_Req_Apply write_req_apply;
    has_dram_read_req = dramB_read_req.read_nb(read_req);
    if (has_dram_read_req) {
      Dram_Read_Resp read_resp;
      for (int i = 0; i < read_req.num; i++) {
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = read_req.addr + i * 64 + j;
          dramB_space_mutex.lock();
          read_resp.data(8 * j + 7, 8 * j) = dramB_space[addr];
          dramB_space_mutex.unlock();
        }
        read_resp.last = (i == read_req.num - 1);
        dramB_read_resp.write(read_resp);
      }
    }
    has_dram_write_req = dramB_write_req_apply.read_nb(write_req_apply);
    if (has_dram_write_req) {
      Dram_Write_Req_Data write_req_data;
      for (int i = 0; i < write_req_apply.num; i++) {
        dramB_write_req_data.read(write_req_data);
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = write_req_apply.addr + i * 64 + j;
          dramB_space_mutex.lock();
          dramB_space[addr] = write_req_data.data(8 * j + 7, 8 * j);
          dramB_space_mutex.unlock();
        }
      }
    }
  }
}

void fpga_dramC_simulator() {
  int cnt = 0;
  while (1) {
    bool has_dram_read_req, has_dram_write_req;
    Dram_Read_Req read_req;
    Dram_Write_Req_Apply write_req_apply;
    has_dram_read_req = dramC_read_req.read_nb(read_req);
    if (has_dram_read_req) {
      Dram_Read_Resp read_resp;
      for (int i = 0; i < read_req.num; i++) {
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = read_req.addr + i * 64 + j;
          dramC_space_mutex.lock();
          read_resp.data(8 * j + 7, 8 * j) = dramC_space[addr];
          dramC_space_mutex.unlock();
        }
        read_resp.last = (i == read_req.num - 1);
        dramC_read_resp.write(read_resp);
      }
    }
    has_dram_write_req = dramC_write_req_apply.read_nb(write_req_apply);
    if (has_dram_write_req) {
      Dram_Write_Req_Data write_req_data;
      for (int i = 0; i < write_req_apply.num; i++) {
        dramC_write_req_data.read(write_req_data);
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = write_req_apply.addr + i * 64 + j;
          dramC_space_mutex.lock();
          dramC_space[addr] = write_req_data.data(8 * j + 7, 8 * j);
          dramC_space_mutex.unlock();
        }
      }
    }
  }
}

void fpga_dramD_simulator() {
  int cnt = 0;
  while (1) {
    bool has_dram_read_req, has_dram_write_req;
    Dram_Read_Req read_req;
    Dram_Write_Req_Apply write_req_apply;
    has_dram_read_req = dramD_read_req.read_nb(read_req);
    if (has_dram_read_req) {
      Dram_Read_Resp read_resp;
      for (int i = 0; i < read_req.num; i++) {
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = read_req.addr + i * 64 + j;
          dramD_space_mutex.lock();
          read_resp.data(8 * j + 7, 8 * j) = dramD_space[addr];
          dramD_space_mutex.unlock();
        }
        read_resp.last = (i == read_req.num - 1);
        dramD_read_resp.write(read_resp);
      }
    }
    has_dram_write_req = dramD_write_req_apply.read_nb(write_req_apply);
    if (has_dram_write_req) {
      Dram_Write_Req_Data write_req_data;
      for (int i = 0; i < write_req_apply.num; i++) {
        dramD_write_req_data.read(write_req_data);
        for (int j = 0; j < 64; j++) {
          unsigned long long addr;
          addr = write_req_apply.addr + i * 64 + j;
          dramD_space_mutex.lock();
          dramD_space[addr] = write_req_data.data(8 * j + 7, 8 * j);
          dramD_space_mutex.unlock();
        }
      }
    }
  }
}

#endif

#endif
