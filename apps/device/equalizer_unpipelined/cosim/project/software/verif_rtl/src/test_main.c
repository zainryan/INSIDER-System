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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Vivado does not support svGetScopeFromName
//#ifdef INCLUDE_DPI_CALLS
#ifndef VIVADO_SIM
#include "svdpi.h"
#endif
//#endif

#include "sh_dpi_tasks.h"
#include "insider_cosim.h"
#include "../../../../inc/equalizer.h"

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

#define IMAGE_HEIGHT_PIXELS (64)

ushort input_data[IMAGE_HEIGHT_PIXELS / 32][IMAGE_WIDTH_PIXELS][64 / 2];
ushort real_output_data[IMAGE_HEIGHT_PIXELS / 32][IMAGE_WIDTH_PIXELS][64 / 2];
ushort expected_output_data[IMAGE_HEIGHT_PIXELS / 32][IMAGE_WIDTH_PIXELS][64 / 2];

#define READ_BUF_SIZE (128)

void generate_input() {
  for (int i = 0; i < IMAGE_HEIGHT_PIXELS / 32; i++) {
    for (int j = 0; j < IMAGE_WIDTH_PIXELS; j++) {
      for (int k = 0; k < 32; k++) {
        input_data[i][j][k] = (ushort)rand();
      }
    }
  }
}

ushort eq_func(ushort val, ushort min) {
  uint eq_val;
  if (val == 0) {
    eq_val = 0;
  } else {
    eq_val = val - min;
    eq_val *= (IMAGE_WIDTH_PIXELS - 1);
    eq_val /= (1 + IMAGE_WIDTH_PIXELS - min);
  }
  return eq_val;
}

void run_standard_version() {
  static ushort line[32][IMAGE_WIDTH_PIXELS];
  static ushort hist[32][HISTOGRAM_DEPTH];
  static ushort old[32];
  static ushort acc[32];
  static ushort pline[32];
  static ushort max[32];
  static ushort min[32];
  static ushort cdf[32][HISTOGRAM_DEPTH];
  static ushort bucket_out[32];

  for (uint n = 0; n < IMAGE_HEIGHT_PIXELS / 32; n++) {
    for (uint i = 0; i < HISTOGRAM_DEPTH; i++) {
      for (uint k = 0; k < 32; k++) {
        hist[k][i] = 0;
      }
    }

    for (uint i = 0; i < IMAGE_WIDTH_PIXELS; i++) {
      for (int k = 0; k < 32; k++) {
        pline[k] = input_data[n][i][k];
      }

      if (i == 0) {
        for (uint k = 0; k < 32; k++) {
          old[k] = 0;
          acc[k] = 0;
        }
      }

      for (uint k = 0; k < 32; k++) {        
        ushort val = (pline[k] & (((1 << HISTOGRAM_BITS) - 1) << 4)) >> 4;
        line[k][i] = val;

        if (old[k] == val) {
          acc[k]++;
        } else {
          hist[k][old[k]] = acc[k];
          acc[k] = hist[k][val] + 1;
        }

        old[k] = val;
      }
    }

    /* CDF */
    for (uint i = 0; i < HISTOGRAM_DEPTH; i++) {
      for (int j = 0; j < 32; j++) {
        if (i == 0) {
          max[j] = 0;
          min[j] = IMAGE_WIDTH_PIXELS;
          hist[j][old[j]] = acc[j];
        }
        ushort val = hist[j][i];

        if (val != 0 && val < min[j]) {
          min[j] = val;
        }

        max[j] += val;

        cdf[j][i] = max[j];
      }
    }

    for (uint i = 0; i < IMAGE_WIDTH_PIXELS; i++) {

      for (uint k = 0; k < 32; k++) {
        ushort val = line[k][i];
        bucket_out[k] = eq_func(cdf[k][val], min[k]) << 4;
      }

      for (int k = 0; k < 32; k++) {
        expected_output_data[n][i][k] = bucket_out[k];
      }
    }
  }
}

void user_simulation_function() {
  generate_input();
  puts("Finish generating input...");
  run_standard_version();
  puts("Finish running standard version...");
  send_input_param(IMAGE_HEIGHT_PIXELS);
  puts("Finish sending input param...");

  unsigned long long extents_start_idx = 0;
  unsigned long long extents_len = sizeof(input_data);
  set_physical_file((unsigned char *)input_data, 1, &extents_start_idx, 
		    &extents_len);
  puts("Finish setting physical file...");

  int fd = vopen("cosim_phy_file", 0);
  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  int fin_file = 0;

  unsigned char *output = (unsigned char *)(&real_output_data[0][0][0]);
  int total_read_bytes = 0;

  cosim_wait(400);

  while (!fin_file) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int tmp = vread(fd, buf + read_bytes, READ_BUF_SIZE - read_bytes);
      if (!tmp) {
        fin_file = 1;
        break;
      } else {
        read_bytes += tmp;
      }
    }
    memcpy(output, buf, read_bytes);
    output += read_bytes;
    total_read_bytes += read_bytes;
  }
  puts("Finish reading virtual file...");
  printf("Total read bytes = %d\n", total_read_bytes);
  printf("Expected total read bytes = %d\n", sizeof(expected_output_data));
  assert(total_read_bytes == sizeof(expected_output_data));
  vclose(fd);

  for (int i = 0; i < IMAGE_HEIGHT_PIXELS / 32; i++) {
    for (int j = 0; j < IMAGE_WIDTH_PIXELS; j++) {
      for (int k = 0; k < 32; k++) {
        if (real_output_data[i][j][k] != expected_output_data[i][j][k]) {
          printf("Mismatch at i = %d, j = %d, k = %d!\n", i, j, k);
          for (int t = 0; t < 32; t++) {
            printf("real_output_data[%d][%d][%d] = %d\n", i, j, t,
                   real_output_data[i][j][t]);
          }
          for (int t = 0; t < 32; t++) {
            printf("expected_output_data[%d][%d][%d] = %d\n", i, j, t,
                   expected_output_data[i][j][t]);
          }
          exit(-1);
        }
      }
    }
  }
  puts("PASSED!");
}
