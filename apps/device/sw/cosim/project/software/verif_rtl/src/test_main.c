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
#include <assert.h>

// Vivado does not support svGetScopeFromName
//#ifdef INCLUDE_DPI_CALLS
#ifndef VIVADO_SIM
#include "svdpi.h"
#endif
//#endif

#include "../../../../inc/sw.h"
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


#define READ_BUF_SIZE (128)
#define ITER_CNT (5)

unsigned int input_data[ITER_CNT][NUMPACKED][PACKEDSZ];
short expected_output_data[ITER_CNT][3][NUMPACKED];
short real_output_data[ITER_CNT][3][NUMPACKED];

typedef struct _pe {
  short d;
  short p;
} pe;

void generate_input() {
  for (int i = 0; i < ITER_CNT; i++) {
    for (int j = 0; j < NUMPACKED; j++) {
      for (int k = 0; k < PACKEDSZ; k++) {
        input_data[i][j][k] = rand();
      }
    }
  }
}

void standard_updatePE(pe *pex, short d, short q, short n, short nw, short w,
                       short r, short c) {
  short max = 0;
  short match = (d == q) ? MATCH : MISS_MATCH;
  short x1 = nw + match;
  short t1 = (x1 > max) ? x1 : max;
  short x2 = w + GAP;
  short t2 = (x2 > t1) ? x2 : t1;
  short x3 = n + GAP;
  max = (x3 > t2) ? x3 : t2;
  pex->p = max;
  pex->d = n;
}

void standard_executeFirstPE(short r, short c, pe *p, short *d, short *q,
                             short nw, short w) {
  short n;
  if (r == 0) {
    n = 0;
  } else {
    n = p->p;
  }
  short d1 = d[c];
  short q1 = q[r];
  standard_updatePE(p, d1, q1, n, nw, w, r, c);
}

void standard_executePE(short r, short c, pe *pex, pe *ppex, short *d, short *q) {
  short nw, w, n;

  if (r == 0) {
    n = 0;
    nw = 0;
  } else {
    n = pex->p;
    nw = ppex->d;
  }
  w = ppex->p;
  short d1 = d[c];
  short q1 = q[r];
  standard_updatePE(pex, d1, q1, n, nw, w, r, c);
}

void standard_initPE(pe *pex) {
  for (int i = 0; i < MAXPE; i++) {
    pex[i].d = 0;
    pex[i].p = 0;
  }
}

void standard_swCoreB(short *d, short *q, short *maxr, short *maxc,
                      short *maxv, short *iterB, pe *myPE, short stripe,
                      short rows) {
  int i, loop;
  short w = 0; // Initial condition at the start of a row
  d += stripe * MAXPE;
  standard_initPE(myPE);
  for (loop = 0; loop < rows; ++loop) {
    short rowmaxv = MINVAL;
    short rowmaxpe = 0;
    for (i = 0; i < MAXPE; i++) {
      if (i == 0) {
        short nw = w;
        w = (stripe == 0) ? 0 : iterB[loop];
        standard_executeFirstPE(loop, i, &myPE[i], d, q, nw, w);
      } else {
        standard_executePE(loop, i, &myPE[i], &myPE[i - 1], d, q);
      }
      if (i == MAXPE - 1) {
        iterB[loop] = myPE[i].p;
      }
      if (myPE[i].p > rowmaxv) {
        rowmaxv = myPE[i].p;
        rowmaxpe = i;
      }
    }

    if (rowmaxv > *maxv) {
      *maxv = rowmaxv;
      *maxc = rowmaxpe + stripe * MAXPE; // log2(MAXPE);
      *maxr = loop;
    }
  }
}

/*Only columns*/
void standard_swSystolicBlocking(short d[MAXCOL], short q[MAXROW],
                                 short *maxr, short *maxc, short *maxv,
                                 short rows, short cols) {
  pe myPE[MAXPE];
  short iterB[MAXROW];
  *maxc = MINVAL;
  *maxv = MINVAL;
  *maxr = MINVAL;
  short stripes = MAXCOL / MAXPE;
  for (short stripe = 0; stripe < stripes; stripe = stripe + 1) {
    standard_swCoreB(d, q, maxr, maxc, maxv, iterB, myPE, stripe, rows);
  }
}

void standard_sw(short d[MAXCOL], short q[MAXROW], short *maxr, short *maxc,
                 short *maxv) {
  standard_swSystolicBlocking(d, q, maxr, maxc, maxv, MAXROW, MAXCOL);
}

void standard_intTo2bit(int buffersz, unsigned int *buffer, short *buffer2b) {
  int i, j;
  for (i = 0; i < buffersz; ++i) {
    for (j = 0; j < 16; ++j) {
      buffer2b[16 * i + j] = (buffer[i] & (3 << (2 * j))) >> (2 * j);
    }
  }
}

void standard_swInt(unsigned int *readRefPacked, short *maxr, short *maxc,
                    short *maxv) {
  short d2bit[MAXCOL];
  short q2bit[MAXROW];
  standard_intTo2bit(MAXCOL / 16, (readRefPacked + MAXROW / 16), d2bit);
  standard_intTo2bit(MAXROW / 16, readRefPacked, q2bit);
  standard_sw(d2bit, q2bit, maxr, maxc, maxv);
}

void standard_swMaxScore(unsigned int readRefPacked[NUMPACKED][PACKEDSZ],
                         short out[NUMPACKED][3]) {
  /*instantiate NUMPACKED PE*/
  for (int i = 0; i < NUMPACKED; ++i) {
    standard_swInt(readRefPacked[i], &out[i][0], &out[i][1], &out[i][2]);
  }
}

void standard_sw_maxscore(unsigned int *input, short *output, int *size) {
  static unsigned int inbuf[PACKEDSZ * NUMPACKED];
  static short outbuf[3 * NUMPACKED];
  static unsigned int readRefPacked[NUMPACKED][PACKEDSZ];
  static short out[NUMPACKED][3];
  int numIter;
  numIter = *size;
  int loop = 0;
  for (loop = 0; loop < numIter; loop++) {
    memcpy(readRefPacked, (unsigned int *)(input + loop * PACKEDSZ * NUMPACKED),
           UINTSZ * PACKEDSZ * NUMPACKED);
    standard_swMaxScore(readRefPacked, out);

    for (int i = 0; i < NUMPACKED; ++i) {
      outbuf[3 * i] = out[i][0];
      outbuf[3 * i + 1] = out[i][1];
      outbuf[3 * i + 2] = out[i][2];
    }

    memcpy((unsigned int *)(output + 3 * NUMPACKED * loop), outbuf,
           sizeof(short) * 3 * NUMPACKED);
  }
}

void run_standard_version() {
  int size = ITER_CNT;
  standard_sw_maxscore(&input_data[0][0][0], &expected_output_data[0][0][0],
                       &size);
}

void user_simulation_function() {
  generate_input();
  puts("Finish generating input...");
  run_standard_version();
  puts("Finish running standard version...");
  send_input_param(ITER_CNT);
  puts("Finish sending input param...");
  unsigned long long extents_start_idx = 0;
  unsigned long long extents_len = sizeof(input_data);
  set_physical_file((unsigned char *)input_data, 1, &extents_start_idx, &extents_len);
  puts("Finish setting physical file...");


  int fd = vopen("cosim_phy_file", 0);
  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  int fin_file = 0;

  cosim_wait(900);

  unsigned char *output = (unsigned char *)(&real_output_data[0][0][0]);
  int total_read_bytes = 0;
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
  printf("Expected total bytes = %d\n", sizeof(expected_output_data));
  assert(total_read_bytes == sizeof(expected_output_data));
  vclose(fd);

  for (int i = 0; i < ITER_CNT; i++) {
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < NUMPACKED; k++) {
        if (expected_output_data[i][j][k] != real_output_data[i][j][k]) {
          printf("Mismatch at i = %d, j = %d, k = %d\n", i, j, k);
          exit(-1);
        }
      }
    }
  }

  puts("PASSED!");
}
