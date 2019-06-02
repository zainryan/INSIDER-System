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
#include <string.h>
#include <unistd.h>

// Vivado does not support svGetScopeFromName
//#ifdef INCLUDE_DPI_CALLS
#ifndef VIVADO_SIM
#include "svdpi.h"
#endif
//#endif

#include "insider_cosim.h"
#include "sh_dpi_tasks.h"

// BUS_WIDTH % sizeof(Point) should be 0.
#define BUS_WIDTH (64)
#define STENCIL_WIDTH (3)
#define STENCIL_HEIGHT (3)
#define PICTURE_WIDTH (256)
#define PICTURE_HEIGHT (32)
/* #define PICTURE_WIDTH (1920) */
/* #define PICTURE_HEIGHT (1080) */
// PADDING_WIDTH should be larger than STENCIL_WIDTH.
// (PADDING_WIDTH + PICTURE_WIDTH) % (BUS_WIDTH / sizeof(Point)) should be 0.
// 4B padding in left and 4B padding in right.
#define PADDING_WIDTH (0)
#define PADDING_HEIGHT (STENCIL_HEIGHT - 1)
#define NUM_POINT_PER_INPUT (BUS_WIDTH / sizeof(Point))
#define READ_BUF_SIZE (128)

typedef struct Point {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
} Point;

Point stencil[STENCIL_HEIGHT][STENCIL_WIDTH];
Point input_pictures[PADDING_HEIGHT + PICTURE_HEIGHT]
                    [PADDING_WIDTH + PICTURE_WIDTH];
Point expected_output_pictures[PICTURE_HEIGHT][PICTURE_WIDTH];
Point real_output_pictures[PADDING_HEIGHT + PICTURE_HEIGHT]
                          [PADDING_WIDTH + PICTURE_WIDTH];

inline Point host_uint_to_point(unsigned int uint) {
#pragma HLS INLINE
  Point point;
  point.red = (uint >> 0) & 0xFFFFFFFF;
  point.green = (uint >> 8) & 0xFFFFFFFF;
  point.blue = (uint >> 16) & 0xFFFFFFFF;
  point.alpha = (uint >> 24) & 0xFFFFFFFF;
  return point;
}

inline unsigned int host_point_to_uint(const Point point) {
#pragma HLS INLINE
  unsigned int uint = 0;
  uint |= ((unsigned int)point.red) << 0;
  uint |= ((unsigned int)point.green) << 8;
  uint |= ((unsigned int)point.blue) << 16;
  uint |= ((unsigned int)point.alpha) << 24;
  return uint;
}

void generate_input() {
  for (int i = 0; i < STENCIL_HEIGHT; i++) {
    for (int j = 0; j < STENCIL_WIDTH; j++) {
      stencil[i][j] = host_uint_to_point(rand());
    }
  }
  memset(input_pictures, 0, sizeof(input_pictures));
  for (int i = 0; i < PADDING_HEIGHT + PICTURE_HEIGHT; i++) {
    for (int j = 0; j < PADDING_WIDTH + PICTURE_WIDTH; j++) {
      if (i >= PADDING_HEIGHT && j >= PADDING_WIDTH) {
        input_pictures[i][j] = host_uint_to_point(rand());
      }
    }
  }
}

Point add(const Point a, const Point b) {
  Point c;
  c.red = a.red + b.red;
  c.green = a.green + b.green;
  c.blue = a.blue + b.blue;
  c.alpha = a.alpha + b.alpha;
  return c;
}

Point mult(const Point a, const Point b) {
  Point c;
  c.red = a.red * b.red;
  c.green = a.green * b.green;
  c.blue = a.blue * b.blue;
  c.alpha = a.alpha * b.alpha;
  return c;
}

void run_standard_version() {
  memset(expected_output_pictures, 0, sizeof(expected_output_pictures));
  for (int i = 0; i < PICTURE_HEIGHT; i++) {
    for (int j = 0; j < PICTURE_WIDTH; j++) {
      for (int k = 0; k < STENCIL_HEIGHT; k++) {
        for (int p = 0; p < STENCIL_WIDTH; p++) {
          expected_output_pictures[i][j] = add(
              expected_output_pictures[i][j],
              mult(
                  stencil[k][p],
                  input_pictures[i + PADDING_HEIGHT - (STENCIL_HEIGHT - 1 - k)]
                                [j + PADDING_WIDTH - (STENCIL_WIDTH - 1 - p)]));
        }
      }
    }
  }
}

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

void user_simulation_function() {
  generate_input();
  puts("Finish generating input...");
  run_standard_version();
  puts("Finish running standard version...");
  for (int i = 0; i < STENCIL_HEIGHT; i++) {
    for (int j = 0; j < STENCIL_WIDTH; j++) {
      unsigned int uint = host_point_to_uint(stencil[i][j]);
      send_input_param(uint);
    }
  }
  puts("Finish sending input param...");

  unsigned long long extents_start_idx = 0;
  unsigned long long extents_len = sizeof(input_pictures);
  set_physical_file((unsigned char *)input_pictures, 1, &extents_start_idx, 
		    &extents_len);
  puts("Finish setting physical file...");

  int fd = vopen("cosim_phy_file", O_RDONLY);
  puts("Finish vopen...");

  cosim_wait(350);

  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  int fin_file = 0;
  unsigned char *output = (unsigned char *)(&real_output_pictures[0][0]);
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
      printf("Read bytes = %d\n", read_bytes);
    }
    memcpy(output, buf, read_bytes);
    output += read_bytes;
    total_read_bytes += read_bytes;
    printf("Total read bytes = %d\n", total_read_bytes);
  }
  puts("Finish reading virtual file...");
  printf("Total read bytes = %d\n", total_read_bytes);
  printf("Expected total read bytes = %d\n", sizeof(real_output_pictures));
  assert(total_read_bytes == sizeof(real_output_pictures));
  vclose(fd);

  for (int i = 0; i < 16; i++) {
    printf("expected_output_pictures[0][%d] = %d, %d, %d, %d\n", i,
           (int)expected_output_pictures[0][i].red,
           (int)expected_output_pictures[0][i].green,
           (int)expected_output_pictures[0][i].blue,
           (int)expected_output_pictures[0][i].alpha);
  }

  for (int i = 0; i < 16; i++) {
    printf(
        "real_output_pictures[0 + PADDING_HEIGHT][%d + PADDING_WIDTH] = %d, "
        "%d, %d, %d\n",
        i, (int)real_output_pictures[0 + PADDING_HEIGHT][i + PADDING_WIDTH].red,
        (int)real_output_pictures[0 + PADDING_HEIGHT][i + PADDING_WIDTH].green,
        (int)real_output_pictures[0 + PADDING_HEIGHT][i + PADDING_WIDTH].blue,
        (int)real_output_pictures[0 + PADDING_HEIGHT][i + PADDING_WIDTH].alpha);
  }

  for (int i = 0; i < PICTURE_HEIGHT; i++) {
    for (int j = 0; j < PICTURE_WIDTH; j++) {
      Point expected, real;
      expected = expected_output_pictures[i][j];
      real = real_output_pictures[i + PADDING_HEIGHT][j + PADDING_WIDTH];
      if (host_point_to_uint(expected) != host_point_to_uint(real)) {
        printf("Failed at i = %d, j = %d\n", i, j);
        exit(-1);
      }
    }
  }
  puts("PASSED!");
}
