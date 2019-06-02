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

#ifndef X_HLS_ATANH_H
#define X_HLS_ATANH_H

namespace explog_based {

/*
  Method:
  atanh(x), odd function
  input: nan           outout: nan
  input: inf           output: nan
  input: -inf          output: nan
  input: (1,inf)       output: nan
  input: 1.0           output: inf
  input: [0.0,0.5)     output: 0.5*log1p(2x+2x*x/(1-x))
  input: [0.5,1.0)     output: 0.5*log1p(2x/(1-x))
*/
template <class T> T generic_atanh(T t_in) {

  static const int exp_bias = fp_struct<T>::EXP_BIAS;

  fp_struct<T> din(t_in);
  T abst_in = hls::fabs(t_in);
  T resultf;

  if (din.exp == fp_struct<T>::EXP_INFNAN) {

    resultf = ::hls::nan("");
  } else if ((din.exp > exp_bias) or (din.exp == exp_bias and din.sig > 0)) {

    resultf = ::hls::nan("");
  } else if (din.exp == exp_bias and din.sig == 0) {

    fp_struct<T> out;
    out.sign = 0;
    out.exp = fp_struct<T>::EXP_INFNAN;
    out.sig = 0;
    resultf = out.to_ieee(); // +inf
  } else {

    const T cst1 = 1.0;
    const T cst05 = 0.5;
    T t = abst_in + abst_in;
    T x;
    if (din.exp < exp_bias - 1) {
      x = t + t * abst_in / (cst1 - abst_in);
    } else {
      x = t / (cst1 - abst_in);
    };

    resultf = cst05 * hls::log1p(x);
  }

  // return
  if (din.sign == 0) { // positive
    return resultf;
  } else {
    return -resultf;
  };
};

static double atanh(double t_in) { return generic_atanh(t_in); };

static float atanh(float t_in) { return generic_atanh(t_in); };

static half atanh(half t_in) { return generic_atanh(t_in); };

static float atanhf(float t_in) { return generic_atanh(t_in); };

static half half_atanh(half t_in) { return generic_atanh(t_in); };

} // namespace explog_based

#endif
