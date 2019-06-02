#ifndef APP_SW_CPP_
#define APP_SW_CPP_

#include "sw.h"
#include <insider_kernel.h>
#include <string.h>

typedef ap_uint<2> uint2_t;
typedef ap_uint<1> uint1_t;

typedef struct _pe {
  short d;
  short p;
} pe;

void initPE(pe *pex) {
#pragma HLS PIPELINE
  for (int i = 0; i < MAXPE; i++) {
    pex[i].d = 0;
    pex[i].p = 0;
  }
}

#ifdef _COMPUTE_FULL_MATRIX
short **localMat;
static short colIter = 0;
#endif

void updatePE(pe *pex, uint2_t d, uint2_t q, short n, short nw, short w,
              short r, short c) {
#pragma HLS PIPELINE
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
#ifdef _COMPUTE_FULL_MATRIX
  localMat[r][colIter * MAXPE + c] = max;
#endif
}

void executePE(short r, short c, pe *pex, pe *ppex, uint2_t *d, uint2_t *q) {
#pragma HLS PIPELINE
  short nw, w, n;

  if (r == 0) {
    n = 0;
    nw = 0;
  } else {
    n = pex->p;
    nw = ppex->d;
  }
  w = ppex->p;
  uint2_t d1 = d[c];
  uint2_t q1 = q[r];
  updatePE(pex, d1, q1, n, nw, w, r, c);
}

void executeFirstPE(short r, short c, pe *p, uint2_t *d, uint2_t *q, short nw,
                    short w) {
#pragma HLS PIPELINE
  short n;
  if (r == 0) {
    n = 0;
  } else {
    n = p->p;
  }
  uint2_t d1 = d[c];
  uint2_t q1 = q[r];
  updatePE(p, d1, q1, n, nw, w, r, c);
}

template <int FACTOR>
void swCoreB(uint2_t *d, uint2_t *q, short *maxr, short *maxc, short *maxv,
             short *iterB, pe *myPE, short stripe, short rows) {
#pragma HLS inline
#pragma HLS array partition variable = d cyclic factor = FACTOR
  int i, loop;
  short w = 0; // Initial condition at the start of a row
  d += stripe * MAXPE;
  initPE(myPE);
  for (loop = 0; loop < rows; ++loop) {
#pragma HLS PIPELINE
    short rowmaxv = MINVAL;
    short rowmaxpe = 0;
    for (i = 0; i < MAXPE; i++) {
#pragma HLS inline region recursive
      if (i == 0) {
        short nw = w;
        w = (stripe == 0) ? 0 : iterB[loop];
        executeFirstPE(loop, i, &myPE[i], d, q, nw, w);
      } else {
        executePE(loop, i, &myPE[i], &myPE[i - 1], d, q);
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
void swSystolicBlocking(uint2_t d[MAXCOL], uint2_t q[MAXROW], short *maxr,
                        short *maxc, short *maxv, short rows, short cols) {
  pe myPE[MAXPE];
  short iterB[MAXROW];
#pragma HLS inline
#pragma HLS RESOURCE variable = iterB core = RAM_S2P_LUTRAM
#pragma HLS RESOURCE variable = q core = RAM_S2P_LUTRAM
  *maxc = MINVAL;
  *maxv = MINVAL;
  *maxr = MINVAL;
  short stripes = MAXCOL / MAXPE;
  // assert(stripes <= (MAXCOL + MAXPE - 1) / MAXPE);
  // assert(rows <= MAXROW);
#pragma HLS array partition variable = myPE
  for (short stripe = 0; stripe < stripes; stripe = stripe + 1) {
#ifdef _COMPUTE_FULL_MATRIX
    colIter = stripe;
#endif
    swCoreB<MAXPE>(d, q, maxr, maxc, maxv, iterB, myPE, stripe, rows);
  }
}

void simpleSW(uint2_t refSeq[MAXCOL], uint2_t readSeq[MAXROW], short *maxr,
              short *maxc, short *maxv) {
#pragma HLS inline region off
  *maxv = MINVAL;
  int row, col;
  short mat[MAXROW][MAXCOL];
  for (col = 0; col < MAXCOL; col++) {
    short d = refSeq[col];
    for (row = 0; row < MAXROW; ++row) {
      short n, nw, w;
      if (row == 0) {
        n = 0;
      } else {
        n = mat[row - 1][col];
      }
      if (col == 0) {
        w = 0;
      } else {
        w = mat[row][col - 1];
      }
      if (row > 0 && col > 0) {
        nw = mat[row - 1][col - 1];
      } else {
        nw = 0;
      }
      short q = readSeq[row];
      short max = 0;
      short match = (d == q) ? MATCH : MISS_MATCH;
      short t1 = (nw + match > max) ? nw + match : max;
      short t2 = (n + GAP > w + GAP) ? n + GAP : w + GAP;
      max = t1 > t2 ? t1 : t2;
      mat[row][col] = max;
      if (max > *maxv) {
        *maxv = max;
        *maxr = row;
        *maxc = col;
      }
    }
  }
}

void sw(uint2_t d[MAXCOL], uint2_t q[MAXROW], short *maxr, short *maxc,
        short *maxv) {
#pragma HLS inline region off
  swSystolicBlocking(d, q, maxr, maxc, maxv, MAXROW, MAXCOL);
}

template <int BUFFERSZ>
void intTo2bit(unsigned int *buffer, uint2_t *buffer2b) {
  int i, j;
#pragma HLS PIPELINE
  for (i = 0; i < BUFFERSZ; ++i) {
    for (j = 0; j < 16; ++j) {
      buffer2b[16 * i + j] = (buffer[i] & (3 << (2 * j))) >> (2 * j);
    }
  }
}

template <int FACTOR>
void swInt(unsigned int *readRefPacked, short *maxr, short *maxc, short *maxv) {
#pragma HLS function_instantiate variable = maxv
  uint2_t d2bit[MAXCOL];
  uint2_t q2bit[MAXROW];
#pragma HLS array partition variable = d2bit cyclic factor = FACTOR
#pragma HLS array partition variable = q2bit cyclic factor = FACTOR

  intTo2bit<MAXCOL / 16>((readRefPacked + MAXROW / 16), d2bit);
  intTo2bit<MAXROW / 16>(readRefPacked, q2bit);
  sw(d2bit, q2bit, maxr, maxc, maxv);
}

void swMaxScore(unsigned int readRefPacked[NUMPACKED][PACKEDSZ],
                short out[NUMPACKED][3]) {
  /*instantiate NUMPACKED PE*/
  for (int i = 0; i < NUMPACKED; ++i) {
#pragma HLS UNROLL
    swInt<MAXPE>(readRefPacked[i], &out[i][0], &out[i][1], &out[i][2]);
  }
}

void sw_maxscore(unsigned int loop_cnt, ST_Queue<APP_Data> &app_input_data,
                 ST_Queue<APP_Data> &app_output_data) {
  unsigned int inbuf[PACKEDSZ * NUMPACKED];
  unsigned int outbuf[3 * NUMPACKED];
  unsigned int readRefPacked[NUMPACKED][PACKEDSZ];
  short out[NUMPACKED][3];
  int numIter;
#pragma HLS array partition variable = readRefPacked dim = 1
#pragma HLS array partition variable = out dim = 0
  APP_Data buffered_output;

  unsigned int input_data[PACKEDSZ * NUMPACKED];
#pragma HLS array_partition variable = input_data complete dim = 0
  for (unsigned int loop = 0; loop < loop_cnt; loop++) {
  app_input_read_loop:
    for (int i = 0; i < UINTSZ * PACKEDSZ * NUMPACKED / 64; i++) {
#pragma HLS pipeline
      APP_Data input;
      app_input_data.read(input);
      for (int j = 0; j < 16; j++) {
#pragma HLS unroll
        input_data[i * 16 + j] = input.data(j * 32 + 31, j * 32);
      }
    }
  input_copy_j_loop:
    for (int j = 0; j < PACKEDSZ; j++) {
    input_copy_i_loop:
      for (int i = 0; i < NUMPACKED; i++) {
#pragma HLS unroll
        readRefPacked[i][j] = input_data[i * PACKEDSZ + j];
      }
    }
    swMaxScore(readRefPacked, out);
    /*PE OUT to outbuf*/
    for (int i = 0; i < NUMPACKED; ++i) {
#pragma HLS PIPELINE
      outbuf[3 * i] = out[i][0];
      outbuf[3 * i + 1] = out[i][1];
      outbuf[3 * i + 2] = out[i][2];
    }
    short flatten_out[NUMPACKED * 3];
#pragma HLS array partition variable = flatten_out complete dim = 0
  flatten_out_i_loop:
    for (int i = 0; i < NUMPACKED; i++) {
#pragma HLS unroll
    flatten_out_j_loop:
#pragma HLS unroll
      for (int j = 0; j < 3; j++) {
        flatten_out[i * 3 + j] = out[i][j];
      }
    }

    bool eop = (loop == loop_cnt - 1);
    if (loop % 2 == 0) {
      APP_Data out;
      out.eop = 0;
      out.len = 64;
    write_output_loop_1:
      for (int i = 0; i < 32; i++) {
#pragma HLS unroll
        out.data(i * 16 + 15, i * 16) = flatten_out[i];
      }
      app_output_data.write(out);
      buffered_output.len = 32;
    write_output_loop_2:
      for (int i = 0; i < 16; i++) {
#pragma HLS unroll
        buffered_output.data(i * 16 + 15, i * 16) = flatten_out[i + 32];
      }
      if (eop) {
        buffered_output.eop = true;
        app_output_data.write(buffered_output);
      }
    } else {
    write_output_loop_3:
      for (int i = 0; i < 16; i++) {
#pragma HLS unroll
        buffered_output.data((i + 16) * 16 + 15, (i + 16) * 16) =
            flatten_out[i];
      }
      buffered_output.eop = false;
      buffered_output.len = 64;
      app_output_data.write(buffered_output);
      APP_Data out;
      out.eop = eop;
      out.len = 64;
    write_output_loop_4:
      for (int i = 0; i < 32; i++) {
#pragma HLS unroll
        out.data(i * 16 + 15, i * 16) = flatten_out[i + 16];
      }
      app_output_data.write(out);
    }
  }
}

void app_sw(ST_Queue<bool> &reset_app_sw,
            ST_Queue<unsigned int> &app_input_params,
            ST_Queue<APP_Data> &app_input_data,
            ST_Queue<APP_Data> &app_output_data) {

  bool reset = false;
  unsigned reset_cnt = 0;
  while (1) {
    bool dummy;
    if (reset || (reset = reset_app_sw.read_nb(dummy))) {
      unsigned int dummy0;
      app_input_params.read_nb(dummy0);
      reset_cnt++;
      if (reset_cnt == RESET_CNT) {
        reset_cnt = 0;
        reset = false;
      }
    } else {
      unsigned int loop_cnt;
      if (app_input_params.read_nb(loop_cnt)) {
        sw_maxscore(loop_cnt, app_input_data, app_output_data);
      }
    }
  }
}

#endif
