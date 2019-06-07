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

#ifndef X_HLS_TANH_H
#define X_HLS_TANH_H

namespace explog_based {

/*
  Method:
  tanh(x), odd function
  input: nan           outout: nan
  input: inf           output: 1.0
  input: -inf          output: -1.0
  input: (0,2**-55]    output: x*(x+1)
  input: (2**-55,1.0)  output: -expm1(-2x) / (expm1(-2x)+2)
  input: [1.0,22)      output: 1.0 - 2.0/(expm1(2x)+2)
  input: [22.0,inf)    output: 1.0
*/
template <class T> T generic_tanh(T t_in) {

  static const int exp_bias = fp_struct<T>::EXP_BIAS;

  fp_struct<T> din(t_in);
  T abst_in = hls::fabs(t_in);
  T resultf;

  if (din.exp == fp_struct<T>::EXP_INFNAN) {

    if (din.sig != 0) {
      resultf = ::hls::nan("");
    } else {
      resultf = (T)1.0;
    }
  } else if ((din.exp < exp_bias - 55) or
             (din.exp == exp_bias - 55 and din.sig == 0)) {

    const T cst1 = 1.0;
    resultf = abst_in * (cst1 + abst_in);
  } else if (abst_in < (T)22.0) {

    T x;
    if (din.exp < exp_bias) {
      x = -abst_in - abst_in;
    } else {
      x = abst_in + abst_in;
    }

    T expx = hls::expm1(x);

    if (din.exp < exp_bias) {

      const T cst2 = 2.0;
      resultf = -expx / (expx + cst2);
    } else {
      const T cst1 = 1.0;
      const T cst2 = 2.0;
      resultf = cst1 - cst2 / (expx + cst2);
    }
  } else {
    resultf = (T)1.0;
  }

  // return
  if (din.sign == 0) { // positive
    return resultf;
  } else {
    return -resultf;
  };
}

static double tanh(double t_in) { return generic_tanh(t_in); };

static float tanh(float t_in) { return generic_tanh(t_in); };

static half tanh(half t_in) { return generic_tanh(t_in); };

static float tanhf(float t_in) { return generic_tanh(t_in); };

static half half_tanh(half t_in) { return generic_tanh(t_in); };

} // namespace explog_based

#endif
