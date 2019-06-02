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
 * @file hls_fmod_rem_quo.h
 *
 */
#ifndef __HLS_FMOD_REM_QUO_HH
#define __HLS_FMOD_REM_QUO_HH
namespace fmdrmd {

template <int N, int S> ap_uint<S + 1> rcp_sub(ap_uint<N> d) {
  ap_uint<N + S + 1> b2 = 1;
  b2 <<= (N + S);
  ap_uint<S + 1> rst = b2 / d;
  return b2 / d;
}
template <int N, int S, int E>
ap_uint<N> mod_sub(ap_uint<N> x, ap_uint<N> y, ap_int<E + 2> n) {
  ap_uint<N + 1> rem(0);
  ap_uint<N> ret(0);
  ap_int<E + 2> i(0);
  ap_uint<E + 1> loop(0);
  const int LOOPEND = ((1 << E) - 2 + N - 2);
  rem = x;
  i = n;
  for (loop = 0; loop < LOOPEND; loop++) {
    if (i == 0)
      continue;
    i--;
    if (rem >= y)
      rem -= y;
    rem <<= 1;
  }
  if (rem >= y)
    rem -= y;
  ret = rem;
  return ret;
}
template <int N, int S, int E>
ap_uint<N> mod_sub_nr2(ap_uint<N> x, ap_uint<N> y, ap_int<E + 2> n) {
  ap_uint<N> ret(0);
  ap_int<E + 2> i(0);
  ap_uint<E + 1> loop(0);
  ap_int<N + 1> rem(0);
  ap_uint<N + 1> rem2(0);
  ap_uint<1> s;
  i = n;
  rem(N - 1, 0) = x;
  rem2(N - 1, 0) = x;
  const int LOOPEND = ((1 << E) - 2 + N - 2);
  for (loop = 0; loop < LOOPEND; loop++) {
    if (i == 0)
      continue;
    i--;
    rem = rem2 - y;
    s = rem(N, N);
    if (s == 0)
      rem2 = rem;
    rem2 <<= 1;
  }
  if (rem2 >= y)
    rem2 -= y;
  ret = rem2;
  return ret;
}
template <int N, int S, int E>
ap_uint<N> mod_rcp_mult(ap_uint<N> r_sh, ap_uint<N> d, ap_int<E + 2> n) {
  const int LOOPEND = ((1 << E) - 2 + N - 2 + S - 1) / S;
  ap_uint<S + 1> rcp; //
  ap_uint<6> shift(0);
  ap_uint<N + 1> d2d(0);
  ap_int<E + 2> i(0);
  ap_uint<E + 1> loop(0);
  ap_uint<N + S + 1> quo(0);
  ap_uint<S + 1> quoN(0);
  ap_uint<N + S> prdct(0);
  ap_uint<N + S> r_shN(0);
  ap_uint<N + 1> rem(0);

  i = n;
  d2d(N, 1) = d;
  rcp = rcp_sub<N, S>(d);
  // TODO
  //#pragma HLS unroll
  for (loop = 0; loop < LOOPEND; loop++) {
    if (i > S)
      shift = S;
    else
      shift = i;
    quo = rcp * r_sh;
    quoN = quo >> (N + S - shift);
    prdct = quoN * d;
    r_shN = r_sh;
    r_shN = r_shN << shift;
    rem = r_shN - prdct;
    if (rem >= d2d)
      rem -= d2d;
    if (rem >= d)
      rem -= d;
    r_sh(N - 1, 0) = rem(N - 1, 0);
    i -= shift;
  } // end of for
  return r_sh;
}

template <class T> T generic_fmod_mult(T x, T y) {
  const int W = fp_struct<T>::EXP_BITS + fp_struct<T>::SIG_BITS + 1;
  const int E = fp_struct<T>::EXP_BITS;
  const int M = W - E - 1;
  const int N = M + 1;
  const int S = N + 12;
  fp_struct<T> fx(x);
  fp_struct<T> fy(y);
  fp_struct<T> fz;
  ap_uint<E> FF_E = ((1 << E) - 1);
  ap_uint<M> FF_M = ((1LL << (W - E - 1)) - 1);
  ap_uint<M + 2> ap_mx(0LL);
  ap_uint<M + 2> ap_my(0LL);
  ap_uint<1> isZ_e_x = fx.exp == 0 ? 1 : 0;
  ap_uint<1> isF_e_x = fx.exp == FF_E ? 1 : 0;
  ap_uint<1> isN_e_x = (isZ_e_x == 0 && isF_e_x == 0);
  ap_uint<1> isZ_m_x = fx.sig == 0 ? 1 : 0;
  ap_uint<1> isF_m_x = fx.sig == FF_M ? 1 : 0;
  ap_uint<1> isZ_e_y = fy.exp == 0 ? 1 : 0;
  ap_uint<1> isF_e_y = fy.exp == FF_E ? 1 : 0;
  ap_uint<1> isN_e_y = (isZ_e_y == 0 && isF_e_y == 0);
  ap_uint<1> isZ_m_y = fy.sig == 0 ? 1 : 0;
  ap_uint<1> isF_m_y = fy.sig == FF_M ? 1 : 0;
  ap_uint<1> isxEy_e = fx.exp == fy.exp ? 1 : 0;
  ap_uint<1> isyBx_e = fy.exp > fx.exp ? 1 : 0;
  ap_uint<1> isxEy_m = fx.sig == fy.sig ? 1 : 0;
  ap_uint<1> isyBx_m = fy.sig > fx.sig ? 1 : 0;
  fz.sign = fx.sign;
  ap_int<E + 1> ix, iy;
  ap_int<E + 2> n;
  ap_uint<6> LZ;

  if ((isZ_m_y && isZ_e_y) || ((isF_e_x)) || ((isF_e_y) && (!isZ_m_y))) {
    fz.exp = FF_E;
    fz.sig = FF_M;
    return fz.to_ieee();
  }
  if (isyBx_e) {
    fz.exp = fx.exp;
    fz.sig = fx.sig;
    return fz.to_ieee();
  } else if ((isxEy_e) && (isyBx_m)) {
    fz.exp = fx.exp;
    fz.sig = fx.sig;
    return fz.to_ieee();
  } else if ((isxEy_e) && (isxEy_m)) {
    fz.exp = 0;
    fz.sig = 0;
    return fz.to_ieee();
  }
  if (isZ_e_x) {
    LZ = fx.sig.countLeadingZeros();
    ix = -fx.EXP_BIAS + 1 - LZ - 1;
    ap_mx(M, LZ + 1) = fx.sig(M - 1 - LZ, 0);
    ap_mx(LZ, 0) = 0;
  } else {
    ix = fx.exp - fx.EXP_BIAS;
    ap_mx(M - 1, 0) = fx.sig;
    ap_mx[M] = 1;
  }
  if (isZ_e_y) {
    LZ = fy.sig.countLeadingZeros();
    iy = -fy.EXP_BIAS + 1 - fy.sig.countLeadingZeros() - 1;
    ap_my(M, LZ + 1) = fy.sig(M - 1 - LZ, 0);
    ap_my(LZ, 0) = 0;
  } else {
    iy = fy.exp - fy.EXP_BIAS;
    ap_my(M - 1, 0) = fy.sig;
    ap_my[M] = 1;
  }

  n = ix - iy;
  ap_uint<N> d = ap_my(N - 1, 0);
  ap_uint<N + 0> r_sh(0);
  r_sh(N - 1, 0) = ap_mx(N - 1, 0);
  ap_uint<N> res(0);

  res = mod_rcp_mult<N, S, E>(ap_mx, d, n);

  r_sh = res;
  ap_uint<N + 1> ap_mx_2(0);
  ap_mx_2(N - 1, 0) = (r_sh(N - 1, 0));

  if (ap_mx_2 == 0) {
    fz.exp = 0;
    fz.sig = 0;
    return fz.to_ieee();
  }
  iy -= (ap_mx_2.countLeadingZeros() - 1);
  ap_mx_2 = ap_mx_2 << (ap_mx_2.countLeadingZeros() - 1);
  if (iy >= 1 - fy.EXP_BIAS) {
    fz.exp = iy + fy.EXP_BIAS;
    fz.sig(M - 1, 0) = ap_mx_2(M - 1, 0);
  } else {
    n = 1 - fy.EXP_BIAS - iy;
    ap_mx_2 >>= n;
    fz.exp = 0;
    fz.sig(M - 1, 0) = ap_mx_2(M - 1, 0);
  }
  return fz.to_ieee();
}
template <class T> T generic_fmod(T x, T y) {
  const int W = fp_struct<T>::EXP_BITS + fp_struct<T>::SIG_BITS + 1;
  const int E = fp_struct<T>::EXP_BITS;
  const int M = W - E - 1;
  const int N = M + 1;
  fp_struct<T> fx(x);
  fp_struct<T> fy(y);
  fp_struct<T> fz;
  ap_uint<E> FF_E = ((1 << E) - 1);
  ap_uint<M> FF_M = ((1LL << (W - E - 1)) - 1);
  ap_uint<M + 2> ap_mx(0LL);
  ap_uint<M + 2> ap_my(0LL);
  ap_uint<1> isZ_e_x = fx.exp == 0 ? 1 : 0;
  ap_uint<1> isF_e_x = fx.exp == FF_E ? 1 : 0;
  ap_uint<1> isN_e_x = (isZ_e_x == 0 && isF_e_x == 0);
  ap_uint<1> isZ_m_x = fx.sig == 0 ? 1 : 0;
  ap_uint<1> isF_m_x = fx.sig == FF_M ? 1 : 0;
  ap_uint<1> isZ_e_y = fy.exp == 0 ? 1 : 0;
  ap_uint<1> isF_e_y = fy.exp == FF_E ? 1 : 0;
  ap_uint<1> isN_e_y = (isZ_e_y == 0 && isF_e_y == 0);
  ap_uint<1> isZ_m_y = fy.sig == 0 ? 1 : 0;
  ap_uint<1> isF_m_y = fy.sig == FF_M ? 1 : 0;
  ap_uint<1> isxEy_e = fx.exp == fy.exp ? 1 : 0;
  ap_uint<1> isyBx_e = fy.exp > fx.exp ? 1 : 0;
  ap_uint<1> isxEy_m = fx.sig == fy.sig ? 1 : 0;
  ap_uint<1> isyBx_m = fy.sig > fx.sig ? 1 : 0;
  fz.sign = fx.sign;
  ap_int<E + 1> ix, iy;
  ap_int<E + 2> n;
  ap_uint<6> LZ;
  if ((isZ_m_y && isZ_e_y) || ((isF_e_x)) || ((isF_e_y) && (!isZ_m_y))) {
    fz.exp = FF_E;
    fz.sig = FF_M;
    return fz.to_ieee();
  }
  if (isyBx_e) {
    fz.exp = fx.exp;
    fz.sig = fx.sig;
    return fz.to_ieee();
  } else if ((isxEy_e) && (isyBx_m)) {
    fz.exp = fx.exp;
    fz.sig = fx.sig;
    return fz.to_ieee();
  } else if ((isxEy_e) && (isxEy_m)) {
    fz.exp = 0;
    fz.sig = 0;
    return fz.to_ieee();
  }
  if (isZ_e_x) {
    LZ = fx.sig.countLeadingZeros();
    ix = -fx.EXP_BIAS + 1 - LZ - 1;
    ap_mx(M, LZ + 1) = fx.sig(M - 1 - LZ, 0);
    ap_mx(LZ, 0) = 0;
  } else {
    ix = fx.exp - fx.EXP_BIAS;
    ap_mx(M - 1, 0) = fx.sig;
    ap_mx[M] = 1;
  }
  if (isZ_e_y) {
    LZ = fy.sig.countLeadingZeros();
    iy = -fy.EXP_BIAS + 1 - fy.sig.countLeadingZeros() - 1;
    ap_my(M, LZ + 1) = fy.sig(M - 1 - LZ, 0);
    ap_my(LZ, 0) = 0;
  } else {
    iy = fy.exp - fy.EXP_BIAS;
    ap_my(M - 1, 0) = fy.sig;
    ap_my[M] = 1;
  }
  n = ix - iy;
  ap_uint<N> d = ap_my(N - 1, 0);
  ap_uint<N + 1> r_sh(0);
  ap_int<E + 2> i;
  r_sh(N - 1, 0) = ap_mx(N - 1, 0);
  i = n;

  const int LOOPEND = ((1 << E) - 2 + M);
  ap_uint<E + 1> loop(0);
  // TODO
  // to be fixed for half
  //#pragma HLS unroll
  for (loop = 0; loop < LOOPEND; loop++) {
    if (r_sh >= d)
      r_sh = r_sh - d;
    if (i > 0) {
      r_sh <<= 1;
      i--;
    };
  } // end of for

  ap_uint<N + 1> ap_mx_2(r_sh(N, 0));
  if (ap_mx_2 == 0) {
    fz.exp = 0;
    fz.sig = 0;
    return fz.to_ieee();
  }
  iy -= (ap_mx_2.countLeadingZeros() - 1);
  ap_mx_2 = ap_mx_2 << (ap_mx_2.countLeadingZeros() - 1);
  if (iy >= 1 - fy.EXP_BIAS) {
    fz.exp = iy + fy.EXP_BIAS;
    fz.sig(M - 1, 0) = ap_mx_2(M - 1, 0);
  } else {
    n = 1 - fy.EXP_BIAS - iy;
    ap_mx_2 >>= n;
    fz.exp = 0;
    fz.sig(M - 1, 0) = ap_mx_2(M - 1, 0);
  }
  return fz.to_ieee();
}
// BEGIN "remainder"
// --------------------------------------------------------------------------------------------
template <class T>
T generic_remainder(T x, T y) // T ggeneric_remainder_fast_2S(T x, T y)
{
  const int W = fp_struct<T>::EXP_BITS + fp_struct<T>::SIG_BITS + 1;
  const int E = fp_struct<T>::EXP_BITS;
  const int M = W - E - 1;
  const int N = M + 1;
  fp_struct<T> fx(x);
  fp_struct<T> fy(y);
  fp_struct<T> fz;
  ap_uint<E> FF_E = ((1 << E) - 1);
  ap_uint<M> FF_M = ((1LL << (W - E - 1)) - 1);
  ap_uint<M + 2> ap_mx(0LL);
  ap_uint<M + 2> ap_my(0LL);
  ap_uint<1> isZ_e_x = fx.exp == 0 ? 1 : 0;
  ap_uint<1> isF_e_x = fx.exp == FF_E ? 1 : 0;
  ap_uint<1> isN_e_x = (isZ_e_x == 0 && isF_e_x == 0);
  ap_uint<1> isZ_m_x = fx.sig == 0 ? 1 : 0;
  ap_uint<1> isF_m_x = fx.sig == FF_M ? 1 : 0;
  ap_uint<1> isZ_e_y = fy.exp == 0 ? 1 : 0;
  ap_uint<1> isF_e_y = fy.exp == FF_E ? 1 : 0;
  ap_uint<1> isN_e_y = (isZ_e_y == 0 && isF_e_y == 0);
  ap_uint<1> isZ_m_y = fy.sig == 0 ? 1 : 0;
  ap_uint<1> isF_m_y = fy.sig == FF_M ? 1 : 0;
  ap_uint<1> isxEy_e = fx.exp == fy.exp ? 1 : 0;
  ap_uint<1> isyBx_1e = (fy.exp - fx.exp) == 1 ? 1 : 0;
  ap_uint<1> isyBx_2e = (fy.exp - fx.exp) > 1 ? 1 : 0;
  ap_uint<1> isxEy_m = fx.sig == fy.sig ? 1 : 0;
  ap_uint<1> isyBx_m = fy.sig > fx.sig ? 1 : 0;
  ap_uint<M + 1> sigx_2;
  sigx_2(M, 1) = fx.sig(M - 1, 0);
  ap_uint<1> isyBE2x_m = fy.sig >= (sigx_2) ? 1 : 0;
  fz.sign = fx.sign;
  ap_int<E + 1> ix, iy;
  ap_int<E + 2> n;
  ap_uint<6> LZ;
  if ((isZ_m_y && isZ_e_y) || ((isF_e_x)) || ((isF_e_y) && (!isZ_m_y))) {
    fz.exp = FF_E;
    fz.sig = FF_M;
    return fz.to_ieee();
  }
  if ((isF_e_y) && (isZ_m_y)) {
    fz = fx;
    return fz.to_ieee();
  }
  if ((isxEy_e) && (isxEy_m)) {
    fz.exp = 0;
    fz.sig = 0;
    fz.sign =
        fx.sign; // If the returned value is 0, it will have the same sign as x
    return fz.to_ieee();

  } else if (isyBx_2e) { //(fy.exp-fx.exp)>1?1:0;

    fz.exp = fx.exp;
    fz.sig = fx.sig;
    fz.sign = fx.sign;
    return fz.to_ieee();

  } else if (isyBx_1e) {
    if (isZ_e_x) {
      if (sigx_2(M - 1, 0) <= fy.sig) {
        fz.exp = fx.exp;
        fz.sig = fx.sig;
        fz.sign = fx.sign;
        return fz.to_ieee();
      }
    } else if (isyBx_m) { // fy.sig > fx.sig?1:0;
      fz.exp = fx.exp;
      fz.sig = fx.sig;
      fz.sign = fx.sign;
      return fz.to_ieee();
    } else if (isxEy_m) { // fx.sig == fy.sig?1:0;
      fz.exp = fx.exp;
      fz.sig = fx.sig;
      fz.sign = fx.sign;
      return fz.to_ieee();
    }
  } else if (isxEy_e && isZ_e_y) {
    if (isyBE2x_m) {
      fz.exp = fx.exp;
      fz.sig = fx.sig;
      fz.sign = fx.sign;
      return fz.to_ieee();
    }
  }

  if (isZ_e_x) {
    LZ = fx.sig.countLeadingZeros();
    ix = -fx.EXP_BIAS + 1 - LZ - 1;
    ap_mx(M, LZ + 1) = fx.sig(M - 1 - LZ, 0);
    ap_mx(LZ, 0) = 0;
  } else {
    ix = fx.exp - fx.EXP_BIAS;
    ap_mx(M - 1, 0) = fx.sig;
    ap_mx[M] = 1;
  }
  if (isZ_e_y) {
    LZ = fy.sig.countLeadingZeros();
    iy = -fy.EXP_BIAS + 1 - fy.sig.countLeadingZeros() - 1;
    ap_my(M, LZ + 1) = fy.sig(M - 1 - LZ, 0);
    ap_my(LZ, 0) = 0;
  } else {
    iy = fy.exp - fy.EXP_BIAS;
    ap_my(M - 1, 0) = fy.sig;
    ap_my[M] = 1;
  }

  n = ix - iy;
  ap_uint<N> d = ap_my(N - 1, 0);
  ap_uint<N + 1> d2d(0);
  ap_uint<N + 1> r_sh(0);
  ap_uint<1> s_r(fx.sign);
  ap_int<E + 2> i;
  ap_uint<1> even(1);
  d2d(N, 1) = d(N - 1, 0);
  const int LOOPEND = ((1 << E) - 2 + M);
  ap_uint<E + 1> loop(0);
  r_sh(N - 1, 0) = ap_mx(N - 1, 0);
  i = n;
  // TODO
  // To be fixed for float and half
  //#pragma HLS unroll
  for (loop = 0; loop < LOOPEND; loop++) {
    if (i > 0) {
      i--;
      if (r_sh >= d)
        r_sh = (r_sh - d) << 1;
      else
        r_sh = r_sh << 1;
    }
  }
  if (i == 0 && r_sh >= d) {
    r_sh -= d;
    even = 0;
  }

  ap_uint<N + 2> r_sh2 = (r_sh << 1);
  if (n < -1) {
    fz = fx;
    return fz.to_ieee();
  }
  if (r_sh2 < d) {
    ;
  } else if (r_sh2 == d) {
    if (even == 0)
      s_r = 1 - s_r;
  } else if (r_sh < d) { // but > 1/2 d
    r_sh = d - r_sh;
    s_r = 1 - s_r;
  } else if (r_sh > d) { //&&n==-1 in fact
    r_sh = d2d - r_sh;
    s_r = 1 - s_r;
  }

  fz.sign = s_r;
  ap_uint<N + 1> ap_mx_2(r_sh(N, 0));
  if (ap_mx_2 == 0) {
    fz.exp = 0;
    fz.sig = 0;
    return fz.to_ieee();
  }
  iy -= (ap_mx_2.countLeadingZeros() - 1);
  iy += i;
  ap_mx_2 = ap_mx_2 << (ap_mx_2.countLeadingZeros() - 1);

  if (iy >= 1 - fy.EXP_BIAS) {
    fz.exp = iy + fy.EXP_BIAS;
    fz.sig(M - 1, 0) = ap_mx_2(M - 1, 0);
  } else {
    n = 1 - fy.EXP_BIAS - iy;
    ap_mx_2 >>= n;
    fz.exp = 0;
    fz.sig(M - 1, 0) = ap_mx_2(M - 1, 0);
  }
  return fz.to_ieee();
}
// remquo-------------------------------------------------------------------------------
template <class T> T generic_remquo(T x, T y, int *quo) {
  const int W = fp_struct<T>::EXP_BITS + fp_struct<T>::SIG_BITS + 1;
  const int E = fp_struct<T>::EXP_BITS;
  const int M = W - E - 1;
  const int N = M + 1;
  fp_struct<T> fx(x);
  fp_struct<T> fy(y);
  fp_struct<T> fz;
  ap_uint<E> FF_E = ((1 << E) - 1);
  ap_uint<M> FF_M = ((1LL << (W - E - 1)) - 1);
  ap_uint<M + 2> ap_mx(0LL);
  ap_uint<M + 2> ap_my(0LL);
  ap_uint<1> isZ_e_x = fx.exp == 0 ? 1 : 0;
  ap_uint<1> isF_e_x = fx.exp == FF_E ? 1 : 0;
  ap_uint<1> isN_e_x = (isZ_e_x == 0 && isF_e_x == 0);
  ap_uint<1> isZ_m_x = fx.sig == 0 ? 1 : 0;
  ap_uint<1> isF_m_x = fx.sig == FF_M ? 1 : 0;
  ap_uint<1> isZ_e_y = fy.exp == 0 ? 1 : 0;
  ap_uint<1> isF_e_y = fy.exp == FF_E ? 1 : 0;
  ap_uint<1> isN_e_y = (isZ_e_y == 0 && isF_e_y == 0);
  ap_uint<1> isZ_m_y = fy.sig == 0 ? 1 : 0;
  ap_uint<1> isF_m_y = fy.sig == FF_M ? 1 : 0;
  ap_uint<1> isxEy_e = fx.exp == fy.exp ? 1 : 0;
  ap_uint<1> isyBx_1e = (fy.exp - fx.exp) == 1 ? 1 : 0;
  ap_uint<1> isyBx_2e = (fy.exp - fx.exp) > 1 ? 1 : 0;
  ap_uint<1> isxEy_m = fx.sig == fy.sig ? 1 : 0;
  ap_uint<1> isyBx_m = fy.sig > fx.sig ? 1 : 0;
  ap_uint<1> isE_s = fy.sign == fx.sign ? 1 : 0;
  ap_uint<M + 1> sigx_2;
  sigx_2(M, 1) = fx.sig(M - 1, 0);
  ap_uint<1> isyBE2x_m = fy.sig >= (sigx_2) ? 1 : 0;
  fz.sign = fx.sign;
  ap_int<E + 1> ix, iy;
  ap_int<E + 2> n;
  ap_uint<6> LZ;
  if ((isZ_m_y && isZ_e_y) || ((isF_e_x)) || ((isF_e_y) && (!isZ_m_y))) {
    fz.exp = FF_E;
    fz.sig = FF_M;
    *quo = isE_s ? 0 : -0;
    return fz.to_ieee();
  }
  if ((isF_e_y) && (isZ_m_y)) {
    fz = fx;
    *quo = isE_s ? 0 : -0;
    return fz.to_ieee();
  }
  if ((isxEy_e) && (isxEy_m)) {
    fz.exp = 0;
    fz.sig = 0;
    fz.sign =
        fx.sign; // If the returned value is 0, it will have the same sign as x
    *quo = isE_s ? 1 : -1;
    return fz.to_ieee();

  } else if (isyBx_2e) { //(fy.exp-fx.exp)>1?1:0;

    fz.exp = fx.exp;
    fz.sig = fx.sig;
    fz.sign = fx.sign;
    *quo = isE_s ? 0 : -0;
    return fz.to_ieee();

  } else if (isyBx_1e) {
    if (isZ_e_x) {
      if (sigx_2(M - 1, 0) <= fy.sig) {
        fz.exp = fx.exp;
        fz.sig = fx.sig;
        fz.sign = fx.sign;
        *quo = isE_s ? 0 : -0;
        return fz.to_ieee();
      }
    } else if (isyBx_m) { // fy.sig > fx.sig?1:0;
      fz.exp = fx.exp;
      fz.sig = fx.sig;
      fz.sign = fx.sign;
      *quo = isE_s ? 0 : -0;
      return fz.to_ieee();
    } else if (isxEy_m) { // fx.sig == fy.sig?1:0;
      fz.exp = fx.exp;
      fz.sig = fx.sig;
      fz.sign = fx.sign;
      *quo = isE_s ? 0 : -0; // to be even
      return fz.to_ieee();
    }
  } else if (isxEy_e && isZ_e_y) {
    if (isyBE2x_m) {
      fz.exp = fx.exp;
      fz.sig = fx.sig;
      fz.sign = fx.sign;
      *quo = isE_s ? 0 : -0;
      return fz.to_ieee();
    }
  }

  if (isZ_e_x) {
    LZ = fx.sig.countLeadingZeros();
    ix = -fx.EXP_BIAS + 1 - LZ - 1;
    ap_mx(M, LZ + 1) = fx.sig(M - 1 - LZ, 0);
    ap_mx(LZ, 0) = 0;
  } else {
    ix = fx.exp - fx.EXP_BIAS;
    ap_mx(M - 1, 0) = fx.sig;
    ap_mx[M] = 1;
  }
  if (isZ_e_y) {
    LZ = fy.sig.countLeadingZeros();
    iy = -fy.EXP_BIAS + 1 - fy.sig.countLeadingZeros() - 1;
    ap_my(M, LZ + 1) = fy.sig(M - 1 - LZ, 0);
    ap_my(LZ, 0) = 0;
  } else {
    iy = fy.exp - fy.EXP_BIAS;
    ap_my(M - 1, 0) = fy.sig;
    ap_my[M] = 1;
  }
  n = ix - iy;
  ap_uint<N> d = ap_my(N - 1, 0);
  ap_uint<N + 2> d2d(0);
  ap_uint<N + 2> r_sh(0);
  ap_uint<1> s_r(fx.sign);
  ap_int<E + 2> i;
  ap_uint<1> even(1);
  ap_uint<3> apq(0);
  ap_uint<E + 1> loop(0);
  d2d(N, 1) = d(N - 1, 0);
  r_sh(N - 1, 0) = ap_mx(N - 1, 0);
  i = n; // i can be negtive
  const int LOOPEND = ((1 << E) - 2 + M);
  // TODO
  // To be fixed for float and half
  //#pragma HLS unroll
  for (loop = 0; loop < LOOPEND; loop++) {
    if (i > 0) {
      i--;
      if (r_sh >= d) {
        r_sh = (r_sh - d) << 1;
        apq[0] = 1;
        apq <<= 1;
      } else {
        r_sh = r_sh << 1;
        apq <<= 1;
      }
    }
  }
  if (i == 0 && r_sh >= d) {
    r_sh -= d;
    even = 0;
    apq[0] = 1;
  }

  ap_uint<N + 2> r_sh2 = (r_sh << 1);
  if (n < -1) {
    fz = fx;
    return fz.to_ieee();
  }
  if (r_sh2 < d) {
    ;
  } else if (r_sh2 == d) {
    if (even == 0) {
      s_r = 1 - s_r;
      apq += 1;
    }
  } else if (r_sh < d) { // but > 1/2 d
    r_sh = d - r_sh;
    s_r = 1 - s_r;
    apq += 1;
  } else if (r_sh > d) { //&&n==-1 in fact
    r_sh = d2d - r_sh;
    s_r = 1 - s_r;
    apq += 1;
  }
  if (isE_s)
    *quo = apq;
  else
    *quo = 0 - apq;

  fz.sign = s_r;
  ap_uint<N + 1> ap_mx_2(r_sh(N, 0));
  if (ap_mx_2 == 0) {
    fz.exp = 0;
    fz.sig = 0;
    return fz.to_ieee();
  }
  iy -= (ap_mx_2.countLeadingZeros() - 1);
  iy += i;
  ap_mx_2 = ap_mx_2 << (ap_mx_2.countLeadingZeros() - 1);

  if (iy >= 1 - fy.EXP_BIAS) {
    fz.exp = iy + fy.EXP_BIAS;
    fz.sig(M - 1, 0) = ap_mx_2(M - 1, 0);
  } else {
    n = 1 - fy.EXP_BIAS - iy;
    ap_mx_2 >>= n;
    fz.exp = 0;
    fz.sig(M - 1, 0) = ap_mx_2(M - 1, 0);
  }
  return fz.to_ieee();
}
} // namespace fmdrmd

static double fmod(double x, double y) {
  return fmdrmd::generic_fmod<double>(x, y);
}
static float fmodf(float x, float y) {
  return fmdrmd::generic_fmod_mult<float>(x, y);
}
static half half_fmod(half x, half y) {
  return fmdrmd::generic_fmod<half>(x, y);
}
static float fmod(float x, float y) { return hls::fmodf(x, y); }
static half fmod(half x, half y) { return hls::half_fmod(x, y); }

static double remainder(double x, double y) {
  return fmdrmd::generic_remainder<double>(x, y);
}
static float remainderf(float x, float y) {
  return fmdrmd::generic_remainder<float>(x, y);
}
static half half_remainder(half x, half y) {
  return fmdrmd::generic_remainder<half>(x, y);
}
static float remainder(float x, float y) { return hls::remainderf(x, y); }
static half remainder(half x, half y) { return hls::half_remainder(x, y); }

static double remquo(double x, double y, int *quo) {
  return fmdrmd::generic_remquo<double>(x, y, quo);
}
static float remquof(float x, float y, int *quo) {
  return fmdrmd::generic_remquo<float>(x, y, quo);
}
static half half_remquo(half x, half y, int *quo) {
  return fmdrmd::generic_remquo<half>(x, y, quo);
}
static float remquo(float x, float y, int *quo) {
  return hls::remquof(x, y, quo);
}
static half remquo(half x, half y, int *quo) {
  return hls::half_remquo(x, y, quo);
}
// divide for
// Opencl----------------------------------------------------------------
template <class T> T generic_divide(T x, T y) {
  T z;
  z = x / y;
  return z;
}

static double divide(double x, double y) {
  return generic_divide<double>(x, y);
}

static float dividef(float x, float y) { return generic_divide<float>(x, y); }

static half half_divide(half x, half y) { return generic_divide<half>(x, y); }

#ifdef __cplusplus
static float divide(float x, float y) { return dividef(x, y); }
static half divide(half x, half y) {
  return generic_divide<half>(x, y);
}
#endif

;
#endif // entire file
