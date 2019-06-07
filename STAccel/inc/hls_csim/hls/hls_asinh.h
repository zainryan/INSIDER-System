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
 *     OR INFORMATION AS ONE_HERE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
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
 *     (c) Copyright 2012-2017 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/**
 * @file hls_asinh.h
 */
#ifndef X_HLS_ASINH_H
#define X_HLS_ASINH_H

namespace explog_based {

template <class T> T generic_asinh(T x) {
  const T ONE_HERE = 1.0;
  const int W = fp_struct<T>::EXP_BITS + fp_struct<T>::SIG_BITS + 1;
  const int E = fp_struct<T>::EXP_BITS;
  const double LN2 = 6.93147180559945286227e-01;
  T ln2_t(LN2);
  fp_struct<T> fx(x);
  fp_struct<T> fxa(x);
  fp_struct<T> fx2(x);
  fx2.exp += 1;
  fx2.sign = 0;
  fxa.sign = 0;
  T x_abs = fxa.to_ieee();
  T x2 = fx2.to_ieee();

  // The value input is seperated :into 5  segments
  // 1) NAN/INF: |x| >= INF, function returns x;
  // 2) ST1: 2^28/14/7< |x| < INF, reutrns log (fabs (x)) + ln2
  // 3) ST2: ST1 >= |x| > 2.0, log, sqr, sqrt and divide are used by return
  // value 4) ST3: |x|< 2^-28/-14/-7<=|x|<ST2, log1p, sqr, sqrt and divide are
  // used 5) SML: |x|< 2^-28/-14/-7  function returns x There are 3 segments
  // flag variables needed  to be defined, which used different range_seg_exp
  // for different data-type.
  const int range_seg_exp = 28 >> (64 / W - 1);
  fp_struct<T> ST1((T)1.0);
  ST1.exp += range_seg_exp; // double 28; float 14;half 7;
  fp_struct<T> ST2((T)2.0);
  fp_struct<T> ST3((T)1.0);
  ST3.exp -= range_seg_exp; // double -28; float -14;half -7;
  bool is1sg = false;
  bool is2sg = false;
  bool is3sg = false;

  if (fxa.exp == ((1 << E) - 1)) // NAN/INF
    return x;
  else if (x_abs > ST1.to_ieee()) // ST1
    is1sg = true;
  else if (x_abs > ST2.to_ieee()) // ST2
    is2sg = true;
  else if (x_abs >= ST3.to_ieee()) // SML//ST3
    is3sg = true;
  else
    return x; // SML
  // sqr and sqrt
  T x_sqr = x * x;
  T in_sqrt = x_sqr + ONE_HERE;
  T out_sqrt = hls::sqrt(in_sqrt);
  // divide
  T div2 = out_sqrt + x_abs;
  T div3 = out_sqrt + ONE_HERE;
  T inx_div = (T)0.0;
  T iny_div = (T)1.0;
  T out_div;
  if (is2sg) {
    inx_div = ONE_HERE;
    iny_div = div2;
  }
  if (is3sg) {
    inx_div = x_sqr;
    iny_div = div3;
  }
  out_div = inx_div / iny_div;
  // log
  T in_log, in_log1p, out_log;
  if (is1sg)
    in_log = x_abs;
  else // if(is2sg)
    in_log = x2 + out_div;
  in_log1p = (T)1.0 + x_abs + out_div;
  if (is3sg)
    out_log = hls::log(in_log1p);
  else
    out_log = hls::log(in_log);

  if (is1sg)
    out_log = out_log + ln2_t;

  fp_struct<T> fz(out_log);
  fz.sign = fx.sign;
  return fz.to_ieee();
}

static double asinh(double x) { return generic_asinh<double>(x); }

static float asinhf(float x) { return generic_asinh<float>(x); }

static half half_asinh(half x) { return generic_asinh<half>(x); }

static float asinh(float x) { return generic_asinh<float>(x); }
static half asinh(half x) { return generic_asinh<half>(x); }

} // namespace explog_based

#endif // entire file
