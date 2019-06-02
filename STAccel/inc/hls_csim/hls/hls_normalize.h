/*****************************************************************************
 *
 *     Author: Xilinx, Inc.
 *
 *     This text contains proprietary, confidential information of
 *     Xilinx, Inc. , is distributed by under license from Xilinx,
 *     Inc., and may be used, copied and/or disclosed only pursuant to
 *     the terms of a valid license agreement with Xilinx, Inc.
 *
 *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
 *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
 *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
 *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
 *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
 *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
 *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
 *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
 *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
 *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
 *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
 *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE.
 *
 *     Xilinx products are not intended for use in life support appliances,
 *     devices, or systems. Use in such applications is expressly prohibited.
 *
 *     (c) Copyright 2012 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/**
 * @file hls_normalize.h
 */

#ifndef X_HLS_NORMALIZE_H
#define X_HLS_NORMALIZE_H

/* @{ */

/**
 * This function returns the number of 'redundant sign bits' in a
 * signed number.  This is equivalent to the gcc __builtin_clrsb(),
 * but synthesizable.
 *
 * @param value The input
 */
template <int W, int I, ap_q_mode Q, ap_o_mode O>
unsigned int clrsb(ap_fixed<W, I, Q, O> value) {
  ap_uint<W> x = value(W - 1, 0);
  if (value[W - 1]) {
    x = ~x;
  }
  x = x << 1 | 0x1;
  assert((int)x.countLeadingZeros() <= W);
  assert((int)x.countLeadingZeros() >= 0);
  return x.countLeadingZeros();
}

/**
 * This function returns the number of 'redundant sign bits' in a
 * signed number.  This is equivalent to the gcc __builtin_clrsb(),
 * but synthesizable.
 *
 * @param value The input
 */
template <int W> unsigned int clrsb(ap_int<W> value) {
  ap_uint<W> x = value(W - 1, 0);
  if (value[W - 1]) {
    x = ~x;
  }
  x = x << 1 | 0x1;
  assert((int)x.countLeadingZeros() <= W);
  assert((int)x.countLeadingZeros() >= 0);
  return x.countLeadingZeros();
}

/**
 * This function returns the number of 'redundant sign bits' in a
 * signed number.  This is equivalent to the gcc __builtin_clrsb(),
 * but synthesizable.
 *
 * @param value The input
 */
static unsigned int clrsb(int value) {
#pragma HLS INLINE SELF
  return clrsb(ap_int<32>(value));
}

template <int Z, int W, int I> unsigned int clz(ap_ufixed<W, I> x) {
  ap_uint<Z + 1> t;
  t(Z, 1) = x(x.wl() - 1, x.wl() - Z);
  t[0] = 1;

  return t.countLeadingZeros();
}

static unsigned int clz(int value) {
  ap_int<32> x = value;
  return x.countLeadingZeros();
}

template <int W> unsigned int clz(ap_int<W> value) {
  return value.countLeadingZeros();
}

template <int W> unsigned int clz(ap_uint<W> value) {
  return value.countLeadingZeros();
}

/**
 * This function returns the number of 'redundant sign bits' in a
 * signed number.  This is equivalent to the gcc __builtin_clrsb(),
 * but synthesizable.  The result of left-shifting the input by the
 * result will be a Q31 format number in [-1,-0.5) or (0.5, 1).
 *
 * @param x The input
 */
static unsigned int find_normalization(int x) { return clrsb(x); }

/**
 * This function returns the number of 'redundant sign bits' in a
 * signed number.  This is equivalent to the gcc __builtin_clrsb(),
 * but synthesizable.    The result of left-shifting the input by the
 * result will be a Q15 format number in [-1,-0.5) or (0.5, 1).
 *
 * @param x The input
 */
static unsigned int find_normalization(short x) {
  // Set the low order bits to the opposite of the sign bit to
  // ensure that the case of zero results in a shift of 15.
  int bits;
  if (x < 0) {
    bits = 0;
  } else {
    bits = 0xFFFF;
  }
  return clrsb(((int)x) << 16 | bits);
}

#endif

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
