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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Vivado does not support svGetScopeFromName
//#ifdef INCLUDE_DPI_CALLS
#ifndef VIVADO_SIM
#include "svdpi.h"
#endif
//#endif

#include "sh_dpi_tasks.h"
#include "insider_cosim.h"

void test_main(uint32_t *exit_code) {

// Vivado does not support svGetScopeFromName
//#ifdef INCLUDE_DPI_CALLS
#ifndef VIVADO_SIM
  svScope scope;
#endif
  //#endif

  uint32_t rdata;

// Vivado does not support svGetScopeFromName
//#ifdef INCLUDE_DPI_CALLS
#ifndef VIVADO_SIM
  scope = svGetScopeFromName("tb");
  svSetScope(scope);
#endif

  simulator();

  *exit_code = 0;
}

#define input_size (1024)
const long long READ_BUF_SIZE = 64;
char input[input_size];
char output[input_size];

void user_simulation_function() {
  for (int i = 0; i < input_size; i++) {
    input[i] = i % 2;
  }
  send_input_param(input_size >> 32);
  send_input_param(input_size & 0xFFFFFFFF);
  puts("Finish sending input param...");
  memset(input, 1, sizeof(input));
  unsigned long long extents_start_idx = 0;
  unsigned long long extents_len = sizeof(input);
  set_physical_file(input, 1, &extents_start_idx, &extents_len);
  puts("Finish setting physical file...");

  int fd = vopen("cosim_phy_file", 0);
  puts("Finish vopen...");
  unsigned char *buf = (unsigned char *)malloc(input_size);
  int fin_file = 0;

  cosim_wait(300);

  int total_read_bytes = 0;
  while (!fin_file) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      puts("Before vread...");
      int tmp = vread(fd, buf + read_bytes, READ_BUF_SIZE - read_bytes);
      puts("After vread...");
      if (!tmp) {
        fin_file = 1;
        break;
      } else {
        read_bytes += tmp;
      }
    }
    printf("Read bytes = %d\n", read_bytes);
    total_read_bytes += read_bytes;
  }
  puts("Finish reading virtual file...");
  printf("Original input bytes = %d\n", input_size);
  printf("Total read bytes = %d\n", total_read_bytes);
  vclose(fd);
  puts("PASSED!");
}
