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
 * @file hls_exp.h
 *
 * see: J.Detrey and F.d.Dinechin, "A parameterized floating-point exponential
 * function for FPGA"
 */
namespace exp_reduce_ {

#include "hls_exp_tables_.h"

template <typename T> class exp_traits {};

template <> class exp_traits<half> {
public:
  const static int we = fp_struct<half>::EXP_BITS;
  const static int wf = fp_struct<half>::SIG_BITS; // 10

  const static int gbits = 3;
  const static int gbits_Z2 = 3;
  const static int w_Z1 = 9;

  // input ap_ufixed < 4, -9 >
  // output ap_uifxed < 5, -8 >
  static ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1>
  exp_Z1P_m_1(ap_ufixed<wf + gbits - w_Z1, -w_Z1> Z1P) {
    ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> Z1P_l = Z1P;
    return Z1P_l;
  }
};

template <> class exp_traits<float> {
public:
  const static int we = fp_struct<float>::EXP_BITS;
  const static int wf = fp_struct<float>::SIG_BITS; // 23

  const static int gbits = 4;
  const static int gbits_Z2 = 3;
  const static int w_Z1 = 9;

  // input ap_ufixed < 18, -9 >
  // output ap_ufixed < 18, -8 >
  // Z = Z1 + Z1P = Z1 + Z2
  // e^Z1P - 1 = Z1P + Z2^2/2 = Z1P + f(Z2)
  // w_Z2_ind = w - w/2 - w1 = 5
  static ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1>
  exp_Z1P_m_1(ap_ufixed<wf + gbits - w_Z1, -w_Z1> Z1P) {
    const static int w_Z2_ind = 5;
    ap_uint<w_Z2_ind> Z2_ind = Z1P(Z1P.wl() - 1, Z1P.wl() - w_Z2_ind);
    ap_ufixed<wf + gbits - 2 * w_Z1 - 1, -2 *w_Z1 - 1> f_Z2 =
        table_f_Z2<ap_ufixed<wf + gbits, 0>>::array[Z2_ind]; // < 8, -19 >
    ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 = Z1P + f_Z2;
    return exp_Z1P_m_1;
  }
};

template <> class exp_traits<double> {
public:
  const static int we = fp_struct<double>::EXP_BITS;
  const static int wf = fp_struct<double>::SIG_BITS; // 52

  // guard bits should be 7bit
  // but to reduce size of multiplier
  // use 7bit for exp_Z3_m_1 * exp_Z4_m_1 and exp_Z2_m_1 * exp_Z2P_m_1
  // use 5bit for exp_Z1 * exp_Z1P_m_1
  const static int gbits = 7;
  const static int gbits_Z2 = 5;
  const static int w_Z1 = 8;

  // input ap_ufixed < 51, -8 >
  // output ap_ufixed < 50, -7 >
  // Z = Z1 + Z2 + Z3 + Z4, w = 59, w1 = 8, w2 = 8, w3 = 8, w4 = 35
  static ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1>
  exp_Z1P_m_1(ap_ufixed<wf + gbits - w_Z1, -w_Z1> Z1P) {
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
    ap_ufixed<wf + gbits - 2 * (w_Z1 + w_Z2 + w_Z3) - 1,
              -2 * (w_Z1 + w_Z2 + w_Z3) - 1>
        f_Z4 = table_f_Z3<ap_ufixed<wf + gbits, 0>>::array[Z4_ind] >>
               (2 * w_Z3); // < 10, -49 >
    ap_ufixed<wf + gbits - w_Z1 - w_Z2 - w_Z3 + 1, -w_Z1 - w_Z2 - w_Z3 + 1>
        exp_Z4_m_1 = Z4 + f_Z4; // < 36, -23 >

    // 1.2 Z3 + f(Z3)
    ap_uint<w_Z3> Z3_ind = Z3(Z3.wl() - 1, 0);
    ap_ufixed<wf + gbits - 2 * (w_Z1 + w_Z2) - 1, -2 * (w_Z1 + w_Z2) - 1> f_Z3 =
        table_f_Z3<ap_ufixed<wf + gbits, 0>>::array[Z3_ind]; // < 26, -33 >
    ap_ufixed<wf + gbits - w_Z1 - w_Z2 + 1, -w_Z1 - w_Z2 + 1> exp_Z3_m_1 =
        Z3 + f_Z3; // < 44, -15 >

    // 1.3 ( Z3 + f(Z3) ) * ( Z4 + f(Z4) )
    ap_ufixed<wf + gbits - 2 * (w_Z1 + w_Z2) - w_Z3 + 2,
              -2 * (w_Z1 + w_Z2) - w_Z3 + 2>
        exp_Z2P_m_1_lo = exp_Z3_m_1 * exp_Z4_m_1; // < 21, -38 >

    // 1.4 e^Z2P - 1
    ap_ufixed<wf + gbits - w_Z1 - w_Z2 + 2, -w_Z1 - w_Z2 + 2> exp_Z2P_m_1_l =
        exp_Z3_m_1 + exp_Z4_m_1 + exp_Z2P_m_1_lo; // < 45, -14 >
    assert(exp_Z2P_m_1_l[exp_Z2P_m_1_l.wl() - 1] == 0);
    ap_ufixed<wf + gbits - w_Z1 - w_Z2 + 1, -w_Z1 - w_Z2 + 1> exp_Z2P_m_1 =
        exp_Z2P_m_1_l; // < 44, -15 >

    // 2. e^Z1P - 1
    //  = ( 1 + Z2 + f(Z2) ) * ( 1 + ( e^Z2P - 1 ) ) - 1
    //  = ( Z2 + f(Z2) ) + ( e^Z2P - 1 ) + ( Z2 + f(Z2) ) * ( e^Z2P - 1 )
    //
    // 2.1 Z2 + f(Z2)
    ap_uint<w_Z2> Z2_ind = Z2(Z2.wl() - 1, 0);
    ap_ufixed<wf + gbits - 2 * w_Z1 - 1, -2 *w_Z1 - 1> f_Z2 =
        table_f_Z2<ap_ufixed<wf + gbits, 0>>::array[Z2_ind]; // < 42, -17 >
    ap_ufixed<wf + gbits - w_Z1 + 1 - 2, -w_Z1 + 1> exp_Z2_m_1 =
        Z2 + f_Z2; // < 52, -7 > -> < 50, -7> to save multipliers

    // 2.2 ( Z2 + f(Z2) ) * ( e^Z2P - 1 )
    ap_ufixed<wf + gbits - 2 * w_Z1 - w_Z2 + 2, -2 *w_Z1 - w_Z2 + 2>
        exp_Z1P_m_1_lo = exp_Z2_m_1 * exp_Z2P_m_1; // < 37, -22 >

    // 2.3 e^Z1P - 1
    ap_ufixed<wf + gbits - w_Z1 + 2, -w_Z1 + 2> exp_Z1P_m_1_l =
        exp_Z2_m_1 + exp_Z2P_m_1 + exp_Z1P_m_1_lo; // < 53, -6 >
    assert(exp_Z1P_m_1_l[exp_Z1P_m_1_l.wl() - 1] == 0);
    ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
        exp_Z1P_m_1_l; // < 50, -7 >

    return exp_Z1P_m_1;
  }
};

template <typename T> T exp_generic(T x) {
#pragma HLS pipeline

  fp_struct<T> es(x);
  const static int we = exp_traits<T>::we;
  const static int wf = exp_traits<T>::wf;

  fp_struct<T> out;
  out.sign[0] = 0;
  out.sig = 0;

  // special cases include:
  // y = 0, +inf, -inf, NaN
  //
  //      x	|       +inf	|	-inf	|       NaN
  // =============================================================
  // 	exp(x)	|	+inf	|	0	|	NaN
  //
  bool x_is_NaN = 0;
  bool x_is_inf = 0;
  bool x_is_pinf = 0;
  bool x_is_ninf = 0;

  if (::hls::__isnan(x))
    x_is_NaN = 1;
  if (::hls::__isinf(x))
    x_is_inf = 1;
  x_is_pinf = x_is_inf & ~es.sign[0];
  x_is_ninf = x_is_inf & es.sign[0];

  if (x_is_NaN) {
    // out = NaN
    // out.sign[0] = 0;
    out.sig = -1; // all 1's
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if (x_is_pinf) {
    // out = +inf
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if (x_is_ninf) {
    // out = 0
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  int m_exp = es.expv();
  fp_struct<T> nes = es;
#ifdef STDSUBNORMALS
  if (nes.exp == 0 && nes.sig != 0) {
    // subnormal handling.
    unsigned int zeros;
#pragma unroll
    for (zeros = 0; zeros < wf; zeros++)
      if (nes.sig[wf - zeros - 1] == 1)
        break;
    m_exp -= zeros;
    nes.sig = nes.sig << (zeros + 1); // add one so we shift off the leading one
  }
#endif
  ap_fixed<1 + 1 + wf, 1 + 1> e_frac = 0;
  e_frac[e_frac.wl() - e_frac.iwl()] = 1; // The implicit '1' in IEEE format.
  e_frac(e_frac.wl() - e_frac.iwl() - 1, 0) = nes.sig(wf - 1, 0);
  if (nes.sign)
    e_frac = -e_frac;

  const static int gbits = exp_traits<T>::gbits;
  ap_fixed<1 + we + wf + gbits, 1 + we> m_frac_l = e_frac;

  ap_fixed<1 + we + wf, 1 + we> m_fix_l =
      m_frac_l << m_exp; // used for overflow checking only
  ap_fixed<1 + we + wf, 1 + we> m_fix_back = m_fix_l >> m_exp;

  ap_fixed<1 + we + wf + gbits, 1 + we> m_fix;
  m_fix = m_frac_l << m_exp;

  ap_fixed<1 + we + 4, 1 + we> m_fix_hi = m_fix;

  const ap_ufixed<1 + we + 3, 1> LOG2R = 1.4426950408889634073599246810019;

  ap_fixed<2, 1> delta1;
  delta1[1] = m_fix[m_fix.wl() - 1];
  delta1[0] = 1;

  ap_int<1 + we + 1> r_exp = m_fix_hi * LOG2R + delta1;

  const ap_ufixed<wf + gbits + we + 1, 0> LOG2_hi =
      0.69314718055994517520446152047953;
  const ap_ufixed<wf + gbits + we + 1, 0> LOG2_lo =
      0.60444058366692929317548838826088;
  const ap_ufixed<wf + gbits + we + 1, 0> LOG2 = LOG2_hi + (LOG2_lo >> 52);

  ap_fixed<1 + we + wf + gbits, 1 + we + 1> m_fix_a =
      r_exp * LOG2; // m_fix approximation

  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a < 0.5)); // check r_exp zeros out integer and most
                                   // significant fraction bits
  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a > -0.5)); // check r_exp zeros out integer and most
                                    // significant fraction bits
  ap_fixed<1 - 1 + wf + gbits, 1 - 1> m_diff = m_fix - m_fix_a;

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

  const static int w_Z1 = exp_traits<T>::w_Z1;
  // Z1
  ap_uint<w_Z1> m_diff_hi = m_diff(m_diff.wl() - 1, m_diff.wl() - w_Z1);
  // Z1P = Z2 + ... + Zk
  ap_ufixed<wf + gbits - w_Z1, -w_Z1> m_diff_lo =
      m_diff; // ( m_diff.wl()-m_diff.iwl()-w_Z1-1 , 0 );

  // e^Z1 by table_exp_Z1
  const static int gbits_Z2 = exp_traits<T>::gbits_Z2;
  ap_ufixed<1 + wf + gbits_Z2, 1> exp_Z1 =
      table_exp_Z1<ap_ufixed<1 + wf + gbits_Z2, 1>>::array[m_diff_hi];
  ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
      exp_traits<T>::exp_Z1P_m_1(m_diff_lo);
  ap_ufixed<1 + wf + gbits_Z2 - w_Z1, 1> exp_Z1_hi = exp_Z1;
  ap_ufixed<1, -wf> delta;
  delta[0] = 1;
  ap_ufixed<2 + wf + gbits_Z2, 2> exp_Y_l =
      (exp_Z1 + delta) + exp_Z1_hi * exp_Z1P_m_1;
  assert(exp_Y_l[exp_Y_l.wl() - 1] == 0);
  ap_ufixed<1 + wf + gbits_Z2, 1> exp_Y = exp_Y_l;

  if (exp_Y[exp_Y.wl() - 1] == 0) {
    exp_Y = exp_Y << 1;
    r_exp = r_exp - 1;
  }

  // check overflow here
  if ((m_exp > 0 && m_fix_back != m_frac_l) ||
      (r_exp > fp_struct<T>::EXP_BIAS)) {
    if (~m_frac_l[m_frac_l.wl() - 1]) {
      // out = +inf
      // out.sign[0] = 0;
      // out.sig = 0;
      out.exp = fp_struct<T>::EXP_INFNAN;
      return out.to_ieee();
    } else {
      // out = 0
      // out.sign[0] = 0;
      // out.sig = 0;
      out.exp = 0;
      return out.to_ieee();
    }
  }

  // check underflow here
  if (r_exp <= -fp_struct<T>::EXP_BIAS) {
    // out = 0
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // let's output the result
  out.sig(wf - 1, 0) = exp_Y(exp_Y.wl() - 1 - 1, exp_Y.wl() - 1 - wf);
  out.exp = fp_struct<T>::EXP_BIAS + r_exp;
  return out.to_ieee();
}

// exp() for fixed point data type ap_fixed<16,8>
// input x is divided into x_hi and x_lo
// x_hi consists the most significant bits
// and x_lo consists the least significant bits
// exp(x) = exp(x_hi+x_lo) = exp(x_hi) * exp(x_lo)
// x_hi and x_lo are used as indexes to look up their exponential values
//
// to save memory, exp(x_lo) = 1 + x_lo + f(x_lo),
// only f(x_lo) is stored in the table
//
static ap_fixed<16, 8> exp_generic(ap_fixed<16, 8> x) {
#pragma HLS pipeline

  const static int W_ = 16;  // total bit width
  const static int I_ = 8;   // width of integer, including sign bit
  const static int F_ = 8;   // width of fraction bits
  const static int w_hi = 6; // width of x_hi
  const static int w_lo = 6; // width of x_lo

  const static int gbits = 7;

  ap_uint<w_hi> x_hi_ind;
  x_hi_ind[w_hi - 1] = x[W_ - 1];
  x_hi_ind(w_hi - 2, 0) = x(w_hi + w_lo - 2, w_lo);
  ap_uint<w_lo> x_lo_ind = x(w_lo - 1, 0);

  ap_ufixed<W_ - 1 + gbits, I_ - 1> exp_x_hi; // = exp_x_hi_array [ x_hi_ind ];
  exp_x_hi(W_ - 1 + gbits - 1, F_ + gbits) = table_exp_x_hi::array_0[x_hi_ind];
  exp_x_hi(F_ + gbits - 1, gbits) = table_exp_x_hi::array_1[x_hi_ind];
  exp_x_hi(gbits - 1, 0) = table_exp_x_hi::array_2[x_hi_ind];

  ap_ufixed<F_ + (w_lo - F_) * 2 + gbits, (w_lo - F_) * 2> f_x_lo =
      table_f_x_lo::array[x_lo_ind];

  ap_ufixed<w_lo, w_lo - F_> x_lo = x;
  ap_ufixed<w_lo + 1 + gbits, w_lo - F_ + 1> exp_x_lo_m_1 = x_lo + f_x_lo;
  ap_ufixed<w_lo + 1 + gbits, I_ - 1> exp_x_hi_hi = exp_x_hi;
  ap_ufixed<1, -F_> delta; // for rounding to nearest
  delta[0] = 1;
  ap_ufixed<W_ - 1 + gbits, I_ - 1> exp_x_l =
      (exp_x_hi + delta) + exp_x_hi_hi * exp_x_lo_m_1;

  ap_fixed<W_, I_> exp_x = exp_x_l;

  if (x > 4.8515625)
    exp_x = 127.99609375;
  if (x < -6)
    exp_x = 0;

  return exp_x;
}

// exp() for fixed point data type ap_fixed<8,4>
// input x is used as index to look up the exponential value
//
static ap_fixed<8, 4> exp_generic(ap_fixed<8, 4> x) {
#pragma HLS pipeline

  const static int W_ = 8; // total bit width
  const static int I_ = 4; // width of integer, including sign bit

  ap_uint<W_ - 1> x_ind;
  x_ind(W_ - 2, 0) = x(W_ - 2, 0);

  ap_fixed<W_, I_> exp_x;
  exp_x[7] = 0;
  exp_x(6, 4) = table_exp_x::array_0[x_ind];
  exp_x(3, 0) = table_exp_x::array_1[x_ind];

  if (x >= 2.125)
    exp_x = 7.9375;
  if (x < -3)
    exp_x = 0;

  return exp_x;
}

static half exp(half x) { return exp_generic(x); }
static float exp(float x) { return exp_generic(x); }
static double exp(double x) { return exp_generic(x); }
static ap_fixed<16, 8> exp(ap_fixed<16, 8> x) { return exp_generic(x); }
static ap_fixed<8, 4> exp(ap_fixed<8, 4> x) { return exp_generic(x); }

static float expf(float x) { return exp_generic(x); }

static half half_exp(half x) { return exp_generic(x); }

template <typename T> T exp2_generic(T x) {
#pragma HLS pipeline

  fp_struct<T> es(x);
  const static int we = exp_traits<T>::we;
  const static int wf = exp_traits<T>::wf;

  fp_struct<T> out;
  out.sign[0] = 0;
  out.sig = 0;

  // special cases include:
  // y = 0, +inf, -inf, NaN
  //
  //      x	|       +inf	|	-inf	|       NaN
  // =============================================================
  // 	exp(x)	|	+inf	|	0	|	NaN
  //
  bool x_is_NaN = 0;
  bool x_is_inf = 0;
  bool x_is_pinf = 0;
  bool x_is_ninf = 0;

  if (::hls::__isnan(x))
    x_is_NaN = 1;
  if (::hls::__isinf(x))
    x_is_inf = 1;
  x_is_pinf = x_is_inf & ~es.sign[0];
  x_is_ninf = x_is_inf & es.sign[0];

  if (x_is_NaN) {
    // out = NaN
    // out.sign[0] = 0;
    out.sig = -1; // all 1's
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if (x_is_pinf) {
    // out = +inf
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if (x_is_ninf) {
    // out = 0
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  int m_exp = es.expv();
  fp_struct<T> nes = es;
#ifdef STDSUBNORMALS
  if (nes.exp == 0 && nes.sig != 0) {
    // subnormal handling.
    unsigned int zeros;
#pragma unroll
    for (zeros = 0; zeros < wf; zeros++)
      if (nes.sig[wf - zeros - 1] == 1)
        break;
    m_exp -= zeros;
    nes.sig = nes.sig << (zeros + 1); // add one so we shift off the leading one
  }
#endif
  ap_fixed<1 + 1 + wf, 1 + 1> e_frac = 0;
  e_frac[e_frac.wl() - e_frac.iwl()] = 1; // The implicit '1' in IEEE format.
  e_frac(e_frac.wl() - e_frac.iwl() - 1, 0) = nes.sig(wf - 1, 0);
  if (nes.sign)
    e_frac = -e_frac;

  const static int gbits = exp_traits<T>::gbits;
  ap_fixed<1 + we + wf + gbits, 1 + we> m_frac_l = e_frac;

  ap_fixed<1 + we + wf, 1 + we> m_fix_l =
      m_frac_l << m_exp; // used for overflow checking only
  ap_fixed<1 + we + wf, 1 + we> m_fix_back = m_fix_l >> m_exp;

  ap_fixed<1 + we + wf + gbits, 1 + we> m_fix;
  m_fix = m_frac_l << m_exp;

  ap_fixed<2, 1> delta1;
  delta1[1] = m_fix[m_fix.wl() - 1];
  delta1[0] = 1;

  ap_int<1 + we + 1> r_exp = m_fix + delta1;

  const ap_ufixed<wf + gbits + we + 1, 0> LOG2_hi =
      0.69314718055994517520446152047953;
  const ap_ufixed<wf + gbits + we + 1, 0> LOG2_lo =
      0.60444058366692929317548838826088;
  const ap_ufixed<wf + gbits + we + 1, 0> LOG2 = LOG2_hi + (LOG2_lo >> 52);

  ap_fixed<1 + we + wf + gbits, 1 + we + 1> m_fix_a =
      r_exp; // m_fix approximation

  assert(
      (m_fix_back != m_frac_l) ||
      (m_fix - m_fix_a <
       1)); // check r_exp zeros out integer and most significant fraction bits
  assert(
      (m_fix_back != m_frac_l) ||
      (m_fix - m_fix_a >
       -1)); // check r_exp zeros out integer and most significant fraction bits
  ap_fixed<1 + wf + gbits, 1> m_diff_l = (m_fix - m_fix_a) * LOG2;

  assert((m_fix_back != m_frac_l) ||
         (m_diff_l < 0.5)); // check r_exp zeros out integer and most
                            // significant fraction bits
  assert((m_fix_back != m_frac_l) ||
         (m_diff_l > -0.5)); // check r_exp zeros out integer and most
                             // significant fraction bits
  ap_fixed<1 - 1 + wf + gbits, 1 - 1> m_diff = m_diff_l;

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

  const static int w_Z1 = exp_traits<T>::w_Z1;
  // Z1
  ap_uint<w_Z1> m_diff_hi = m_diff(m_diff.wl() - 1, m_diff.wl() - w_Z1);
  // Z1P = Z2 + ... + Zk
  ap_ufixed<wf + gbits - w_Z1, -w_Z1> m_diff_lo =
      m_diff; // ( m_diff.wl()-m_diff.iwl()-w_Z1-1 , 0 );

  // e^Z1 by table_exp_Z1
  const static int gbits_Z2 = exp_traits<T>::gbits_Z2;
  ap_ufixed<1 + wf + gbits_Z2, 1> exp_Z1 =
      table_exp_Z1<ap_ufixed<1 + wf + gbits_Z2, 1>>::array[m_diff_hi];
  ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
      exp_traits<T>::exp_Z1P_m_1(m_diff_lo);
  ap_ufixed<1 + wf + gbits_Z2 - w_Z1, 1> exp_Z1_hi = exp_Z1;
  ap_ufixed<1, -wf> delta;
  delta[0] = 1;
  ap_ufixed<2 + wf + gbits_Z2, 2> exp_Y_l =
      (exp_Z1 + delta) + exp_Z1_hi * exp_Z1P_m_1;
  assert(exp_Y_l[exp_Y_l.wl() - 1] == 0);
  ap_ufixed<1 + wf + gbits_Z2, 1> exp_Y = exp_Y_l;

  if (exp_Y[exp_Y.wl() - 1] == 0) {
    exp_Y = exp_Y << 1;
    r_exp = r_exp - 1;
  }

  // check overflow here
  if ((m_exp > 0 && m_fix_back != m_frac_l) ||
      (r_exp > fp_struct<T>::EXP_BIAS)) {
    if (~m_frac_l[m_frac_l.wl() - 1]) {
      // out = +inf
      // out.sign[0] = 0;
      // out.sig = 0;
      out.exp = fp_struct<T>::EXP_INFNAN;
      return out.to_ieee();
    } else {
      // out = 0
      // out.sign[0] = 0;
      // out.sig = 0;
      out.exp = 0;
      return out.to_ieee();
    }
  }

  // check underflow here
  if (r_exp <= -fp_struct<T>::EXP_BIAS) {
    // out = 0
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // let's output the result
  out.sig(wf - 1, 0) = exp_Y(exp_Y.wl() - 1 - 1, exp_Y.wl() - 1 - wf);
  out.exp = fp_struct<T>::EXP_BIAS + r_exp;
  return out.to_ieee();
}

static half exp2(half x) { return exp2_generic(x); }
static float exp2(float x) { return exp2_generic(x); }
static double exp2(double x) { return exp2_generic(x); }

static float exp2f(float x) { return exp2_generic(x); }

static half half_exp2(half x) { return exp2_generic(x); }

template <typename T> T exp10_generic(T x) {
#pragma HLS pipeline

  fp_struct<T> es(x);
  const static int we = exp_traits<T>::we;
  const static int wf = exp_traits<T>::wf;

  fp_struct<T> out;
  out.sign[0] = 0;
  out.sig = 0;

  // special cases include:
  // y = 0, +inf, -inf, NaN
  //
  //      x	|       +inf	|	-inf	|       NaN
  // =============================================================
  // 	exp(x)	|	+inf	|	0	|	NaN
  //
  bool x_is_NaN = 0;
  bool x_is_inf = 0;
  bool x_is_pinf = 0;
  bool x_is_ninf = 0;

  if (::hls::__isnan(x))
    x_is_NaN = 1;
  if (::hls::__isinf(x))
    x_is_inf = 1;
  x_is_pinf = x_is_inf & ~es.sign[0];
  x_is_ninf = x_is_inf & es.sign[0];

  if (x_is_NaN) {
    // out = NaN
    // out.sign[0] = 0;
    out.sig = -1; // all 1's
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if (x_is_pinf) {
    // out = +inf
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if (x_is_ninf) {
    // out = 0
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  int m_exp = es.expv();
  fp_struct<T> nes = es;
#ifdef STDSUBNORMALS
  if (nes.exp == 0 && nes.sig != 0) {
    // subnormal handling.
    unsigned int zeros;
#pragma unroll
    for (zeros = 0; zeros < wf; zeros++)
      if (nes.sig[wf - zeros - 1] == 1)
        break;
    m_exp -= zeros;
    nes.sig = nes.sig << (zeros + 1); // add one so we shift off the leading one
  }
#endif
  ap_fixed<1 + 1 + wf, 1 + 1> e_frac = 0;
  e_frac[e_frac.wl() - e_frac.iwl()] = 1; // The implicit '1' in IEEE format.
  e_frac(e_frac.wl() - e_frac.iwl() - 1, 0) = nes.sig(wf - 1, 0);
  if (nes.sign)
    e_frac = -e_frac;

  const static int gbits = exp_traits<T>::gbits;
  ap_fixed<1 + we + wf + gbits, 1 + we> m_frac_l = e_frac;

  ap_fixed<1 + we + wf, 1 + we> m_fix_l =
      m_frac_l << m_exp; // used for overflow checking only
  ap_fixed<1 + we + wf, 1 + we> m_fix_back = m_fix_l >> m_exp;

  ap_fixed<1 + we + wf + gbits, 1 + we> m_fix;
  m_fix = m_frac_l << m_exp;

  ap_fixed<1 + we + 5, 1 + we> m_fix_hi = m_fix;

  const ap_ufixed<2 + we + 3, 2> LOG2_10 = 3.3219280948873623478703194294894;

  ap_fixed<2, 1> delta1;
  delta1[1] = m_fix[m_fix.wl() - 1];
  delta1[0] = 1;

  ap_int<1 + we + 2> r_exp = m_fix_hi * LOG2_10 + delta1;

  const ap_ufixed<2 + wf + gbits + we + 1, 2> LOG10_hi =
      2.302585092994045679049008867878;
  const ap_ufixed<-5 + wf + gbits + we + 1, -5> LOG10_lo =
      0.02237830812635162513896609861137;
  const ap_ufixed<2 + wf + gbits + we + 1, 2> LOG10 =
      LOG10_hi + (LOG10_lo >> 52);

  const ap_ufixed<-1 + wf + gbits + we + 3, -1> LOG10_2_hi =
      0.30102999566398103148401332873618;
  const ap_ufixed<wf + gbits + we + 3, 0> LOG10_2_lo =
      0.73737313104845853883700312679039;
  const ap_ufixed<-1 + wf + gbits + we + 3, -1> LOG10_2 =
      LOG10_2_hi + (LOG10_2_lo >> 52);

  ap_fixed<1 + we + 1 + wf + gbits, 1 + we + 1> m_fix_a =
      r_exp * LOG10_2; // m_fix approximation

  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a < 0.25)); // check r_exp zeros out integer and most
                                    // significant fraction bits
  assert((m_fix_back != m_frac_l) ||
         (m_fix - m_fix_a > -0.25)); // check r_exp zeros out integer and most
                                     // significant fraction bits
  ap_fixed<1 - 1 + wf + gbits, 1 - 1> m_diff = (m_fix - m_fix_a) * LOG10;

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

  const static int w_Z1 = exp_traits<T>::w_Z1;
  // Z1
  ap_uint<w_Z1> m_diff_hi = m_diff(m_diff.wl() - 1, m_diff.wl() - w_Z1);
  // Z1P = Z2 + ... + Zk
  ap_ufixed<wf + gbits - w_Z1, -w_Z1> m_diff_lo =
      m_diff; // ( m_diff.wl()-m_diff.iwl()-w_Z1-1 , 0 );

  // e^Z1 by table_exp_Z1
  const static int gbits_Z2 = exp_traits<T>::gbits_Z2;
  ap_ufixed<1 + wf + gbits_Z2, 1> exp_Z1 =
      table_exp_Z1<ap_ufixed<1 + wf + gbits_Z2, 1>>::array[m_diff_hi];
  ap_ufixed<wf + gbits_Z2 - w_Z1 + 1, -w_Z1 + 1> exp_Z1P_m_1 =
      exp_traits<T>::exp_Z1P_m_1(m_diff_lo);
  ap_ufixed<1 + wf + gbits_Z2 - w_Z1, 1> exp_Z1_hi = exp_Z1;
  ap_ufixed<1, -wf> delta;
  delta[0] = 1;
  ap_ufixed<2 + wf + gbits_Z2, 2> exp_Y_l =
      (exp_Z1 + delta) + exp_Z1_hi * exp_Z1P_m_1;
  assert(exp_Y_l[exp_Y_l.wl() - 1] == 0);
  ap_ufixed<1 + wf + gbits_Z2, 1> exp_Y = exp_Y_l;

  if (exp_Y[exp_Y.wl() - 1] == 0) {
    exp_Y = exp_Y << 1;
    r_exp = r_exp - 1;
  }

  // check overflow here
  if ((m_exp > 0 && m_fix_back != m_frac_l) ||
      (r_exp > fp_struct<T>::EXP_BIAS)) {
    if (~m_frac_l[m_frac_l.wl() - 1]) {
      // out = +inf
      // out.sign[0] = 0;
      // out.sig = 0;
      out.exp = fp_struct<T>::EXP_INFNAN;
      return out.to_ieee();
    } else {
      // out = 0
      // out.sign[0] = 0;
      // out.sig = 0;
      out.exp = 0;
      return out.to_ieee();
    }
  }

  // check underflow here
  if (r_exp <= -fp_struct<T>::EXP_BIAS) {
    // out = 0
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  // let's output the result
  out.sig(wf - 1, 0) = exp_Y(exp_Y.wl() - 1 - 1, exp_Y.wl() - 1 - wf);
  out.exp = fp_struct<T>::EXP_BIAS + r_exp;
  return out.to_ieee();
}

static half exp10(half x) { return exp10_generic(x); }
static float exp10(float x) { return exp10_generic(x); }
static double exp10(double x) { return exp10_generic(x); }

static float exp10f(float x) { return exp10_generic(x); }

static half half_exp10(half x) { return exp10_generic(x); }

static double expm1(double x) {
  fp_struct<double> xs(x);
  if ((xs.sign[0] == 1) && (xs.exp < 996) && (xs.exp != 0))
    return x;
  if ((xs.sign[0] == 0) && (xs.exp <= 996) && (xs.exp != 0))
    return x;
  return exp_generic(x) - 1;
}

static float expm1(float x) {
  fp_struct<float> xs(x);
  if (xs.exp == 0)
    return 0;
  if (xs.exp < 96)
    return x;
  double xd = x;
  return exp_generic(xd) - 1;
}

static half expm1(half x) {
  float xf = x;
  return exp_generic(xf) - 1;
}

} // namespace exp_reduce_
