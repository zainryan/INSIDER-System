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
 * @file hls_log.h
 *
 * Natural log implementation using range reduction + Taylor series
 * see: de Dinechin, "A flexible floating-point logarithm for reconfigurable
 * computers"
 */
namespace log_reduce {

#include "hls_log_tables.h"

template <typename LOG_TYPE, int p, int alpha, int size, int sizeout>
void range_reduce(ap_ufixed<size, -p> zN, LOG_TYPE &log,
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

template <typename T> class log_traits {};

template <> class log_traits<half> {
public:
  const static int we = fp_struct<half>::EXP_BITS;
  const static int wf = 15;
  const static int org_wf = fp_struct<half>::SIG_BITS; // 10

  const static int gbits = 0;
  const static int MaxPrecision = p3 + wf + 1 + gbits;      // 35
  typedef ap_fixed<1 + we + MaxPrecision, 1 + we> LOG_TYPE; // < 41, 6 >

  const static int w1 = wf + 1;
  const static int w2 =
      w1 + 2 * p1 + 1 -
      (alpha1 - 1); // MaxWidth of z2, needs to be truncted to MaxPrecision
  const static int w3 = w2 + 2 * p2 + 1 - (alpha2 - 1);

  template <int size>
  static ap_ufixed<MaxPrecision - p3, -p3>
  range_reduction(ap_ufixed<size, -p1> z1, LOG_TYPE &log_sum) {
    ap_ufixed<(w2 < MaxPrecision - p2) ? w2 : (MaxPrecision - p2), -p2> z2;
    ap_ufixed<(w3 < MaxPrecision - p3) ? w3 : (MaxPrecision - p3), -p3> z3;
    LOG_TYPE logn;
    range_reduce<LOG_TYPE, p1, alpha1>(z1, logn, z2);
    log_sum += logn;
    range_reduce<LOG_TYPE, p2, alpha2>(z2, logn, z3);
    log_sum += logn;
    return z3;
  }
};
template <> class log_traits<float> {
public:
  const static int we = fp_struct<float>::EXP_BITS;
  const static int wf = 31;
  const static int org_wf = fp_struct<float>::SIG_BITS; // 23

  const static int gbits = 0;
  const static int MaxPrecision = p4 + wf + 1 + gbits;      // 56
  typedef ap_fixed<1 + we + MaxPrecision, 1 + we> LOG_TYPE; // < 65, 9 >

  const static int w1 = wf + 1;
  const static int w2 =
      w1 + 2 * p1 + 1 -
      (alpha1 - 1); // MaxWidth of z2, needs to be truncted to MaxPrecision
  const static int w3 = w2 + 2 * p2 + 1 - (alpha2 - 1);
  const static int w4 = w3 + 2 * p3 + 1 - (alpha3 - 1);

  template <int size>
  static ap_ufixed<MaxPrecision - p4, -p4>
  range_reduction(ap_ufixed<size, -p1> z1, LOG_TYPE &log_sum) {
    ap_ufixed<(w2 < MaxPrecision - p2) ? w2 : (MaxPrecision - p2), -p2> z2;
    ap_ufixed<(w3 < MaxPrecision - p3) ? w3 : (MaxPrecision - p3), -p3> z3;
    ap_ufixed<(w4 < MaxPrecision - p4) ? w4 : (MaxPrecision - p4), -p4> z4;
    LOG_TYPE logn;
    range_reduce<LOG_TYPE, p1, alpha1>(z1, logn, z2);
    log_sum += logn;
    range_reduce<LOG_TYPE, p2, alpha2>(z2, logn, z3);
    log_sum += logn;
    range_reduce<LOG_TYPE, p3, alpha3>(z3, logn, z4);
    log_sum += logn;
    return z4;
  }
};

template <> class log_traits<double> {
public:
  const static int we = fp_struct<double>::EXP_BITS;
  const static int wf = 52;
  const static int org_wf = fp_struct<double>::SIG_BITS; // 52

  const static int gbits = 0;
  const static int MaxPrecision = p8 + wf + 1 + gbits;      // 109
  typedef ap_fixed<1 + we + MaxPrecision, 1 + we> LOG_TYPE; // < 121, 12>

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
  static ap_ufixed<MaxPrecision - p8, -p8>
  range_reduction(ap_ufixed<size, -p1> z1, LOG_TYPE &log_sum) {
    ap_ufixed<(w2 < MaxPrecision - p2) ? w2 : (MaxPrecision - p2), -p2> z2;
    ap_ufixed<(w3 < MaxPrecision - p3) ? w3 : (MaxPrecision - p3), -p3> z3;
    ap_ufixed<(w4 < MaxPrecision - p4) ? w4 : (MaxPrecision - p4), -p4> z4;
    ap_ufixed<(w5 < MaxPrecision - p5) ? w5 : (MaxPrecision - p5), -p5> z5;
    ap_ufixed<(w6 < MaxPrecision - p6) ? w6 : (MaxPrecision - p6), -p6> z6;
    ap_ufixed<(w7 < MaxPrecision - p7) ? w7 : (MaxPrecision - p7), -p7> z7;
    ap_ufixed<(w8 < MaxPrecision - p8) ? w8 : (MaxPrecision - p8), -p8> z8;
    LOG_TYPE logn;

    range_reduce<LOG_TYPE, p1, alpha1>(z1, logn, z2);
    log_sum += logn;
    range_reduce<LOG_TYPE, p2, alpha2>(z2, logn, z3);
    log_sum += logn;
    range_reduce<LOG_TYPE, p3, alpha3>(z3, logn, z4);
    log_sum += logn;
    range_reduce<LOG_TYPE, p4, alpha4>(z4, logn, z5);
    log_sum += logn;
    range_reduce<LOG_TYPE, p5, alpha5>(z5, logn, z6);
    log_sum += logn;
    range_reduce<LOG_TYPE, p6, alpha6>(z6, logn, z7);
    log_sum += logn;
    range_reduce<LOG_TYPE, p7, alpha7>(z7, logn, z8);
    log_sum += logn;
    return z8;
  }
};

template <typename T> T log_generic(T base) {
#pragma HLS pipeline

  fp_struct<T> bs(base);
  const static int we = log_traits<T>::we;
  const static int wf = log_traits<T>::wf;
  const static int org_wf = log_traits<T>::org_wf;

  fp_struct<T> out;
  out.sign[0] = 0;
  out.sig = 0;

  int b_exp = bs.expv();

  // 	x	|    NaN / <0	|	+/-0	|	+inf	|	normal
  // =============================================================================
  // 	log(x)	|	NaN	|	-inf	|	+inf	|
  //
  bool x_is_0 = 0;
  bool x_is_NaN = 0;
  bool x_is_neg = 0; // not strongly correct, including -0 and -inf
  bool x_is_inf = 0;

#ifndef STDSUBNORMALS
  if (bs.exp == 0)
#else
  if (bs.exp == 0 && bs.sig == 0)
#endif
    x_is_0 = 1;
  if (::hls::__isnan(base))
    x_is_NaN = 1;
  if (bs.sign[0] == 1)
    x_is_neg = 1;
  if (::hls::__isinf(base))
    x_is_inf = 1;

  if (x_is_0) {
    // out = -inf
    out.sign[0] = 1;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if (x_is_NaN | x_is_neg) {
    // out = NaN
    // out.sign[0] = 0;
    out.sig = -1; // all 1's
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }
  if (x_is_inf) {
    // out = +inf
    // out.sign[0] = 0;
    // out.sig = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    return out.to_ieee();
  }

  const static int bypass_threshold = wf / 2;
  const static int MaxPrecision = log_traits<T>::MaxPrecision;
  typedef typename log_traits<T>::LOG_TYPE LOG_TYPE;
  LOG_TYPE sum;
  LOG_TYPE log_sum;
  LOG_TYPE log_base;

  // storing exp and frac of base using int b_exp and ap_ufixed<1+wf+1,1> b_frac
  // int b_exp = bs.expv();
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
  const ap_ufixed<wf + we + 2, 0> LOG2_lo = 0.60444058366692929317548838826088;
  const ap_ufixed<wf + we + 2, 0> LOG2 = LOG2_hi + (LOG2_lo >> 52);

  ap_fixed<1 + we + wf + we + 2, 1 + we> Elog2 = LOG2 * b_exp;

  const int T0size = 1 << (1 + alpha0); // The size of the first table

#define inverse_lut log_inverse_lut_table<p0, alpha0, T0size>::array
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

  ap_fixed<1 - bypass_threshold + 1 + MaxPrecision, -bypass_threshold + 1> zk;
  zk = log_traits<T>::range_reduction(z1, log_sum);

  ap_fixed<1 - 2 * bypass_threshold + 1 + MaxPrecision, -bypass_threshold + 1>
      zk_trunc = zk;
  sum = zk - (zk_trunc * zk_trunc / 2);
  // sum = zk*2/(zk+2);

  log_base = Elog2 + log_sum + sum;
#undef log_lut
#undef inverse_lut

  // let's output the result
  out.sign[0] = log_base[log_base.wl() - 1];
  if (log_base[log_base.wl() - 1]) {
    log_base = -log_base;
  }

  int r_exp = 0;
  int pone;
#pragma unroll
  for (pone = we - 1; pone > -MaxPrecision + 1; pone--) {
    if (log_base[pone + MaxPrecision] == 1) {
      r_exp = pone;
      break;
    }
  }

  if (r_exp != pone) {
    // out = 0
    out.sign[0] = 0;
    // out.sig = 0;
    out.exp = 0;
    return out.to_ieee();
  }

  ap_fixed<1 + MaxPrecision + org_wf, 1> delta = 0;
  delta[delta.wl() - 1] = log_base[log_base.wl() - 1];
  delta[delta.wl() - 2] = 1;
  ap_fixed<1 + we + MaxPrecision + org_wf, 1 + we> log_base_l =
      log_base + (delta >> (org_wf - r_exp));

  if (log_base_l[r_exp + 1 + MaxPrecision + org_wf] == 1) {
    r_exp++;
  }

  out.exp = fp_struct<T>::EXP_BIAS + r_exp;
  out.sig(org_wf - 1, 0) =
      log_base_l(r_exp + MaxPrecision + org_wf - 1, r_exp + MaxPrecision);
  return out.to_ieee();
}

static half log(half base) { return log_generic(base); }
static float log(float base) { return log_generic(base); }
static double log(double base) { return log_generic(base); }

static float logf(float base) { return log_generic(base); }

static half half_log(half base) { return log_generic(base); }

static half log2(half base) {
  return log_generic(base) *
         static_cast<half>(1.4426950408889634073599246810019);
}
static float log2(float base) {
  return HLS_FPO_MULF(log_generic(base), 1.4426950408889634073599246810019f);
}
static double log2(double base) {
  return HLS_FPO_MUL(log_generic(base), 1.4426950408889634073599246810019);
}

static float log2f(float base) {
  return HLS_FPO_MULF(log_generic(base), 1.4426950408889634073599246810019f);
}

static half half_log2(half base) {
  return log_generic(base) *
         static_cast<half>(1.4426950408889634073599246810019);
}

static half log10(half base) {
  return log_generic(base) *
         static_cast<half>(0.43429448190325182765112891891661);
}
static float log10(float base) {
  return HLS_FPO_MULF(log_generic(base), 0.43429448190325182765112891891661f);
}
static double log10(double base) {
  return HLS_FPO_MUL(log_generic(base), 0.43429448190325182765112891891661);
}

static float log10f(float base) {
  return HLS_FPO_MULF(log_generic(base), 0.43429448190325182765112891891661f);
}

static half half_log10(half base) {
  return log_generic(base) *
         static_cast<half>(0.43429448190325182765112891891661);
}

static half logb(half base) {
  return log_generic(fabs(base)) *
         static_cast<half>(1.4426950408889634073599246810019);
}
static float logb(float base) {
  return HLS_FPO_MULF(log_generic(fabs(base)),
                      1.4426950408889634073599246810019f);
}
static double logb(double base) {
  return HLS_FPO_MUL(log_generic(fabs(base)),
                     1.4426950408889634073599246810019);
}

static float logbf(float base) {
  return HLS_FPO_MULF(log_generic(fabs(base)),
                      1.4426950408889634073599246810019f);
}

static half half_logb(half base) {
  return log_generic(fabs(base)) *
         static_cast<half>(1.4426950408889634073599246810019);
}

static double log1p(double x) {
  fp_struct<double> xs(x);
  if (xs.exp == 0)
    return 0;
  if (xs.sign[0] & ::hls::__isinf(x)) {
    xs.sig = -1;
    return xs.to_ieee();
  }
  if (xs.exp == 2047)
    return x;
  ap_uint<2> xexp_h;
  xexp_h(1, 0) = xs.exp(10, 9);
  if (xexp_h == 0)
    return x;
  fp_struct<double> ys;
  ys.sign[0] = 0;
  ap_uint<12> yexp = xs.exp;
  yexp <<= 1;
  ap_uint<2> yexp_h;
  yexp_h(1, 0) = yexp(11, 10);
  yexp_h--;
  yexp(11, 10) = yexp_h(1, 0);
  ys.exp(10, 0) = yexp(10, 0);
  ap_ufixed<53, 1> xsig;
  xsig[52] = 1;
  xsig(51, 0) = xs.sig(51, 0);
  ap_ufixed<106, 2> ysig = xsig * xsig;
  if (ysig[105]) {
    ys.exp++;
    ysig >>= 1;
  }
  ys.sig(51, 0) = ysig(103, 52);
  if (xs.exp <= 998)
    return x - ys.to_ieee();
  double xd1p = x + 1;
  return log_generic(xd1p) - ((xd1p - 1) - x) / xd1p;
}
static float log1p(float x) {
  fp_struct<float> xs(x);
  if (xs.exp == 0)
    return 0;
  if (xs.sign[0] & ::hls::__isinf(x)) {
    xs.sig = -1;
    return xs.to_ieee();
  }
  if (xs.exp == 255)
    return x;
  float xf1p = x + 1;
  return log_generic(xf1p) - ((xf1p - 1) - x) / xf1p;
}
static half log1p(half x) {
  fp_struct<half> xs(x);
  if (xs.exp == 0)
    return 0;
  if (xs.sign[0] == 1 && xs.exp == 15 && xs.sig == 0) {
    xs.exp = 31;
    return xs.to_ieee();
  }
  if (xs.sign[0] & ::hls::__isinf(x)) {
    xs.sig = -1;
    return xs.to_ieee();
  }
  if (xs.exp == 31)
    return x;
  float xf1p = x + 1;
  return log_generic(xf1p) - ((xf1p - 1) - x) / xf1p;
}

} // namespace log_reduce
