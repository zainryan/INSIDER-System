#ifndef FPGA_UTIL_H_
#define FPGA_UTIL_H_

#include "const.h"
#include "fpga_mgmt.h"
#include "fpga_pci.h"
#include "utils/lcd.h"
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

extern uint16_t _pci_vendor_id;
extern uint16_t _pci_device_id;
extern pci_bar_handle_t _pci_bar_handle;
extern struct fpga_pci_bar *_bar;
extern void *_IBufs[INUM_LIMIT];
extern void *_OBufs[ONUM_LIMIT];
extern uint64_t _IBufs_phy[INUM_LIMIT];
extern uint64_t _OBufs_phy[ONUM_LIMIT];
extern int _configfds[INUM_LIMIT + ONUM_LIMIT];
extern uint8_t _inputL;
extern uint8_t _inputR;
extern bool _input_empty;
extern uint8_t _outputL;
extern uint8_t _outputR;
extern bool _output_empty;
extern unsigned char _comm_Inum;
extern unsigned char _comm_Onum;
extern pthread_mutex_t _receive_control_msg_mutex;

static int check_afi_ready(int slot) {
  struct fpga_mgmt_image_info info = {0};
  int rc;

  rc = fpga_mgmt_describe_local_image(SLOT_ID, &info, 0);
  fail_on(
      rc, out,
      "Unable to get AFI information from slot %d. Are you running as root?",
      SLOT_ID);

  if (info.status != FPGA_STATUS_LOADED) {
    rc = 1;
    fail_on(rc, out, "AFI in Slot %d is not in READY state !", SLOT_ID);
  }

  printf("AFI PCI  Vendor ID: 0x%x, Device ID 0x%x\n",
         info.spec.map[FPGA_APP_PF].vendor_id,
         info.spec.map[FPGA_APP_PF].device_id);

  if (info.spec.map[FPGA_APP_PF].vendor_id != _pci_vendor_id ||
      info.spec.map[FPGA_APP_PF].device_id != _pci_device_id) {
    printf("AFI does not show expected PCI vendor id and device ID. If the AFI "
           "was just loaded, it might need a rescan. Rescanning now.\n");

    rc = fpga_pci_rescan_slot_app_pfs(SLOT_ID);
    fail_on(rc, out, "Unable to update PF for slot %d", SLOT_ID);
    rc = fpga_mgmt_describe_local_image(SLOT_ID, &info, 0);
    fail_on(rc, out, "Unable to get AFI information from slot %d", SLOT_ID);

    printf("AFI PCI  Vendor ID: 0x%x, Device ID 0x%x\n",
           info.spec.map[FPGA_APP_PF].vendor_id,
           info.spec.map[FPGA_APP_PF].device_id);

    if (info.spec.map[FPGA_APP_PF].vendor_id != _pci_vendor_id ||
        info.spec.map[FPGA_APP_PF].device_id != _pci_device_id) {
      rc = 1;
      fail_on(rc, out,
              "The PCI vendor id and device of the loaded AFI are not "
              "the expected values.");
    }
  }

  return rc;

out:
  return 1;
}

static int init_fpga() {
  int rc;
  rc = fpga_pci_init();
  fail_on(rc, out, "Unable to initialize the fpga_pci library");
  rc = check_afi_ready(SLOT_ID);
  fail_on(rc, out, "AFI not ready");
  return rc;

out:
  exit(-1);
}

void execute_program(void (*func)()) {
  int i;
  int rc = init_fpga();
  _pci_bar_handle = PCI_BAR_HANDLE_INIT;
  rc = fpga_pci_attach(0, FPGA_APP_PF, APP_PF_BAR0, 0, &_pci_bar_handle);
  fail_on(rc, out, "Unable to attach to the AFI on slot id %d", SLOT_ID);
  _bar = fpga_pci_bar_get(_pci_bar_handle);
  (*func)();
out:
  pthread_mutex_destroy(&_receive_control_msg_mutex);
  for (i = 0; i < INUM_LIMIT + ONUM_LIMIT; i++) {
    if (_configfds[i] > 0) {
      close(_configfds[i]);
    }
  }
  if (_pci_bar_handle >= 0) {
    rc = fpga_pci_detach(_pci_bar_handle);
    if (rc) {
      printf("Failure while detaching from the fpga.\n");
    }
  }
  if (rc != 0) {
    exit(-1);
  }
}

static void *allocate_kernel_buf(int *configfd) {
  void *address;
  *configfd = open("/dev/fpga_dma", O_RDWR);
  if (*configfd < 0) {
    perror("Error in dma driver.");
    exit(-1);
  }
  address =
      mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *configfd, 0);
  if (address == MAP_FAILED) {
    perror("Mmap operation failed.");
    exit(-1);
  }
  return address;
}

inline void send_control_msg(uint32_t tag, uint32_t msg) {
  volatile uint32_t *reg_ptr = (uint32_t *)(_bar->mem_base) + tag;
  *reg_ptr = msg;
}

inline uint32_t receive_control_msg(uint32_t tag) {
  volatile uint32_t *reg_ptr = (uint32_t *)(_bar->mem_base) + tag;
  //  pthread_mutex_lock(&_receive_control_msg_mutex);
  uint32_t ret = *reg_ptr;
  //  pthread_mutex_unlock(&_receive_control_msg_mutex);
  return ret;
}

inline bool init_buf_pools(int Inum, int Onum) {
  _comm_Inum = Inum;
  _comm_Onum = Onum;
  assert(Inum <= INUM_LIMIT);
  assert(Onum <= ONUM_LIMIT);
  int i;
  for (i = 0; i < Inum; i++) {
    _IBufs[i] = allocate_kernel_buf(&_configfds[i]);
    _IBufs_phy[i] = *((uint64_t *)_IBufs[i]);
  }
  for (i = 0; i < Onum; i++) {
    _OBufs[i] = allocate_kernel_buf(&_configfds[i + Inum]);
    _OBufs_phy[i] = *((uint64_t *)_OBufs[i]);
  }
  _inputL = 0;
  _inputR = 0;
  _input_empty = false;
  _outputL = 0;
  _outputR = 0;
  _output_empty = true;
  send_control_msg(PUSH_BUF_INIT_DATA_REG, Inum);
  send_control_msg(PUSH_BUF_INIT_DATA_REG, Onum);
  for (i = 0; i < Inum; i++) {
    send_control_msg(PUSH_BUF_INIT_DATA_REG, _IBufs_phy[i] & 0xFFFFFFFF);
    send_control_msg(PUSH_BUF_INIT_DATA_REG, _IBufs_phy[i] >> 32);
  }
  for (i = 0; i < Onum; i++) {
    send_control_msg(PUSH_BUF_INIT_DATA_REG, _OBufs_phy[i] & 0xFFFFFFFF);
    send_control_msg(PUSH_BUF_INIT_DATA_REG, _OBufs_phy[i] >> 32);
  }
  while (!receive_control_msg(PULL_BUF_INIT_READY_REG)) {
  }
  return true;
}

inline void *get_free_buf() {
  while (_inputL == _inputR && _input_empty) {
    uint32_t input_reg = receive_control_msg(PULL_INPUT_REG);
    if ((input_reg & 0x1) == 1) {
      continue;
    } else {
      input_reg >>= 1;
      _input_empty = false;
    }
    _inputR = input_reg & 0xFF;
    _inputL = (input_reg >> 8) & 0xFF;
  }
  void *ret = _IBufs[_inputL++];
  if (_inputL == _comm_Inum) {
    _inputL = 0;
  }
  _input_empty = (_inputL == _inputR);
  return ret;
}

inline void commit_buf() { send_control_msg(PUSH_INPUT_REG, 0); }

inline void *get_data_buf() {
  while (_outputL == _outputR && _output_empty) {
    uint32_t output_reg = receive_control_msg(PULL_OUTPUT_REG);
    if ((output_reg & 0x1) == 1) {
      continue;
    } else {
      output_reg >>= 1;
      _output_empty = false;
    }
    _outputR = output_reg & 0xFF;
    _outputL = (output_reg >> 8) & 0xFF;
  }
  void *ret = _OBufs[_outputL++];
  if (_outputL == _comm_Onum) {
    _outputL = 0;
  }
  _output_empty = (_outputR == _outputL);
  return ret;
}

inline void release_buf() { send_control_msg(PUSH_OUTPUT_REG, 0); }

#endif
