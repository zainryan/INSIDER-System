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
#include <fcntl.h>

// Vivado does not support svGetScopeFromName
//#ifdef INCLUDE_DPI_CALLS
#ifndef VIVADO_SIM
#include "svdpi.h"
#endif
//#endif

#include "insider_cosim.h"
#include "sh_dpi_tasks.h"

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

#define DATA_SIZE (4096) // in B
#define WRITE_BUF_SIZE (2 * 1024 * 1024)
#define MAX_INFLATE_RATIO (256 / 2)
#define MAX_OUTPUT_SIZE (MAX_INFLATE_RATIO * DATA_SIZE)

ssize_t expected_len = 0;

unsigned char input_data[DATA_SIZE];
unsigned char expected_result[MAX_OUTPUT_SIZE];
unsigned char real_file_buf[MAX_OUTPUT_SIZE];

unsigned long long extents_start_idx[] = {0};
unsigned long long extents_len[] = {MAX_OUTPUT_SIZE};
unsigned int num_extents = sizeof(extents_len) / sizeof(unsigned long long);

void gen_data() {
  unsigned char *input_data_ptr = input_data;
  for (int i = 0; i < DATA_SIZE / 2; i++) {
    unsigned char number = (unsigned char)(rand() % 30);
    *(input_data_ptr++) = number;
    unsigned char alpha = (unsigned char)(rand() % 26 + 'a');
    *(input_data_ptr++) = alpha;
  }
}

void host_decoder() {
  memset(expected_result, 0, sizeof(expected_result));
  unsigned char *expected_ptr = expected_result;
  for (int i = 0; i < DATA_SIZE; i += 2) {
    int len = input_data[i];
    unsigned char alpha = input_data[i + 1];
    for (int j = 0; j < len; j++) {
      *(expected_ptr++) = alpha;
      expected_len++;
    }
  }
}

void user_simulation_function() {
  // PUT YOUR CODE HERE
  gen_data();
  host_decoder();
  memset(real_file_buf, 0, sizeof(real_file_buf));
  set_physical_file(real_file_buf, num_extents, extents_start_idx, extents_len);

  int fd = vopen("cosim_phy_file", O_WRONLY);
  puts("Finish vopen...");
  unsigned long long write_bytes = 0;

  cosim_wait(300);

  while (write_bytes != DATA_SIZE) {
    write_bytes +=
        vwrite(fd, input_data + write_bytes, DATA_SIZE - write_bytes);
    cosim_wait(50);
    puts("vwrite()...");
    printf("write_bytes = %d\n", write_bytes);
  }
  puts("Before vclose()..");
  vclose(fd);
  puts("After vclose()..");

  get_physical_file(real_file_buf, num_extents, extents_start_idx, extents_len);

  for (int i = 0; i < expected_len; i++) {
    if (expected_result[i] != real_file_buf[i]) {
      printf("Mismatch at i = %d, expected = %d, real = %d\n", i,
             (int)expected_result[i], (int)real_file_buf[i]);
      exit(-1);
    }
  }

  printf("expected_len = %d\n", expected_len);
  puts("Passed!");
}
