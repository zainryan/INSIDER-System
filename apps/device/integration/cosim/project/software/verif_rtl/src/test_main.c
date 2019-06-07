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

#define FILE_ROW_NUM (100)
#define FILE_COL_NUM (64) // do not change it
#define APP_QUERY_THRES (1)
#define APP_RECORD_THRES (1)
#define APP_QUERY_LENGTH (32)
#define APP_RECORD_LENGTH (FILE_COL_NUM)
#define READ_BUF_SIZE (2 * 1024 * 1024)

unsigned char query[32];
unsigned char raw_data[FILE_COL_NUM * FILE_ROW_NUM];
unsigned char expected_output_buf[FILE_ROW_NUM * APP_RECORD_LENGTH];

char random_char() {
  int min_ascii = 65;
  int max_ascii = 90;
  return min_ascii + rand() % (max_ascii - min_ascii + 1);
}

void integration() {
  int i;
  int j;
  int record_idx;
  unsigned char *expected_buf_ptr = expected_output_buf;
  unsigned char *record = raw_data;
  int numOfMatch = 0;
  char match = 0;
  for (record_idx = 0; record_idx < FILE_ROW_NUM; record_idx++) {
    match = 0;
    for (i = 0; i < APP_QUERY_THRES; i++) {
      for (j = 0; j < APP_RECORD_THRES; j++) {
        if (query[i] == record[j]) {
          match = 1;
        }
      }
    }
    if (match) {
      int overlap = 0;
      for (i = 0; i < APP_QUERY_LENGTH; i++) {
        for (j = 0; j < APP_QUERY_LENGTH; j++) {
          if (query[i] == record[j]) {
            overlap++;
          }
        }
      }
      if (overlap > APP_QUERY_LENGTH - APP_QUERY_THRES) {
        numOfMatch++;
        for (i = 0; i < APP_RECORD_LENGTH; i++) {
          *expected_buf_ptr++ = record[i];
        }
      }
    }
    record += APP_RECORD_LENGTH;
  }
  printf("expected number of matches is: %d ( %d in total)", numOfMatch,
         FILE_ROW_NUM);
}

int gen_data() {
  int i;
  int j;
  for (i = 0; i < FILE_ROW_NUM * FILE_COL_NUM; i++) {
    raw_data[i] = random_char();
  }
  for (i = 0; i < 32; i++) {
    query[i] = random_char();
  }
}
void user_simulation_function() {
  // PUT YOUR CODE HERE
  gen_data();
  puts("input data generated...");
  int i;
  int j;
  unsigned long long extents_start_idx = 0;
  unsigned long long extents_len = sizeof(raw_data);
  set_physical_file((unsigned char *)raw_data, 1, &extents_start_idx, &extents_len);
  puts("set_physical_file() ends...");

  send_input_param(((APP_RECORD_THRES << 16) | (APP_QUERY_THRES)));
  unsigned int *query_ptr_in_4B = (unsigned int *)query;
  for (i = 0; i < 8; i++) {
    send_input_param(query_ptr_in_4B[i]);
  }

  puts("send input params ends...");

  integration();

  puts("expected data generated...");

  int fd = vopen("cosim_phy_file", 0);
  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  char fin_file = 0;

  puts("file opened...");

  /*
    Wait for a while to ensure that the underlying RTL code has finished
    executing. Keep poking signals to Insider system to prevent the simulation
    from being terminated (since the simulation framework would be terminated if
    there is no active signal in the I/O interface).
   */
  cosim_wait(350);

  unsigned char *expected_output_buf_in_bytes =
      (unsigned char *)expected_output_buf;

  while (!fin_file) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      puts("before vread");
      int tmp = vread(fd, buf, READ_BUF_SIZE - read_bytes);
      printf("after vread len = %d\n", tmp);
      if (!tmp) {
        fin_file = 1;
        break;
      } else {
        read_bytes += tmp;
      }
    }
    printf("read bytes %d\n", read_bytes);
    for (i = 0; i < read_bytes; i++) {
      unsigned char real = buf[i];
      unsigned char expected = expected_output_buf_in_bytes[i];
      // cout << real << " : " << expected << endl;
      if (real != expected) {
        puts("Failed!");
        exit(-1);
      }
    }
    expected_output_buf_in_bytes += read_bytes;
  }
  puts("Passed!");
  vclose(fd);
}
