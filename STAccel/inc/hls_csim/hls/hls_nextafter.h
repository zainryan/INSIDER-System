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
 * @file hls_nextafter.h
 */
#ifndef __HLS_NEXTAFTER_HH
#define __HLS_NEXTAFTER_HH

template <class T> T generic_nextafter(T x, T y) {
  fp_struct<T> fx(x);
  fp_struct<T> fy(y);
  const int E = fp_struct<T>::EXP_BITS;
  const int W = fp_struct<T>::BITS;

  ap_uint<W - 1> INF = ((1LL << E) - 1) << (W - 1 - E);
  ap_uint<1> ap_sx(fx.sign);
  ap_uint<W - 1> ap_hx(fx.data()(W - 2, 0));
  ap_uint<1> ap_sy(fy.sign);
  ap_uint<W - 1> ap_hy(fy.data()(W - 2, 0));
  ap_uint<1> ap_sz(0);
  ap_uint<W - 1> ap_hz(0);
  ap_uint<1> sxEsy(ap_sx == ap_sy);
  ap_uint<1> hxEhy(ap_hx == ap_hy);
  ap_uint<1> hxLhy(ap_hx < ap_hy);
  ;
  ap_uint<1> hxBinf(ap_hx > INF);
  ap_uint<1> hyBinf(ap_hy > INF);
  ap_uint<1> hxE0(ap_hx == 0);
  ap_uint<W - 1> ap_hy_p1(ap_hx + 1);
  ap_uint<W - 1> ap_hy_m1(ap_hx - 1);

  /* MUX for sx */
  ap_sz =
      hyBinf ? ap_sy
             : // 1) y is NAN (not compliant with linux which return x+y)
          hxBinf
              ? ap_sx
              : // 2) only x is NAN (not compliant with linux which return x+y)
              hxEhy & sxEsy ? ap_sy : // 3) x==y
                  hxE0 ? ap_sy : // 4) x is +0 or -0 and is not as same as y
                      ap_sx;     // 5) default

  ap_hz = hyBinf ? ap_hy :                 // 1) y is NAN
              hxBinf ? ap_hx :             // 2) only x is NAN
                  hxEhy && sxEsy ? ap_hy : // 3) x==y
                      hxEhy && (!sxEsy) && hxE0
                          ? ap_hy
                          : // 4) x,y are zero of diff sign
                          (!sxEsy) && (!hxE0)
                              ? ap_hy_m1
                              : // 5) x,y are diff number with diff sign
                              (!hxEhy) && hxE0
                                  ? ap_hy_p1
                                  : // 6) hx is 0 and hy is not,so away from 0
                                  (!hxEhy) && sxEsy && hxLhy
                                      ? ap_hy_p1
                                      : // 7) same sign, normal number compare
                                      ap_hy_m1; // 8) same sign, normal number
                                                // compare
  /* assign output */
  fp_struct<T> fz;
  ap_int<W> out;
  fz.sign = ap_sz;
  fz.exp = ap_hz(W - 2, W - E - 1);
  fz.sig = ap_hz(W - E - 2, 0);
  return fz.to_ieee();
};

static double nextafter(double x, double y) {
  return generic_nextafter<double>(x, y);
}

static float nextafterf(float x, float y) {
  return generic_nextafter<float>(x, y);
}

static half half_nextafter(half x, half y) {
  return generic_nextafter<half>(x, y);
}

#ifdef __cplusplus
static float nextafter(float x, float y) {
  return generic_nextafter<float>(x, y);
}
static half nextafter(half x, half y) {
  return generic_nextafter<half>(x, y);
}
#endif
;
#endif // entire file
