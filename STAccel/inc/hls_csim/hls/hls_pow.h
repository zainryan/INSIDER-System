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
 *     (c) Copyright 2012-2016 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/**
 * @file hls_pow.h
 *
 * see: de Dinechin, "Floating-Point Exponentiation Units for Reconfigurable
 * Computing"
 */
namespace pow_reduce {

#include "hls_pow_tables.h"

template <typename LOG_TYPE, int p, int alpha, int size, int sizeout>
void log_range_reduce(ap_ufixed<size, -p> zN, LOG_TYPE &log,
                      ap_ufixed<sizeout, -(p + alpha - 1)> &zN1) {
  //#pragma HLS inline self off
  const int T1size = 1 << alpha; // The size of the first table
#define log_lut log_lut_table<LOG_TYPE, p, alpha, T1size>::array

  //#pragma HLS RESOURCE variable=log_lut core=ROM_1P_LUTRAM

  ap_ufixed<alpha, -p> a = zN;                // Extract alpha MSBs from z1.
  ap_ufixed<size - alpha, -p - alpha> b = zN; // Extract the LSBs from z1.
  ap_ufixed<size + p + 2 * p + 1, 1> zNext = 1 + zN; // Extend z1 for shift
  int e_shift =
      (p != 4 || a[a.wl() - 1] == 1) ? 2 * p : 2 * p + 1; // Stage dependent?
  ap_ufixed<size + p + 1, 1 - 2 *p> eZ = zNext >> e_shift;

  zN1 = ((b + eZ) - a * zN);
  ap_uint<alpha> index = a(alpha - 1, 0);
  log = log_lut[index];

#undef log_lut
}

//   i  0  1  2  3   4   5   6   7   8
// =====================================
//  pi  0  4  7  12  17  22  27  32  37
const int p0 = 0;     // We start with zero zeroed bits
const int alpha0 = 5; // 1+alpha0 = number of bits considered by stage 0.
const int p1 =
    (alpha0 == 5) ? 4 : (alpha0 - 2); // The number of bits zeroed in y1.
const int alpha1 = 4;
const int p2 = p1 + alpha1 - 1;
const int alpha2 = 6;
const int p3 = p2 + alpha2 - 1;
const int alpha3 = 6;
const int p4 = p3 + alpha3 - 1;
const int alpha4 = 6;
const int p5 = p4 + alpha4 - 1;
const int alpha5 = 6;
const int p6 = p5 + alpha5 - 1;
const int alpha6 = 6;
const int p7 = p6 + alpha6 - 1;
const int alpha7 = 6;
const int p8 = p7 + alpha7 - 1;

template <typename T> class pow_traits {};

template <> class pow_traits<half> {
public:
  const static int we = fp_struct<half>::EXP_BITS;
  const static int wf = 18;
  const static int org_wf = fp_struct<half>::SIG_BITS; // 10

  const static int log_gbits = 4;
  const static int LogMaxPrecision = p3 + wf + 1 + log_gbits;  // 35
  typedef ap_fixed<1 + we + LogMaxPrecision, 1 + we> LOG_TYPE; // < 41, 6 >

  const static int w1 = wf + 1;
  const static int w2 =
      w1 + 2 * p1 + 1 -
      (alpha1 - 1); // MaxWidth of z2, needs to be truncted to MaxPrecision
  const static int w3 = w2 + 2 * p2 + 1 - (alpha2 - 1);

  template <int size>
  static ap_ufixed<LogMaxPrecision - p3, -p3>
  log_range_reduction(ap_ufixed<size, -p1> z1, LOG_TYPE &log_sum) {
    ap_ufixed<(w2 < LogMaxPrecision - p2) ? w2 : (LogMaxPrecision - p2), -p2>
        z2;
    ap_ufixed<(w3 < LogMaxPrecision - p3) ? w3 : (LogMaxPrecision - p3), -p3>
        z3;
    LOG_TYPE logn;
    log_range_reduce<LOG_TYPE, p1, alpha1>(z1, logn, z2);
    log_sum += logn;
    log_range_reduce<LOG_TYPE, p2, alpha2>(z2, logn, z3);
    log_sum += logn;
    return z3;
  }

  const static int exp_gbits = 3;
  const static int exp_gbits_Z2 = 3;
  const static int w_Z1 = 9;

  // input ap_ufixed < 4, -9 >
  // output ap_uifxed < 5, -8 >
  static ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1>
  exp_Z1P_m_1(ap_ufixed<org_wf + exp_gbits - w_Z1, -w_Z1> Z1P) {
    ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> Z1P_l = Z1P;
    return Z1P_l;
  }
};
template <> class pow_traits<float> {
public:
  const static int we = fp_struct<float>::EXP_BITS;
  const static int wf = 34;
  const static int org_wf = fp_struct<float>::SIG_BITS; // 23

  const static int log_gbits = 4;
  const static int LogMaxPrecision = p4 + wf + 1 + log_gbits;  // 56
  typedef ap_fixed<1 + we + LogMaxPrecision, 1 + we> LOG_TYPE; // < 65, 9 >

  const static int w1 = wf + 1;
  const static int w2 =
      w1 + 2 * p1 + 1 -
      (alpha1 - 1); // MaxWidth of z2, needs to be truncted to MaxPrecision
  const static int w3 = w2 + 2 * p2 + 1 - (alpha2 - 1);
  const static int w4 = w3 + 2 * p3 + 1 - (alpha3 - 1);

  template <int size>
  static ap_ufixed<LogMaxPrecision - p4, -p4>
  log_range_reduction(ap_ufixed<size, -p1> z1, LOG_TYPE &log_sum) {
    ap_ufixed<(w2 < LogMaxPrecision - p2) ? w2 : (LogMaxPrecision - p2), -p2>
        z2;
    ap_ufixed<(w3 < LogMaxPrecision - p3) ? w3 : (LogMaxPrecision - p3), -p3>
        z3;
    ap_ufixed<(w4 < LogMaxPrecision - p4) ? w4 : (LogMaxPrecision - p4), -p4>
        z4;
    LOG_TYPE logn;
    log_range_reduce<LOG_TYPE, p1, alpha1>(z1, logn, z2);
    log_sum += logn;

    log_range_reduce<LOG_TYPE, p2, alpha2>(z2, logn, z3);
    log_sum += logn;

    log_range_reduce<LOG_TYPE, p3, alpha3>(z3, logn, z4);
    log_sum += logn;

    return z4;
  }

  const static int exp_gbits = 4;
  const static int exp_gbits_Z2 = 3;
  const static int w_Z1 = 9;

  // input ap_ufixed < 18, -9 >
  // output ap_ufixed < 18, -8 >
  // Z = Z1 + Z1P = Z1 + Z2
  // e^Z1P - 1 = Z1P + Z2^2/2 = Z1P + f(Z2)
  // w_Z2_ind = w - w/2 - w1 = 5
  static ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1>
  exp_Z1P_m_1(ap_ufixed<org_wf + exp_gbits - w_Z1, -w_Z1> Z1P) {
    const static int w_Z2_ind = 5;
    ap_uint<w_Z2_ind> Z2_ind = Z1P(Z1P.wl() - 1, Z1P.wl() - w_Z2_ind);
    ap_ufixed<org_wf + exp_gbits - 2 * w_Z1 - 1, -2 *w_Z1 - 1> f_Z2 =
        table_f_Z2<ap_ufixed<org_wf + exp_gbits, 0>>::array[Z2_ind]; // < 8, -19
                                                                     // >
    ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
        Z1P + f_Z2;
    return exp_Z1P_m_1;
  }
};

template <> class pow_traits<double> {
public:
  const static int we = fp_struct<double>::EXP_BITS;
  const static int wf = 66;
  const static int org_wf = fp_struct<double>::SIG_BITS; // 52

  const static int log_gbits = 5;
  const static int LogMaxPrecision = p8 + wf + 1 + log_gbits;  // 109
  typedef ap_fixed<1 + we + LogMaxPrecision, 1 + we> LOG_TYPE; // < 121, 12>

  const static int w1 = wf + 1;
  const static int w2 =
      w1 + 2 * p1 + 1 -
      (alpha1 - 1); // MaxWidth of z2, needs to be truncted to MaxPrecision
  const static int w3 = w2 + 2 * p2 + 1 - (alpha2 - 1);
  const static int w4 = w3 + 2 * p3 + 1 - (alpha3 - 1);
  const static int w5 = w4 + 2 * p4 + 1 - (alpha4 - 1);
  const static int w6 = w5 + 2 * p5 + 1 - (alpha5 - 1);
  const static int w7 = w6 + 2 * p6 + 1 - (alpha6 - 1);
  const static int w8 = w7 + 2 * p7 + 1 - (alpha7 - 1);

  template <int size>
  static ap_ufixed<LogMaxPrecision - p8, -p8>
  log_range_reduction(ap_ufixed<size, -p1> z1, LOG_TYPE &log_sum) {
    ap_ufixed<(w2 < LogMaxPrecision - p2) ? w2 : (LogMaxPrecision - p2), -p2>
        z2;
    ap_ufixed<(w3 < LogMaxPrecision - p3) ? w3 : (LogMaxPrecision - p3), -p3>
        z3;
    ap_ufixed<(w4 < LogMaxPrecision - p4) ? w4 : (LogMaxPrecision - p4), -p4>
        z4;
    ap_ufixed<(w5 < LogMaxPrecision - p5) ? w5 : (LogMaxPrecision - p5), -p5>
        z5;
    ap_ufixed<(w6 < LogMaxPrecision - p6) ? w6 : (LogMaxPrecision - p6), -p6>
        z6;
    ap_ufixed<(w7 < LogMaxPrecision - p7) ? w7 : (LogMaxPrecision - p7), -p7>
        z7;
    ap_ufixed<(w8 < LogMaxPrecision - p8) ? w8 : (LogMaxPrecision - p8), -p8>
        z8;
    LOG_TYPE logn;

    log_range_reduce<LOG_TYPE, p1, alpha1>(z1, logn, z2);
    log_sum += logn;
    log_range_reduce<LOG_TYPE, p2, alpha2>(z2, logn, z3);
    log_sum += logn;
    log_range_reduce<LOG_TYPE, p3, alpha3>(z3, logn, z4);
    log_sum += logn;
    log_range_reduce<LOG_TYPE, p4, alpha4>(z4, logn, z5);
    log_sum += logn;
    log_range_reduce<LOG_TYPE, p5, alpha5>(z5, logn, z6);
    log_sum += logn;
    log_range_reduce<LOG_TYPE, p6, alpha6>(z6, logn, z7);
    log_sum += logn;
    log_range_reduce<LOG_TYPE, p7, alpha7>(z7, logn, z8);
    log_sum += logn;
    return z8;
  }

  // guard bits should be 7bit
  // but to reduce size of multiplier
  // use 7bit for exp_Z3_m_1 * exp_Z4_m_1 and exp_Z2_m_1 * exp_Z2P_m_1
  // use 5bit for exp_Z1 * exp_Z1P_m_1
  const static int exp_gbits = 7;
  const static int exp_gbits_Z2 = 5;
  const static int w_Z1 = 8;

  // input ap_ufixed < 51, -8 >
  // output ap_ufixed < 50, -7 >
  // Z = Z1 + Z2 + Z3 + Z4, w = 59, w1 = 8, w2 = 8, w3 = 8, w4 = 35
  static ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1>
  exp_Z1P_m_1(ap_ufixed<org_wf + exp_gbits - w_Z1, -w_Z1> Z1P) {
    const static int w_Z2 = 8;
    const static int w_Z2P = 43;
    const static int w_Z3 = 8;
    const static int w_Z4 = 35;

    ap_ufixed<w_Z2, -w_Z1> Z2 = Z1P;               // Z1P [ 50 .. 43 ]
    ap_ufixed<w_Z2P, -w_Z1 - w_Z2> Z2P = Z1P;      // Z1P [ 42 ..  0 ]
    ap_ufixed<w_Z3, -w_Z1 - w_Z2> Z3 = Z1P;        // Z1P [ 42 .. 35 ]
    ap_ufixed<w_Z4, -w_Z1 - w_Z2 - w_Z3> Z4 = Z1P; // Z1P [ 34 ..  0 ]

    // e^Z = e^Z1 * e^Z2 * e^Z3 * e^Z4
    //     = e^Z1 * ( 1 + Z2 + f(Z2) ) * ( 1 + Z3 + f(Z3) ) * ( 1 + Z4 + f(Z4) )
    //
    // 1. let's start from
    //   e^Z2P - 1
    // = ( 1 + Z3 + f(Z3) ) * ( 1 + Z4 + f(Z4) ) - 1
    // = ( Z3 + f(Z3) ) + ( Z4 + f(Z4) ) + ( Z3 + f(Z3) ) * ( Z4 + f(Z4) )
    //
    // 1.1 Z4 + f(Z4)
    // w_Z4_ind = w4 - w/2 = 6
    // instead of using a f_Z4 table, we reuse f_Z3 table for f_Z4
    const static int w_Z4_ind = w_Z3;
    ap_uint<w_Z4_ind> Z4_ind = Z4(Z4.wl() - 1, Z4.wl() - w_Z4_ind);
    ap_ufixed<org_wf + exp_gbits - 2 * (w_Z1 + w_Z2 + w_Z3) - 1,
              -2 * (w_Z1 + w_Z2 + w_Z3) - 1>
        f_Z4 = table_f_Z3<ap_ufixed<org_wf + exp_gbits, 0>>::array[Z4_ind] >>
               (2 * w_Z3); // < 10, -49 >
    ap_ufixed<org_wf + exp_gbits - w_Z1 - w_Z2 - w_Z3 + 1,
              -w_Z1 - w_Z2 - w_Z3 + 1>
        exp_Z4_m_1 = Z4 + f_Z4; // < 36, -23 >

    // 1.2 Z3 + f(Z3)
    ap_uint<w_Z3> Z3_ind = Z3(Z3.wl() - 1, 0);
    ap_ufixed<org_wf + exp_gbits - 2 * (w_Z1 + w_Z2) - 1,
              -2 * (w_Z1 + w_Z2) - 1>
        f_Z3 = table_f_Z3<
            ap_ufixed<org_wf + exp_gbits, 0>>::array[Z3_ind]; // < 26, -33 >
    ap_ufixed<org_wf + exp_gbits - w_Z1 - w_Z2 + 1, -w_Z1 - w_Z2 + 1>
        exp_Z3_m_1 = Z3 + f_Z3; // < 44, -15 >

    // 1.3 ( Z3 + f(Z3) ) * ( Z4 + f(Z4) )
    ap_ufixed<org_wf + exp_gbits - 2 * (w_Z1 + w_Z2) - w_Z3 + 2,
              -2 * (w_Z1 + w_Z2) - w_Z3 + 2>
        exp_Z2P_m_1_lo = exp_Z3_m_1 * exp_Z4_m_1; // < 21, -38 >

    // 1.4 e^Z2P - 1
    ap_ufixed<org_wf + exp_gbits - w_Z1 - w_Z2 + 2, -w_Z1 - w_Z2 + 2>
        exp_Z2P_m_1_l = exp_Z3_m_1 + exp_Z4_m_1 + exp_Z2P_m_1_lo; // < 45, -14 >
    assert(exp_Z2P_m_1_l[exp_Z2P_m_1_l.wl() - 1] == 0);
    ap_ufixed<org_wf + exp_gbits - w_Z1 - w_Z2 + 1, -w_Z1 - w_Z2 + 1>
        exp_Z2P_m_1 = exp_Z2P_m_1_l; // < 44, -15 >

    // 2. e^Z1P - 1
    //  = ( 1 + Z2 + f(Z2) ) * ( 1 + ( e^Z2P - 1 ) ) - 1
    //  = ( Z2 + f(Z2) ) + ( e^Z2P - 1 ) + ( Z2 + f(Z2) ) * ( e^Z2P - 1 )
    //
    // 2.1 Z2 + f(Z2)
    ap_uint<w_Z2> Z2_ind = Z2(Z2.wl() - 1, 0);
    ap_ufixed<org_wf + exp_gbits - 2 * w_Z1 - 1, -2 *w_Z1 - 1> f_Z2 =
        table_f_Z2<ap_ufixed<org_wf + exp_gbits, 0>>::array[Z2_ind]; // < 42,
                                                                     // -17 >
    ap_ufixed<org_wf + exp_gbits - w_Z1 + 1 - 2, -w_Z1 + 1> exp_Z2_m_1 =
        Z2 + f_Z2; // < 52, -7 > -> < 50, -7> to save multipliers

    // 2.2 ( Z2 + f(Z2) ) * ( e^Z2P - 1 )
    ap_ufixed<org_wf + exp_gbits - 2 * w_Z1 - w_Z2 + 2, -2 *w_Z1 - w_Z2 + 2>
        exp_Z1P_m_1_lo = exp_Z2_m_1 * exp_Z2P_m_1; // < 37, -22 >

    // 2.3 e^Z1P - 1
    ap_ufixed<org_wf + exp_gbits - w_Z1 + 2, -w_Z1 + 2> exp_Z1P_m_1_l =
        exp_Z2_m_1 + exp_Z2P_m_1 + exp_Z1P_m_1_lo; // < 53, -6 >
    assert(exp_Z1P_m_1_l[exp_Z1P_m_1_l.wl() - 1] == 0);
    ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
        exp_Z1P_m_1_l; // < 50, -7 >

    return exp_Z1P_m_1;
  }
};

template <typename T> T pow_generic(T base, T exp) {
#pragma HLS pipeline

  fp_struct<T> bs(base);
  fp_struct<T> es(exp);
  const static int we = pow_traits<T>::we;
  const static int wf = pow_traits<T>::wf;
  const static int org_wf = pow_traits<T>::org_wf;

  fp_struct<T> out;
  out.sign[0] = 0;
  out.sig = 0;

  int b_exp = bs.expv();
  int m_exp = es.expv();

  // special cases include:
  // x = 0, 1, -1, +inf, -inf, NaN
  // y = 0, +inf, -inf, NaN
  //
  //	y \ x	|	0	|	+1	|	-1	|	inf	|	NaN	|
  //normal
  // =============================================================================================================
  // 	0	|	1	|	1	|	1	|	1	|	1	|
  // 1
  // 	inf	|		|	1	|	1	|		|	NaN
  // |
  // 	NaN	|	NaN	|	1	|	NaN	|	NaN	|	NaN	|
  // NaN 	normal	|		|	1	|		|
  // |	NaN	|
  //
  // x < 0 and y is non int, r = NaN
  //
  bool y_is_0 = 0;
  bool x_is_1 = 0;
  bool x_is_p1 = 0;
  bool x_is_n1 = 0;
  bool y_is_inf = 0;
  bool y_is_NaN = 0;
  bool x_is_NaN = 0;
  bool x_is_0 = 0;
  bool x_is_inf = 0;
  bool x_is_neg = 0;
  bool y_is_int = 0; // not zero

#ifndef STDSUBNORMALS
  if (es.exp == 0)
#else
  if (es.exp == 0 && es.sig == 0)
#endif
    y_is_0 = 1;

  if (b_exp == 0 && bs.sig == 0)
    x_is_1 = 1;
  x_is_p1 = x_is_1 & ~bs.sign[0];
  x_is_n1 = x_is_1 & bs.sign[0];

  if (::hls::__isinf(exp))
    y_is_inf = 1;

  if (::hls::__isnan(exp))
    y_is_NaN = 1;
  if (::hls::__isnan(base))
    x_is_NaN = 1;

#ifndef STDSUBNORMALS
  if (bs.exp == 0)
#else
  if (bs.exp == 0 && bs.sig == 0)
#endif
    x_is_0 = 1;
  if (::hls::__isinf(base))
    x_is_inf = 1;

  if (bs.sign[0] == 1 && x_is_0 == 0 && x_is_inf == 0)
    x_is_neg = 1;

  fp_struct<T> nes = es;
#ifdef STDSUBNORMALS
  if (nes.exp == 0 && nes.sig != 0) {
    // subnormal handling.
    unsigned int zeros;
#pragma unroll
    for (zeros = 0; zeros < org_wf; zeros++)
      if (nes.sig[org_wf - zeros - 1] == 1)
        break;
    m_exp -= zeros;
    nes.sig = nes.sig << (zeros + 1); // add one so we shift off the leading one
  }
#endif

  if (m_exp >= org_wf)
    y_is_int = ~y_is_inf & ~y_is_NaN;
  else if (m_exp >= 0 && nes.sig(org_wf - 1 - m_exp, 0) == 0)
    y_is_int = 1;

  // priorities are different for different combinations:
  // 1. y = 0 || x = 1 || (x = -1 && y = +/-inf), r = 1
  if (y_is_0 | x_is_p1 | (x_is_n1 & y_is_inf)) {
    // out = 1
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_BIAS;
    return out.to_ieee();
  }
  // 2. y = NaN || x = NaN, r = NaN
  if (y_is_NaN | x_is_NaN | (x_is_neg & ~y_is_inf & ~y_is_int)) {
    // out = NaN
    // out.sign[0] = 0;
    out.sig = -1; // all 1's
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }

  // 3. r = +/-inf or +/-0
  //	y \ x	|	+0	|	-0	|	+inf	|	-inf	|	|x|>1	|
  //|x|<1
  // =============================================================================================================
  // 	+inf	|	0	|	0	|	+inf	|	+inf	|	+inf	|
  // 0
  // 	-inf	|	+inf	|	+inf	|	0	|	0	|	0	|
  // +inf
  //  pos non int	|	0	|	0	|	+inf	|	+inf
  //  | neg non int	|	+inf	|	+inf	|	0
  //  |	0	| pos odd int	|	0	|	-0	|       +inf |
  //  -inf    | pos even int|	0	|	0	|	+inf
  //  |	+inf	| neg odd int	|	+inf	|	-inf	|	0
  //  |	-0	| neg even int|	+inf	|	+inf	|	0	|
  //  0	|
  //
  bool y_is_pinf = 0;
  bool y_is_ninf = 0;
  bool x_abs_greater_1; // use for first 2 rows of the table only
  bool y_is_pos = 0;    // use for 1st and 3rd columns of the table only
  bool y_is_odd = 0;    // use for 2nd and 4th columns of the table only
  bool r_sign = 0;      // use to calculate sgn(x)^y

  y_is_pinf = y_is_inf & ~es.sign[0];
  y_is_ninf = y_is_inf & es.sign[0];

  if (b_exp >= 0)
    x_abs_greater_1 = 1;
  else
    x_abs_greater_1 =
        0; // not strongly correct, because +/-1 and NaN are not considered

  if (es.sign[0] == 0)
    y_is_pos =
        1; // not strongly correct, because 0, +/-inf and NaN are not considered

  // not strongly correct, assume y_is_int == 1 and m_exp >= 0
  if (m_exp == 0)
    y_is_odd = 1;
  else if (m_exp > 0 && m_exp <= org_wf)
    y_is_odd = nes.sig[org_wf - m_exp];

  r_sign = x_is_neg & y_is_odd;
  out.sign[0] = r_sign;

  if (x_is_n1) {
    out.exp = fp_struct<T>::EXP_BIAS;
    return out.to_ieee();
  }
  if ((y_is_pinf & x_abs_greater_1) | (y_is_ninf & ~x_abs_greater_1) |
      (x_is_0 & ~y_is_pos) | (x_is_inf & y_is_pos)) {
    // out = +/-inf
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if ((y_is_pinf & ~x_abs_greater_1) | (y_is_ninf & x_abs_greater_1) |
      (x_is_0 & y_is_pos) | (x_is_inf & ~y_is_pos)) {
    // out = +/-0
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // 1. log|base|
  const static int log_bypass_threshold = wf / 2;
  const static int LogMaxPrecision = pow_traits<T>::LogMaxPrecision;
  typedef typename pow_traits<T>::LOG_TYPE LOG_TYPE;
  LOG_TYPE sum;
  LOG_TYPE log_sum;

  // storing exp and frac of base using int b_exp and ap_ufixed<1+wf+1,1> b_frac
  // int b_exp = bs.expv();
  // initialization is necessary because we are extending the width of b_frac
  // from org_wf to wf
  ap_ufixed<1 + org_wf + 1, 1> b_frac = 0;

  fp_struct<T> nbs = bs;

#ifdef STDSUBNORMALS
  if (nbs.exp == 0 && nbs.sig != 0) {
    // subnormal handling.
    unsigned int zeros;
#pragma unroll
    for (zeros = 0; zeros < org_wf; zeros++)
      if (nbs.sig[org_wf - zeros - 1] == 1)
        break;
    b_exp -= zeros;
    nbs.sig =
        nbs.sig << (zeros + 1); // add one so we shift off the leading one.
  }
#else

#endif

  int FirstBit = nbs.sig[org_wf - 1];

  // Y is prenormalized, between 0.75 and 1.5
  b_frac[b_frac.wl() - b_frac.iwl()] = 1; // The implicit '1' in IEEE format.
  b_frac(b_frac.wl() - b_frac.iwl() - 1, 1) = nbs.sig(org_wf - 1, 0);

  ap_uint<1 + alpha0> index0 =
      b_frac(b_frac.wl() - b_frac.iwl() - 1,
             b_frac.wl() - b_frac.iwl() - (1 + alpha0));

  // the first two bits of b_frac can only be 01 or 10
  if (FirstBit) {
    b_frac = b_frac >> 1;
    b_exp += 1;
  }

  // const ap_ufixed<wf+we+2, 0> LOG2 = 0.69314718055994530941723212145818;
  const ap_ufixed<wf + we + 2, 0> LOG2_hi = 0.69314718055994517520446152047953;
  const ap_ufixed<wf + we + 2, 0> LOG2_lo = 0.60444058366692930952011528220776;
  const ap_ufixed<wf + we + 2, 0> LOG2 = LOG2_hi + (LOG2_lo >> 52);

  ap_fixed<1 + we + wf + we + 2, 1 + we> Elog2 = LOG2 * b_exp;

  const int T0size = 1 << (1 + alpha0); // The size of the first table

#define inverse_lut log_inverse_lut_table_pow<p0, alpha0, T0size>::array
#define log_lut log0_lut_table<LOG_TYPE, T, p0, alpha0, T0size>::array

  // ap_uint<1+alpha0> index0 = b_frac(b_frac.wl()-1, b_frac.wl()-(1+alpha0));
  ap_ufixed<1 + alpha0, 1> b_frac_tilde_inverse = inverse_lut[index0];

  log_sum = log_lut[index0];

  // The first bits of y1 are always a 1, followed by p1 zeros, but
  // it gets dropped later
  ap_ufixed<1 + org_wf + 1 + (1 + alpha0), 1> b_frac1 =
      b_frac * b_frac_tilde_inverse;
  // p_next = p1;

  assert(b_frac1(b_frac1.wl() - b_frac1.iwl() - 1,
                 b_frac1.wl() - b_frac1.iwl() - p1) ==
         0); // Check that the first stage zeros out p1 bits.

  const int z1_width = 1 + wf + 1 + (1 + alpha0) - p1 + 1;
  ap_ufixed<z1_width, -p1> z1 = b_frac1; // Extract a1+b1 from y1.

  ap_fixed<1 - log_bypass_threshold + 1 + LogMaxPrecision,
           -log_bypass_threshold + 1>
      zk;
  zk = pow_traits<T>::log_range_reduction(z1, log_sum);

  ap_fixed<1 - 2 * log_bypass_threshold + 1 + LogMaxPrecision,
           -log_bypass_threshold + 1>
      zk_trunc = zk;
  sum = zk - (zk_trunc * zk_trunc / 2);
  // sum = zk*2/(zk+2);

  ap_fixed<1 + we + wf, 1 + we> log_base = Elog2 + log_sum + sum;
#undef log_lut
#undef inverse_lut

  // 2. mult = m_frac * 2^m_exp = exp * log|base|
  // int m_exp = es.expv();
  ap_fixed<1 + 1 + org_wf, 1 + 1> e_frac = 0;

  // fp_struct<T> nes = es;
  e_frac[e_frac.wl() - e_frac.iwl()] = 1; // The implicit '1' in IEEE format.
  e_frac(e_frac.wl() - e_frac.iwl() - 1, 0) = nes.sig(org_wf - 1, 0);
  if (nes.sign)
    e_frac = -e_frac;

  ap_fixed<1 + we + 1 + wf + org_wf, 1 + we + 1> m_frac_l = log_base * e_frac;
  // use f_frac_l instead of m_frac in the exp shift
  // ap_fixed<1 + we+1 + wf, 1 + we+1> m_frac = m_frac_l;

  // 3. result = e ^ mult
  ap_fixed<1 + we + wf + org_wf, 1 + we> m_fix_l =
      m_frac_l << m_exp; // used for overflow checking only
  ap_fixed<1 + we + wf + org_wf, 1 + we> m_fix_back = m_fix_l >> m_exp;

  const static int exp_gbits = pow_traits<T>::exp_gbits;
  ap_fixed<1 + we + org_wf + exp_gbits, 1 + we> m_fix;

  // should we remove the if here??
  if (m_exp >= 0)
    m_fix = m_frac_l << m_exp;
  else
    m_fix = m_frac_l >> -m_exp;

  ap_fixed<1 + we + 4, 1 + we> m_fix_hi = m_fix;

  const ap_ufixed<1 + we + 3, 1> LOG2R = 1.4426950408889634073599246810019;

  ap_fixed<2, 1> delta1;
  delta1[1] = m_fix[m_fix.wl() - 1];
  delta1[0] = 1;

  ap_int<1 + we + 1> r_exp = m_fix_hi * LOG2R + delta1;

  const ap_ufixed<org_wf + exp_gbits + we + 1, 0> LOG2_s = LOG2;

  // assert( (m_fix_back != m_frac_l) || ((r_exp*LOG2_s) < pow(2,we) &&
  // (r_exp*LOG2_s) > -pow(2,we)) );// to guarantee not overflow
  ap_fixed<1 + we + org_wf + exp_gbits, 1 + we> m_fix_a =
      r_exp * LOG2_s; // m_fix approximation

  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a < 0.5)); // check r_exp zeros out integer and most
                                   // significant fraction bits
  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a > -0.5)); // check r_exp zeros out integer and most
                                    // significant fraction bits
  ap_fixed<1 - 1 + org_wf + exp_gbits, 1 - 1> m_diff = m_fix - m_fix_a;

  // e^Y = 1 + Y + Y^2/2 + ... + Y^n/n! + ...
  // term Y^n/n! can be eliminated when its MSB is less than 2^-(wf+g)
  // Y belongs to (-.5,.5)
  // w = wf+g
  // g = 3,4,7 for h,f,d
  // g_Z2 = 3,3,5 for h,f,d
  // Y = Z1 + Z1P
  //   = Z1 + Z2 + Z2P
  //   = ...
  //   = Z1 + Z2 + ... + Zk
  // wn is width of Zn, n = 1...k
  // T_Z1 = 2^w1*(w+1)
  // T_Z2 = 2^w2*(w+1-2*w1)
  // T_Z3 = 2^w3*(w+1-2*(w1+w2))
  // ...
  //
  //		|	h	|	f	|	d
  //	========================================================
  //	wf	|	10	|	23	|	52
  //	g	|	3	|	4	|	7
  //	g_Z2    |       3       |       3       |       5
  //	w	|	13	|	27	|	59
  //	k	|	2	|	2	|	4
  //	wn	|	9,4	|	9,18	|    8,8,8,35
  //	T_total	|	7k	|	<18k	| 14.5k+10.5k+6.5k
  //	Mult	|	5bit	|	1DSP	|	16DSP

  const static int w_Z1 = pow_traits<T>::w_Z1;
  // Z1
  ap_uint<w_Z1> m_diff_hi = m_diff(m_diff.wl() - 1, m_diff.wl() - w_Z1);
  // Z1P = Z2 + ... + Zk
  ap_ufixed<org_wf + exp_gbits - w_Z1, -w_Z1> m_diff_lo =
      m_diff; // ( m_diff.wl()-m_diff.iwl()-w_Z1-1 , 0 );

  // e^Z1 by table_exp_Z1
  const static int exp_gbits_Z2 = pow_traits<T>::exp_gbits_Z2;
  ap_ufixed<1 + org_wf + exp_gbits_Z2, 1> exp_Z1 =
      table_exp_Z1<ap_ufixed<1 + org_wf + exp_gbits_Z2, 1>>::array[m_diff_hi];
  ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
      pow_traits<T>::exp_Z1P_m_1(m_diff_lo);
  ap_ufixed<1 + org_wf + exp_gbits_Z2 - w_Z1, 1> exp_Z1_hi = exp_Z1;
  ap_ufixed<1, -org_wf> delta;
  delta[0] = 1;
  ap_ufixed<2 + org_wf + exp_gbits_Z2, 2> exp_Y_l =
      (exp_Z1 + delta) + exp_Z1_hi * exp_Z1P_m_1;
  assert(exp_Y_l[exp_Y_l.wl() - 1] == 0);
  ap_ufixed<1 + org_wf + exp_gbits_Z2, 1> exp_Y = exp_Y_l;

  if (exp_Y[exp_Y.wl() - 1] == 0) {
    exp_Y = exp_Y << 1;
    r_exp = r_exp - 1;
  }

  // check overflow here
  if ((m_exp > 0 && m_fix_back != m_frac_l) ||
      (r_exp > fp_struct<T>::EXP_BIAS)) {
    if (~m_frac_l[m_frac_l.wl() - 1]) {
      // out = +/-inf
      // out.sign[0] = r_sign;
      // out.sig = 0;
      out.exp = fp_struct<T>::EXP_INFNAN;
      return out.to_ieee();
    } else {
      // out = +/-0
      // out.sign[0] = r_sign;
      // out.sig = 0;
      out.exp = 0;
      return out.to_ieee();
    }
  }

  // check underflow here
  if (r_exp <= -fp_struct<T>::EXP_BIAS) {
    // out = +/-0
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // let's output the result
  //    fp_struct<T> out;
  // out.sign[0] = r_sign;
  out.sig(org_wf - 1, 0) = exp_Y(exp_Y.wl() - 1 - 1, exp_Y.wl() - 1 - org_wf);
  out.exp = fp_struct<T>::EXP_BIAS + r_exp;

  return out.to_ieee();
}

static half pow(half base, half exp) { return pow_generic(base, exp); }
static float pow(float base, float exp) { return pow_generic(base, exp); }
static double pow(double base, double exp) { return pow_generic(base, exp); }

static float powf(float base, float exp) { return pow_generic(base, exp); }

static half half_pow(half base, half exp) { return pow_generic(base, exp); }

template <typename T> T powr_generic(T base, T exp) {
#pragma HLS pipeline

  fp_struct<T> bs(base);
  fp_struct<T> es(exp);

  fp_struct<T> out;
  out.sign[0] = 0;
  out.sig = 0;

  int b_exp = bs.expv();

  // special cases for powr() include:
  // x = 0, 1, -1, +inf, -inf, NaN
  // y = 0, +inf, -inf, NaN
  //
  //	y \ x	|	0	|	+1	|	-1	|	inf	|	NaN	|
  //normal
  // =============================================================================================================
  // 	0	|	NaN	|	1	|	NaN	|	NaN	|	NaN	|
  // 1
  // 	inf	|		|	NaN	|	NaN	|		|	NaN
  // |
  // 	NaN	|	NaN	|	NaN	|	NaN	|	NaN	|	NaN	|
  // NaN 	normal	|		|	1	|	NaN	|
  // |	NaN	|
  //
  // x < 0 (including -1 and -inf), r = NaN
  //
  //	y \ x	|	+0	|	-0	|	+inf	|	-inf	|	x>1	|
  //0<x<1
  // =============================================================================================================
  // 	+inf	|	0	|	0	|	+inf	|	NaN	|	+inf	|
  // 0
  // 	-inf	|	+inf	|	+inf	|	0	|	NaN	|	0	|
  // +inf
  //  pos non int	|	0	|	0	|	+inf	|	NaN
  //  | neg non int	|	+inf	|	+inf	|	0
  //  |	NaN	|
  //  pos odd int	|	0	|	0	|       +inf    |       NaN
  //  | pos even int|	0	|	0	|	+inf	| NaN	| neg
  //  odd int	|	+inf	|	+inf	|	0	|	NaN
  //  | neg even int|	+inf	|	+inf	|	0 |	NaN	|
  //
  bool y_is_0 = 0;
  bool x_is_0 = 0;
  bool x_is_n0 = 0;
  bool x_is_1 = 0;
  bool y_is_inf = 0;
  bool x_is_inf = 0;
  bool y_is_NaN = 0;
  bool x_is_NaN = 0;
  bool x_is_neg = 0; // including -inf

#ifndef STDSUBNORMALS
  if (es.exp == 0)
#else
  if (es.exp == 0 && es.sig == 0)
#endif
    y_is_0 = 1;

#ifndef STDSUBNORMALS
  if (bs.exp == 0)
#else
  if (bs.exp == 0 && bs.sig == 0)
#endif
    x_is_0 = 1;
  x_is_n0 = x_is_0 & bs.sign[0];

  if (b_exp == 0 && bs.sig == 0)
    x_is_1 = 1;

  if (::hls::__isinf(exp))
    y_is_inf = 1;
  if (::hls::__isinf(base))
    x_is_inf = 1;

  if (::hls::__isnan(exp))
    y_is_NaN = 1;
  if (::hls::__isnan(base))
    x_is_NaN = 1;

  if (bs.sign[0] == 1 && x_is_0 == 0)
    x_is_neg = 1;

  if (y_is_NaN | x_is_NaN | x_is_neg | ((x_is_0 | x_is_inf) & y_is_0) |
      (x_is_1 & y_is_inf)) {
    // out = NaN
    // out.sign[0] = 0;
    out.sig = -1; // all 1's
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }

  if (x_is_n0) {
    base = -base;
  }

  return pow_generic(base, exp);
}

static half powr(half base, half exp) { return powr_generic(base, exp); }
static float powr(float base, float exp) { return powr_generic(base, exp); }
static double powr(double base, double exp) { return powr_generic(base, exp); }

static float powrf(float base, float exp) { return powr_generic(base, exp); }

static half half_powr(half base, half exp) { return powr_generic(base, exp); }

template <typename T> T pown_generic(T base, int exp) {
#pragma HLS pipeline

  fp_struct<T> bs(base);
  ap_fixed<1 + 31, 1 + 31> es = exp;
  const static int we = pow_traits<T>::we;
  const static int wf = pow_traits<T>::wf;
  const static int org_wf = pow_traits<T>::org_wf;

  fp_struct<T> out;
  out.sign[0] = 0;
  out.sig = 0;

  int b_exp = bs.expv();

  // special cases include:
  // x = 0, +inf, -inf, NaN
  // y = 0
  //
  //	y \ x	|	0	|	inf	|	NaN	|	normal
  // =============================================================================
  // 	0	|	1	|	1	|	1	|	1
  // 	normal	|		|		|	NaN	|
  //
  bool y_is_0 = 0;
  bool x_is_NaN = 0;

  if (exp == 0)
    y_is_0 = 1;
  if (::hls::__isnan(base))
    x_is_NaN = 1;

  if (y_is_0) {
    // out = 1
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_BIAS;
    return out.to_ieee();
  }
  if (x_is_NaN) {
    // out = NaN
    // out.sign[0] = 0;
    out.sig = -1; // all 1's
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }

  //	y \ x	|	+0	|	-0	|	+inf	|	-inf
  // =============================================================================
  //  pos odd int	|	0	|	-0	|       +inf    | -inf
  //  pos even int|	0	|	0	|	+inf	|	+inf
  //  neg odd int	|	+inf	|	-inf	|	0	|
  //  -0 neg even int|	+inf	|	+inf	|	0	|	0
  //
  bool x_is_0 = 0;
  bool x_is_inf = 0;
  bool y_is_pos = ~es[31];    // use for 1st and 3rd columns of the table only
  bool y_is_odd = es[0];      // use for 2nd and 4th columns of the table only
  bool x_is_neg = bs.sign[0]; // including -0 and -inf
  bool r_sign = 0;            // use to calculate sgn(x)^y

#ifndef STDSUBNORMALS
  if (bs.exp == 0)
#else
  if (bs.exp == 0 && bs.sig == 0)
#endif
    x_is_0 = 1;
  if (::hls::__isinf(base))
    x_is_inf = 1;

  r_sign = x_is_neg & y_is_odd;
  out.sign[0] = r_sign;

  if ((x_is_0 & ~y_is_pos) | (x_is_inf & y_is_pos)) {
    // out = +/-inf
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if ((x_is_0 & y_is_pos) | (x_is_inf & ~y_is_pos)) {
    // out = +/-0
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // 1. log|base|
  const static int log_bypass_threshold = wf / 2;
  const static int LogMaxPrecision = pow_traits<T>::LogMaxPrecision;
  typedef typename pow_traits<T>::LOG_TYPE LOG_TYPE;
  LOG_TYPE sum;
  LOG_TYPE log_sum;

  // storing exp and frac of base using int b_exp and ap_ufixed<1+wf+1,1> b_frac
  // int b_exp = bs.expv();
  // initialization is necessary because we are extending the width of b_frac
  // from org_wf to wf
  ap_ufixed<1 + org_wf + 1, 1> b_frac = 0;

  fp_struct<T> nbs = bs;

#ifdef STDSUBNORMALS
  if (nbs.exp == 0 && nbs.sig != 0) {
    // subnormal handling.
    unsigned int zeros;
#pragma unroll
    for (zeros = 0; zeros < org_wf; zeros++)
      if (nbs.sig[org_wf - zeros - 1] == 1)
        break;
    b_exp -= zeros;
    nbs.sig =
        nbs.sig << (zeros + 1); // add one so we shift off the leading one.
  }
#else

#endif

  int FirstBit = nbs.sig[org_wf - 1];

  // Y is prenormalized, between 0.75 and 1.5
  b_frac[b_frac.wl() - b_frac.iwl()] = 1; // The implicit '1' in IEEE format.
  b_frac(b_frac.wl() - b_frac.iwl() - 1, 1) = nbs.sig(org_wf - 1, 0);

  ap_uint<1 + alpha0> index0 =
      b_frac(b_frac.wl() - b_frac.iwl() - 1,
             b_frac.wl() - b_frac.iwl() - (1 + alpha0));

  // the first two bits of b_frac can only be 01 or 10
  if (FirstBit) {
    b_frac = b_frac >> 1;
    b_exp += 1;
  }

  // const ap_ufixed<wf+we+2, 0> LOG2 = 0.69314718055994530941723212145818;
  const ap_ufixed<wf + we + 2, 0> LOG2_hi = 0.69314718055994517520446152047953;
  const ap_ufixed<wf + we + 2, 0> LOG2_lo = 0.60444058366692930952011528220776;
  const ap_ufixed<wf + we + 2, 0> LOG2 = LOG2_hi + (LOG2_lo >> 52);

  ap_fixed<1 + we + wf + we + 2, 1 + we> Elog2 = LOG2 * b_exp;

  const int T0size = 1 << (1 + alpha0); // The size of the first table

#define inverse_lut log_inverse_lut_table_pow<p0, alpha0, T0size>::array
#define log_lut log0_lut_table<LOG_TYPE, T, p0, alpha0, T0size>::array

  // ap_uint<1+alpha0> index0 = b_frac(b_frac.wl()-1, b_frac.wl()-(1+alpha0));
  ap_ufixed<1 + alpha0, 1> b_frac_tilde_inverse = inverse_lut[index0];

  log_sum = log_lut[index0];

  // The first bits of y1 are always a 1, followed by p1 zeros, but
  // it gets dropped later
  ap_ufixed<1 + org_wf + 1 + (1 + alpha0), 1> b_frac1 =
      b_frac * b_frac_tilde_inverse;
  // p_next = p1;

  assert(b_frac1(b_frac1.wl() - b_frac1.iwl() - 1,
                 b_frac1.wl() - b_frac1.iwl() - p1) ==
         0); // Check that the first stage zeros out p1 bits.

  const int z1_width = 1 + wf + 1 + (1 + alpha0) - p1 + 1;
  ap_ufixed<z1_width, -p1> z1 = b_frac1; // Extract a1+b1 from y1.

  ap_fixed<1 - log_bypass_threshold + 1 + LogMaxPrecision,
           -log_bypass_threshold + 1>
      zk;
  zk = pow_traits<T>::log_range_reduction(z1, log_sum);

  ap_fixed<1 - 2 * log_bypass_threshold + 1 + LogMaxPrecision,
           -log_bypass_threshold + 1>
      zk_trunc = zk;
  sum = zk - (zk_trunc * zk_trunc / 2);
  // sum = zk*2/(zk+2);

  ap_fixed<1 + we + wf, 1 + we> log_base = Elog2 + log_sum + sum;
#undef log_lut
#undef inverse_lut

  // 2. mult = m_frac * 2^m_exp = exp * log|base|
  ap_fixed<1 + we + 31 + wf, 1 + we + 31> m_frac_l = log_base * es;

  // 3. result = e ^ mult
  ap_fixed<1 + we + wf, 1 + we> m_fix_back =
      m_frac_l; // used for overflow checking only

  const static int exp_gbits = pow_traits<T>::exp_gbits;
  ap_fixed<1 + we + org_wf + exp_gbits, 1 + we> m_fix = m_frac_l;

  ap_fixed<1 + we + 4, 1 + we> m_fix_hi = m_fix;

  const ap_ufixed<1 + we + 3, 1> LOG2R = 1.4426950408889634073599246810019;

  ap_fixed<2, 1> delta1;
  delta1[1] = m_fix[m_fix.wl() - 1];
  delta1[0] = 1;

  ap_int<1 + we + 1> r_exp = m_fix_hi * LOG2R + delta1;

  const ap_ufixed<org_wf + exp_gbits + we + 1, 0> LOG2_s = LOG2;

  // assert( (m_fix_back != m_frac_l) || ((r_exp*LOG2_s) < pow(2,we) &&
  // (r_exp*LOG2_s) > -pow(2,we)) );// to guarantee not overflow
  ap_fixed<1 + we + org_wf + exp_gbits, 1 + we> m_fix_a =
      r_exp * LOG2_s; // m_fix approximation

  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a < 0.5)); // check r_exp zeros out integer and most
                                   // significant fraction bits
  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a > -0.5)); // check r_exp zeros out integer and most
                                    // significant fraction bits
  ap_fixed<1 - 1 + org_wf + exp_gbits, 1 - 1> m_diff = m_fix - m_fix_a;

  // e^Y = 1 + Y + Y^2/2 + ... + Y^n/n! + ...
  // term Y^n/n! can be eliminated when its MSB is less than 2^-(wf+g)
  // Y belongs to (-.5,.5)
  // w = wf+g
  // g = 3,4,7 for h,f,d
  // g_Z2 = 3,3,5 for h,f,d
  // Y = Z1 + Z1P
  //   = Z1 + Z2 + Z2P
  //   = ...
  //   = Z1 + Z2 + ... + Zk
  // wn is width of Zn, n = 1...k
  // T_Z1 = 2^w1*(w+1)
  // T_Z2 = 2^w2*(w+1-2*w1)
  // T_Z3 = 2^w3*(w+1-2*(w1+w2))
  // ...
  //
  //		|	h	|	f	|	d
  //	========================================================
  //	wf	|	10	|	23	|	52
  //	g	|	3	|	4	|	7
  //	g_Z2    |       3       |       3       |       5
  //	w	|	13	|	27	|	59
  //	k	|	2	|	2	|	4
  //	wn	|	9,4	|	9,18	|    8,8,8,35
  //	T_total	|	7k	|	<18k	| 14.5k+10.5k+6.5k
  //	Mult	|	5bit	|	1DSP	|	16DSP

  const static int w_Z1 = pow_traits<T>::w_Z1;
  // Z1
  ap_uint<w_Z1> m_diff_hi = m_diff(m_diff.wl() - 1, m_diff.wl() - w_Z1);
  // Z1P = Z2 + ... + Zk
  ap_ufixed<org_wf + exp_gbits - w_Z1, -w_Z1> m_diff_lo =
      m_diff; // ( m_diff.wl()-m_diff.iwl()-w_Z1-1 , 0 );

  // e^Z1 by table_exp_Z1
  const static int exp_gbits_Z2 = pow_traits<T>::exp_gbits_Z2;
  ap_ufixed<1 + org_wf + exp_gbits_Z2, 1> exp_Z1 =
      table_exp_Z1<ap_ufixed<1 + org_wf + exp_gbits_Z2, 1>>::array[m_diff_hi];
  ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
      pow_traits<T>::exp_Z1P_m_1(m_diff_lo);
  ap_ufixed<1 + org_wf + exp_gbits_Z2 - w_Z1, 1> exp_Z1_hi = exp_Z1;
  ap_ufixed<1, -org_wf> delta;
  delta[0] = 1;
  ap_ufixed<2 + org_wf + exp_gbits_Z2, 2> exp_Y_l =
      (exp_Z1 + delta) + exp_Z1_hi * exp_Z1P_m_1;
  assert(exp_Y_l[exp_Y_l.wl() - 1] == 0);
  ap_ufixed<1 + org_wf + exp_gbits_Z2, 1> exp_Y = exp_Y_l;

  if (exp_Y[exp_Y.wl() - 1] == 0) {
    exp_Y = exp_Y << 1;
    r_exp = r_exp - 1;
  }

  // check overflow here
  if ((m_fix_back != m_frac_l) || (r_exp > fp_struct<T>::EXP_BIAS)) {
    if (~m_frac_l[m_frac_l.wl() - 1]) {
      // out = +/-inf
      // out.sign[0] = r_sign;
      // out.sig = 0;
      out.exp = fp_struct<T>::EXP_INFNAN;
      return out.to_ieee();
    } else {
      // out = +/-0
      // out.sign[0] = r_sign;
      // out.sig = 0;
      out.exp = 0;
      return out.to_ieee();
    }
  }

  // check underflow here
  if (r_exp <= -fp_struct<T>::EXP_BIAS) {
    // out = +/-0
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // let's output the result
  //    fp_struct<T> out;
  // out.sign[0] = r_sign;
  out.sig(org_wf - 1, 0) = exp_Y(exp_Y.wl() - 1 - 1, exp_Y.wl() - 1 - org_wf);
  out.exp = fp_struct<T>::EXP_BIAS + r_exp;

  return out.to_ieee();
}

static double pown(double base, int exp) { return pown_generic(base, exp); }

template <typename T> T rootn_generic(T base, int exp) {
#pragma HLS pipeline

  fp_struct<T> bs(base);
  ap_fixed<1 + 31, 1 + 31> es = exp;
  const static int we = pow_traits<T>::we;
  const static int wf = pow_traits<T>::wf;
  const static int org_wf = pow_traits<T>::org_wf;

  fp_struct<T> out;
  out.sign[0] = 0;
  out.sig = 0;

  int b_exp = bs.expv();

  // special cases include:
  // x = 0, 1, -1, +inf, -inf, NaN
  // y = 0, 1, -1
  //
  //	y \ x	|	<0	|	+0	|	-0	|	+inf	|	-inf	|
  //NaN	|	normal
  // =============================================================================================================================
  // 	0	|	NaN	|	NaN	|	NaN	|	NaN	|	NaN	|	NaN	|
  // NaN 	1/-1	|		|	0/+inf	|	-0/-inf	|
  // +inf/0	|	-inf/-0	|	NaN	|
  //   +/-others	|	NaN	|	0/+inf	|	NaN	|	+inf/0	|	NaN	|	NaN
  //   |
  //
  // x < 0 (including -inf) and 1/y is non int, r = NaN
  //
  bool x_is_NaN = 0;
  bool x_is_neg = 0; // including -0 and -inf
  bool x_is_0 = 0;

  if (::hls::__isnan(base))
    x_is_NaN = 1;
  if (bs.sign[0] == 1)
    x_is_neg = 1;
#ifndef STDSUBNORMALS
  if (bs.exp == 0)
#else
  if (bs.exp == 0 && bs.sig == 0)
#endif
    x_is_0 = 1;

  if ((es == 0) | x_is_NaN | (x_is_neg & ~x_is_0 & (es != 1) & (es != -1))) {
    // out = NaN
    // out.sign[0] = 0;
    out.sig = -1; // all 1's
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }

  bool x_is_inf = 0;
  bool r_sign = 0; // use to calculate sgn(x)^y

  if (::hls::__isinf(base))
    x_is_inf = 1;
  ap_int<2> exp1 = exp;
  r_sign = x_is_neg & (~x_is_0 | (x_is_0 & exp1[0]));
  out.sign[0] = r_sign;

  if ((x_is_0 & (es < 0)) | (x_is_inf & (es > 0))) {
    // out = +/-inf
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if ((x_is_0 & (es > 0)) | (x_is_inf & (es < 0))) {
    // out = +/-0
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // 1. log|base|
  const static int log_bypass_threshold = wf / 2;
  const static int LogMaxPrecision = pow_traits<T>::LogMaxPrecision;
  typedef typename pow_traits<T>::LOG_TYPE LOG_TYPE;
  LOG_TYPE sum;
  LOG_TYPE log_sum;

  // storing exp and frac of base using int b_exp and ap_ufixed<1+wf+1,1> b_frac
  // int b_exp = bs.expv();
  // initialization is necessary because we are extending the width of b_frac
  // from org_wf to wf
  ap_ufixed<1 + org_wf + 1, 1> b_frac = 0;

  fp_struct<T> nbs = bs;

#ifdef STDSUBNORMALS
  if (nbs.exp == 0 && nbs.sig != 0) {
    // subnormal handling.
    unsigned int zeros;
#pragma unroll
    for (zeros = 0; zeros < org_wf; zeros++)
      if (nbs.sig[org_wf - zeros - 1] == 1)
        break;
    b_exp -= zeros;
    nbs.sig =
        nbs.sig << (zeros + 1); // add one so we shift off the leading one.
  }
#else

#endif

  int FirstBit = nbs.sig[org_wf - 1];

  // Y is prenormalized, between 0.75 and 1.5
  b_frac[b_frac.wl() - b_frac.iwl()] = 1; // The implicit '1' in IEEE format.
  b_frac(b_frac.wl() - b_frac.iwl() - 1, 1) = nbs.sig(org_wf - 1, 0);

  ap_uint<1 + alpha0> index0 =
      b_frac(b_frac.wl() - b_frac.iwl() - 1,
             b_frac.wl() - b_frac.iwl() - (1 + alpha0));

  // the first two bits of b_frac can only be 01 or 10
  if (FirstBit) {
    b_frac = b_frac >> 1;
    b_exp += 1;
  }

  // const ap_ufixed<wf+we+2, 0> LOG2 = 0.69314718055994530941723212145818;
  const ap_ufixed<wf + we + 2, 0> LOG2_hi = 0.69314718055994517520446152047953;
  const ap_ufixed<wf + we + 2, 0> LOG2_lo = 0.60444058366692930952011528220776;
  const ap_ufixed<wf + we + 2, 0> LOG2 = LOG2_hi + (LOG2_lo >> 52);

  ap_fixed<1 + we + wf + we + 2, 1 + we> Elog2 = LOG2 * b_exp;

  const int T0size = 1 << (1 + alpha0); // The size of the first table

#define inverse_lut log_inverse_lut_table_pow<p0, alpha0, T0size>::array
#define log_lut log0_lut_table<LOG_TYPE, T, p0, alpha0, T0size>::array

  // ap_uint<1+alpha0> index0 = b_frac(b_frac.wl()-1, b_frac.wl()-(1+alpha0));
  ap_ufixed<1 + alpha0, 1> b_frac_tilde_inverse = inverse_lut[index0];

  log_sum = log_lut[index0];

  // The first bits of y1 are always a 1, followed by p1 zeros, but
  // it gets dropped later
  ap_ufixed<1 + org_wf + 1 + (1 + alpha0), 1> b_frac1 =
      b_frac * b_frac_tilde_inverse;
  // p_next = p1;

  assert(b_frac1(b_frac1.wl() - b_frac1.iwl() - 1,
                 b_frac1.wl() - b_frac1.iwl() - p1) ==
         0); // Check that the first stage zeros out p1 bits.

  const int z1_width = 1 + wf + 1 + (1 + alpha0) - p1 + 1;
  ap_ufixed<z1_width, -p1> z1 = b_frac1; // Extract a1+b1 from y1.

  ap_fixed<1 - log_bypass_threshold + 1 + LogMaxPrecision,
           -log_bypass_threshold + 1>
      zk;
  zk = pow_traits<T>::log_range_reduction(z1, log_sum);

  ap_fixed<1 - 2 * log_bypass_threshold + 1 + LogMaxPrecision,
           -log_bypass_threshold + 1>
      zk_trunc = zk;
  sum = zk - (zk_trunc * zk_trunc / 2);
  // sum = zk*2/(zk+2);

  ap_fixed<1 + we + wf, 1 + we> log_base = Elog2 + log_sum + sum;
#undef log_lut
#undef inverse_lut

  // 2. mult = m_frac * 2^m_exp = exp * log|base|
  ap_fixed<1 + we + wf + org_wf, 1 + we> m_frac_l = log_base / es;

  // 3. result = e ^ mult
  ap_fixed<1 + we + wf + org_wf, 1 + we> m_fix_back = m_frac_l;

  const static int exp_gbits = pow_traits<T>::exp_gbits;
  ap_fixed<1 + we + org_wf + exp_gbits, 1 + we> m_fix = m_frac_l;

  ap_fixed<1 + we + 4, 1 + we> m_fix_hi = m_fix;

  const ap_ufixed<1 + we + 3, 1> LOG2R = 1.4426950408889634073599246810019;

  ap_fixed<2, 1> delta1;
  delta1[1] = m_fix[m_fix.wl() - 1];
  delta1[0] = 1;

  ap_int<1 + we + 1> r_exp = m_fix_hi * LOG2R + delta1;

  const ap_ufixed<org_wf + exp_gbits + we + 1, 0> LOG2_s = LOG2;

  // assert( (m_fix_back != m_frac_l) || ((r_exp*LOG2_s) < pow(2,we) &&
  // (r_exp*LOG2_s) > -pow(2,we)) );// to guarantee not overflow
  ap_fixed<1 + we + org_wf + exp_gbits, 1 + we> m_fix_a =
      r_exp * LOG2_s; // m_fix approximation

  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a < 0.5)); // check r_exp zeros out integer and most
                                   // significant fraction bits
  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a > -0.5)); // check r_exp zeros out integer and most
                                    // significant fraction bits
  ap_fixed<1 - 1 + org_wf + exp_gbits, 1 - 1> m_diff = m_fix - m_fix_a;

  // e^Y = 1 + Y + Y^2/2 + ... + Y^n/n! + ...
  // term Y^n/n! can be eliminated when its MSB is less than 2^-(wf+g)
  // Y belongs to (-.5,.5)
  // w = wf+g
  // g = 3,4,7 for h,f,d
  // g_Z2 = 3,3,5 for h,f,d
  // Y = Z1 + Z1P
  //   = Z1 + Z2 + Z2P
  //   = ...
  //   = Z1 + Z2 + ... + Zk
  // wn is width of Zn, n = 1...k
  // T_Z1 = 2^w1*(w+1)
  // T_Z2 = 2^w2*(w+1-2*w1)
  // T_Z3 = 2^w3*(w+1-2*(w1+w2))
  // ...
  //
  //		|	h	|	f	|	d
  //	========================================================
  //	wf	|	10	|	23	|	52
  //	g	|	3	|	4	|	7
  //	g_Z2    |       3       |       3       |       5
  //	w	|	13	|	27	|	59
  //	k	|	2	|	2	|	4
  //	wn	|	9,4	|	9,18	|    8,8,8,35
  //	T_total	|	7k	|	<18k	| 14.5k+10.5k+6.5k
  //	Mult	|	5bit	|	1DSP	|	16DSP

  const static int w_Z1 = pow_traits<T>::w_Z1;
  // Z1
  ap_uint<w_Z1> m_diff_hi = m_diff(m_diff.wl() - 1, m_diff.wl() - w_Z1);
  // Z1P = Z2 + ... + Zk
  ap_ufixed<org_wf + exp_gbits - w_Z1, -w_Z1> m_diff_lo =
      m_diff; // ( m_diff.wl()-m_diff.iwl()-w_Z1-1 , 0 );

  // e^Z1 by table_exp_Z1
  const static int exp_gbits_Z2 = pow_traits<T>::exp_gbits_Z2;
  ap_ufixed<1 + org_wf + exp_gbits_Z2, 1> exp_Z1 =
      table_exp_Z1<ap_ufixed<1 + org_wf + exp_gbits_Z2, 1>>::array[m_diff_hi];
  ap_ufixed<org_wf + exp_gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
      pow_traits<T>::exp_Z1P_m_1(m_diff_lo);
  ap_ufixed<1 + org_wf + exp_gbits_Z2 - w_Z1, 1> exp_Z1_hi = exp_Z1;
  ap_ufixed<1, -org_wf> delta;
  delta[0] = 1;
  ap_ufixed<2 + org_wf + exp_gbits_Z2, 2> exp_Y_l =
      (exp_Z1 + delta) + exp_Z1_hi * exp_Z1P_m_1;
  assert(exp_Y_l[exp_Y_l.wl() - 1] == 0);
  ap_ufixed<1 + org_wf + exp_gbits_Z2, 1> exp_Y = exp_Y_l;

  if (exp_Y[exp_Y.wl() - 1] == 0) {
    exp_Y = exp_Y << 1;
    r_exp = r_exp - 1;
  }

  // check overflow here
  if (r_exp > fp_struct<T>::EXP_BIAS) {
    if (~m_frac_l[m_frac_l.wl() - 1]) {
      // out = +/-inf
      // out.sign[0] = r_sign;
      // out.sig = 0;
      out.exp = fp_struct<T>::EXP_INFNAN;
      return out.to_ieee();
    } else {
      // out = +/-0
      // out.sign[0] = r_sign;
      // out.sig = 0;
      out.exp = 0;
      return out.to_ieee();
    }
  }

  // check underflow here
  if (r_exp <= -fp_struct<T>::EXP_BIAS) {
    // out = +/-0
    // out.sign[0] = r_sign;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // let's output the result
  //    fp_struct<T> out;
  // out.sign[0] = r_sign;
  out.sig(org_wf - 1, 0) = exp_Y(exp_Y.wl() - 1 - 1, exp_Y.wl() - 1 - org_wf);
  out.exp = fp_struct<T>::EXP_BIAS + r_exp;

  return out.to_ieee();
}

static half rootn(half base, int exp) { return rootn_generic(base, exp); }
static float rootn(float base, int exp) { return rootn_generic(base, exp); }
static double rootn(double base, int exp) { return rootn_generic(base, exp); }

static float rootnf(float base, int exp) { return rootn_generic(base, exp); }

static half half_rootn(half base, int exp) { return rootn_generic(base, exp); }

} // namespace pow_reduce
