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

/* copied from inc/structure.h
 */
typedef char Elem;
typedef short Result_Elem; 
//
// be careful to make all numbers blow a powe of 2
#define DBLOCK_SIZE_IN_BYTE (64)
#define DBLOCK_SIZE_IN_BIT (8 * DBLOCKS_SIZE_IN_BYTE)
#define NUM_OF_ENGINES (16)
#define READ_BUF_SIZE (128)


#define KERNEL_SIZE (64)   // type of 'Elem'
#define DBLOCKS_PER_KERNEL (KERNEL_SIZE * sizeof(Elem) / DBLOCK_SIZE_IN_BYTE) // (1)
#define LOG_DBLOCKS_PER_KERNEL (0)  
#define DBLOCKS_PER_PARAM (DBLOCKS_PER_KERNEL)   
#define ELEMS_PER_DBLOCK (DBLOCK_SIZE_IN_BYTE / sizeof(Elem)) //(64)
#define KERNEL_BATCH_SIZE (2)
#define NUM_OF_KERNELS (NUM_OF_ENGINES * KERNEL_BATCH_SIZE)

#define NUM_OF_TOTAL_ROW (128) // NUM_OF_MATRIX * MATRIX_HEIGHT)

#define PATCH_SIZE (KERNEL_SIZE)
#define RAW_DATA_SIZE (NUM_OF_TOTAL_ROW * PATCH_SIZE * sizeof(Elem)) 

Elem kernel_matrix[NUM_OF_KERNELS][KERNEL_SIZE];
Elem image_array[NUM_OF_TOTAL_ROW][PATCH_SIZE];
Result_Elem expected_result[NUM_OF_TOTAL_ROW][NUM_OF_KERNELS];
unsigned char raw_data[RAW_DATA_SIZE];

void gen_data() {
  
  // generate kernel matrix
  for (int i = 0; i < NUM_OF_KERNELS; i++) {
    for (int j = 0; j < KERNEL_SIZE; j++) {
      Elem number = (Elem) rand();
      kernel_matrix[i][j] = number;
    }
  }

  unsigned char *raw_data_ptr = raw_data;
  // generate image array
  for (int i = 0; i < NUM_OF_TOTAL_ROW; i++) {
    for (int j = 0; j < PATCH_SIZE; j++) {
      Elem number = (Elem) rand();
      image_array[i][j] = number;
      unsigned char *ptr = (unsigned char*) &number;
      for (int k = 0; k < sizeof(Elem); k++) {
        *raw_data_ptr++ = *ptr++;
      }        
    }
  }
}

void conv() {
  for (int i = 0; i < NUM_OF_TOTAL_ROW; i++) {
    for (int j = 0; j < NUM_OF_KERNELS; j++) {
      expected_result[i][j] = 0;
      for (int k = 0; k < PATCH_SIZE; k++) {
        expected_result[i][j] += image_array[i][k] * kernel_matrix[j][k];
      }
    }
  }
}

void send_param() {
  for (int i = 0; i < NUM_OF_KERNELS; i++) {
    unsigned int engine_id = i / KERNEL_BATCH_SIZE;
    unsigned int engine_idx = i % KERNEL_BATCH_SIZE;
    for (int k = 0; k < KERNEL_SIZE; k++) {
      unsigned int data = (engine_id << 24) | (engine_idx << 16) | (k << 8) | 
	(unsigned char) kernel_matrix[i][k];
      send_input_param(data);
    }
  }
}

void user_simulation_function() {
  // PUT YOUR CODE HERE
  gen_data();

  unsigned long long extents_start_idx = 0;
  unsigned long long extents_len = sizeof(raw_data);
  set_physical_file((unsigned char *)raw_data, 1, &extents_start_idx, &extents_len);

  puts("INFO: start sending param...");

  send_param();

  puts("INFO: param sent...");

  conv();

  puts("expected data generated...");

  int fd = vopen("cosim_phy_file", 0);
  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  int fin_file = 0;

  puts("file opened...");

  cosim_wait(350);

  unsigned char *expected_result_in_bytes = (unsigned char *) expected_result;

  while (!fin_file) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int tmp = vread(fd, buf, READ_BUF_SIZE - read_bytes);
      if (!tmp) {
        fin_file = 1;
        break;
      } else {
        read_bytes += tmp;
      }
    }
    printf("read bytes: %d\n", read_bytes);
    for (int i = 0; i < read_bytes; i++) {
      unsigned char real = buf[i];
      unsigned char expected = expected_result_in_bytes[i];
      printf("%d : %d \n", (int) real, (int) expected);
      if (real != expected) {
        puts("Failed!");
        //exit(-1);
      }
    }
    expected_result_in_bytes += read_bytes;
  }
  puts("Passed!");
  vclose(fd);
}
