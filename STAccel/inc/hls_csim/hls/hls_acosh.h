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
 *     (c) Copyright 2017 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

#ifndef X_HLS_ACOSH_H
#define X_HLS_ACOSH_H

namespace explog_based {

/*
  Method:
  acosh(x)
  input: nan           outout: nan
  input: inf           output: inf
  input: -inf          output: nan
  input: (-inf,1.0)    output: nan
  input: 1.0           output: 0.0
  input: (1.0,2.0]     output: log1p(x-1+sqrt(2*(x-1)+(x-1)**2))
  input: (2.0,2**28]   output: log(2*x-1/(x+sqrt(x**2-1)))
  input: (2**28, inf)  output: log(2x) = log(x) + log(2)
*/
template <class T> T generic_acosh(T t_in) {

  static const int exp_bias = fp_struct<T>::EXP_BIAS;

  fp_struct<T> din(t_in);
  T resultf;

  if (din.exp == fp_struct<T>::EXP_INFNAN) {

    if (din.sig != 0) {
      resultf = ::hls::nan("");
    } else {
      if (din.sign == 0) {

        fp_struct<T> out;
        out.sign = 0;
        out.exp = fp_struct<T>::EXP_INFNAN;
        out.sig = 0;
        resultf = out.to_ieee(); // out=+inf
      } else {
        resultf = ::hls::nan("");
      }
    }
  } else if ((din.sign == 1) or (din.exp < exp_bias)) {

    resultf = ::hls::nan("");
  } else if ((din.exp == exp_bias) and (din.sig == 0)) {

    resultf = (T)0.0;
  } else if ((din.exp == exp_bias and din.sig > 0) or
             (din.exp == exp_bias + 1 and din.sig == 0)) {

    const T cst1 = 1.0;
    T t = t_in - cst1;
    T x1 = t + (T)hls::sqrt(t + t + t * t);
    resultf = hls::log1p(x1);
  } else {

    T x;
    if (din.exp < exp_bias + 28) {
      const T cst1 = 1.0;
      x = t_in + t_in - cst1 / (t_in + (T)hls::sqrt(t_in * t_in - cst1));
    } else {
      x = t_in;
    }

    T logx = hls::log(x);

    if (din.exp < exp_bias + 28) {
      resultf = logx;
    } else {
      const T ln2 = 6.93147180559945286227e-01; // 0x3FE62E42, 0xFEFA39EF
      resultf = logx + ln2;
    };
  };

  return resultf;
};

static float acosh(float t_in) { return generic_acosh(t_in); };

static double acosh(double t_in) { return generic_acosh(t_in); };

static half acosh(half t_in) { return generic_acosh(t_in); };

static float acoshf(float t_in) { return generic_acosh(t_in); };

static half half_acosh(half t_in) { return generic_acosh(t_in); };

} // namespace explog_based

#endif
