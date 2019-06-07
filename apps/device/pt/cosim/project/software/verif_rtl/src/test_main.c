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

#define DATA_SIZE (4096)
#define WRITE_DATA_SIZE (512)

unsigned long long extents_start_idx[] = {4096 * 11};
unsigned long long extents_len[] = {4096};
unsigned int num_extents = sizeof(extents_len) / sizeof(unsigned long long);

unsigned char input[DATA_SIZE];
unsigned char output[DATA_SIZE];

void generate_input() {
  for (int i = 0; i < DATA_SIZE; i++) {
    input[i] = rand();
  }
}

void user_simulation_function() {
  generate_input();
  puts("generate_input()...");
  set_physical_file(input, num_extents, extents_start_idx, extents_len);
  puts("set_physical_file_list()...");

  int fd = vopen("cosim_phy_file", O_WRONLY);
  puts("Finish vopen...");
  int write_bytes = 0;

  cosim_wait(350);

  while (write_bytes != WRITE_DATA_SIZE) {
    write_bytes += vwrite(fd, input + write_bytes, WRITE_DATA_SIZE - write_bytes);
    cosim_wait(50);
    puts("vwrite()...");
    printf("write_bytes = %d\n", write_bytes);
  }
  puts("before vclose()");
  vclose(fd);
  puts("after vclose()");
  get_physical_file(output, num_extents, extents_start_idx, extents_len);
  for (int i = 0; i < DATA_SIZE; i++) {
    if (input[i] != output[i]) {
      printf("Mismatch i = %d, input[i] = %d, output[i] = %d\n", i,
             (int)input[i], (int)output[i]);
      exit(-1);
    }
  }

  puts("PASSED!");
}
