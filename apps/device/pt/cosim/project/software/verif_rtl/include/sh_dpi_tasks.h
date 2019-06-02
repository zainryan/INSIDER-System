// Amazon FPGA Hardware Development Kit
//
// Copyright 2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
//
// Licensed under the Amazon Software License (the "License"). You may not use
// this file except in compliance with the License. A copy of the License is
// located at
//
//    http://aws.amazon.com/asl/
//
// or in the "license" file accompanying this file. This file is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express or
// implied. See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SH_DPI_TASKS
#define SH_DPI_TASKS

#include "uthash.h"
#include <stdarg.h>

struct hash_entry {
  uint64_t addr; /* key */
  uint8_t data;
  UT_hash_handle hh; /* makes this structure hashable */
};

struct hash_entry *pcie_space = NULL; /* important! initialize to NULL */

extern void sv_printf(char *msg);
extern void sv_map_host_memory(uint8_t *memory);

extern void cl_peek(uint64_t addr, uint32_t *data);
extern void cl_poke(uint64_t addr, uint32_t data);
extern void sv_pause(uint32_t x);

void test_main(uint32_t *exit_code);

void host_memory_putc(uint64_t addr, uint8_t data) {
  struct hash_entry *s;
  HASH_FIND(hh, pcie_space, &addr, sizeof(uint64_t), s);
  if (s) {
    HASH_DEL(pcie_space, s);
  } else {
    s = malloc(sizeof(struct hash_entry));
  }
  s->addr = addr;
  s->data = data;
  HASH_ADD(hh, pcie_space, addr, sizeof(uint64_t), s);
}

uint8_t host_memory_getc(uint64_t addr) {
  struct hash_entry *s;
  HASH_FIND(hh, pcie_space, &addr, sizeof(uint64_t), s);
  uint8_t ret;
  if (!s) {
    ret = 0;
  } else {
    ret = s->data;
  }
  return ret;
}

void log_printf(const char *format, ...) {
  static char sv_msg_buffer[256];
  va_list args;

  va_start(args, format);
  vsprintf(sv_msg_buffer, format, args);
  sv_printf(sv_msg_buffer);

  va_end(args);
}

#define LOW_32b(a) ((uint32_t)((uint64_t)(a)&0xffffffff))
#define HIGH_32b(a) ((uint32_t)(((uint64_t)(a)) >> 32L))

#endif
