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
 *     (c) Copyright 2016 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/
/* File: hls_rsr.h
 *
 * Description:
 *     reciprocal, square root and reciprocal square root functions based on
 * algorithms introduced in "Reciprocation, Square Root, Inverse Square Root,
 * and Some Elementary Functions Using Small Multipliers" by Milos D.Ercegovac
 * et al.
 *
 */
#ifndef __HLS_RSR_HH
#define __HLS_RSR_HH
namespace hls_rsr {

#include "hls_rsr_tables.h"

// fixed point number to float point number with correct rounding
template <typename T, typename R>
void fixed2Float(T in, int exponent, int sign, R &result) {
  fp_struct<R> out;
  out.sign = sign;
  out.exp = exponent;

  T ain = in - static_cast<T>(0x1.0p0);
  assert((ain.wl() - ain.iwl() - fp_struct<R>::SIG_BITS) >= 0);
  assert(ain.wl() - ain.iwl() - 2 >= 0);
  int bits = ain.wl() - ain.iwl() - 2;
  // round to the nearesst, to even when tie
  if (bits >= fp_struct<R>::SIG_BITS) {
    ap_uint<1> Lb, Gb, Tb;
    Lb = ain[ain.wl() - ain.iwl() - fp_struct<R>::SIG_BITS];
    Gb = ain[ain.wl() - ain.iwl() - fp_struct<R>::SIG_BITS - 1];
    Tb = (ain(ain.wl() - ain.iwl() - fp_struct<R>::SIG_BITS - 2, 0) != 0) ? 1
                                                                          : 0;
    if (Gb & (Lb | Tb)) {
      ain += Trait<R>::RC;
    }
    if (ain >= static_cast<T>(0x1.0p0)) {
      ain -= static_cast<T>(0x1.0p0);
      ++out.exp;
      ain >>= 1;
    }
  }
  out.sig = ain(ain.wl() - ain.iwl() - 1,
                ain.wl() - ain.iwl() - fp_struct<R>::SIG_BITS);

  result = out.to_ieee();
}

// Reciprocal functions
template <typename T> class RecipTrait {
public:
  typedef ap_ufixed<fp_struct<T>::SIG_BITS + 1, 1> FxType;
  typedef ap_fixed<Trait<T>::n_bits - Trait<T>::k_bits + 1,
                   -Trait<T>::k_bits + 1>
      TypeA;
  typedef ap_uint<Trait<T>::k_bits * 2> TypeP;
  typedef ap_fixed<1 + Trait<T>::n_bits - 2 * Trait<T>::k_bits,
                   1 - 2 * Trait<T>::k_bits>
      TypeY;
  typedef typename Trait<T>::LUT_RType TypeR;
  typedef typename Trait<T>::LUT_RType TypeMT;
  typedef typename Trait<T>::LUT_RType TypeMM;
  typedef ap_ufixed<1 + Trait<T>::n_bits, 1> TypeRe;

  typedef ap_uint<Trait<T>::k_bits * 2 + 1> TypeS;
  static const int C_A22 = 1, C_A23 = 1, C_A222 = 1;
  static const int C_SHIFT = 0, A_SHIFT = 0;
  // when x = inf, return 0
  static bool f_procInf(const T &x, T &result) {
    result = ::hls::copysign(static_cast<T>(0), x);
    return true;
  }

  // when x = 0, return inf
  static bool f_procZero(const T &x, T &result) {
    result = ::hls::copysign(fp_struct<T>::infinity(), x);
    return true;
  }

  // when input is too too large, return zero
  static bool f_preProc(const T &x, T &result) {
    fp_struct<T> xg(x);
    if ((xg.exp == (1 << fp_struct<T>::EXP_BITS) - 2) ||
        ((xg.exp == (1 << fp_struct<T>::EXP_BITS) - 3) && xg.sig != 0)) {
      result = ::hls::copysign(static_cast<T>(0), x);
      return true;
    }
    return false;
  }

  static bool f_procOne(const T &x, T &result) {
    fp_struct<T> xg(x);
    fixed2Float<FxType, T>(1, fp_struct<T>::EXP_BIAS - xg.expv(), xg.sign,
                           result);
    return true;
  }

  static void f_evalueA(TypeY yA, TypeA &A0) { A0 = yA - A0; }
  static void f_getM(int indexM, TypeMM &M0) {
    M0 = RTable<TypeMM, half, Trait<T>::LUTsize>::arrayR[indexM];
  }

  // yR is the inverse of 1.Fx, in range of 0.5<= yR <1
  static void f_postProc(const T &x, TypeRe yR, T &result) {
    fp_struct<T> xg(x);
    yR <<= 1;
    fixed2Float(yR, fp_struct<T>::EXP_BIAS - xg.expv() - 1, xg.sign, result);
  }
};

// Square root functions
template <typename T> class SqrtTrait {
public:
  typedef ap_ufixed<fp_struct<T>::SIG_BITS + 1, 1> FxType;
  typedef ap_fixed<Trait<T>::n_bits - Trait<T>::k_bits + 1,
                   -Trait<T>::k_bits + 1>
      TypeA;
  typedef ap_uint<Trait<T>::k_bits * 2> TypeP;
  typedef ap_fixed<1 + Trait<T>::n_bits - 2 * Trait<T>::k_bits,
                   1 - 2 * Trait<T>::k_bits>
      TypeY;
  typedef typename Trait<T>::LUT_RType TypeR;
  typedef typename Trait<T>::LUT_MType TypeMM;
  typedef ap_ufixed<3 * Trait<T>::k_bits + 2, 1> TypeMT;
  typedef ap_uint<Trait<T>::k_bits * 2 + 1> TypeS;
  typedef ap_ufixed<1 + Trait<T>::n_bits, 1> TypeRe;

  static const int C_A22 = 1, C_A23 = 1, C_A222 = 1;
  static const int C_SHIFT = 1, A_SHIFT = 3;
  static const TypeRe SQRT2; // 1.4142135623730950488016887242097;
  // when x = inf, return 0
  static bool f_procInf(const T &x, T &result) {
    fp_struct<T> xg(x);
    if (xg.sign)
      return false;
    result = fp_struct<T>::infinity();
    return true;
  }
  // when x = 0, return inf
  static bool f_procZero(const T &x, T &result) {
    result = ::hls::copysign(static_cast<T>(0), x);
    return true;
  }
  // when input is negative except -0, return nan
  static bool f_preProc(const T &x, T &result) {
    fp_struct<T> xg(x);
    if (xg.sign && xg.exp) {
      result = ::hls::nan("");
      return true;
    }
    return false;
  }
  static bool f_procOne(const T &x, T &result) {
    fp_struct<T> xg(x);
    if (xg.expv() & 0x01)
      fixed2Float(SQRT2, fp_struct<T>::EXP_BIAS + (xg.expv() >> 1), 0, result);
    else
      fixed2Float<FxType, T>(1, fp_struct<T>::EXP_BIAS + (xg.expv() >> 1), 0,
                             result);
    return true;
  }

  static void f_evalueA(TypeY yA, TypeA &A0) { A0 = (A0 >> 1) - yA; }
  static void f_getM(int indexM, TypeMM &M0) {
    M0 = SqrtMTable<typename Trait<T>::LUT_MType, T,
                    Trait<T>::LUTsize>::arrayM[indexM];
  }

  // yR is the sqrt of 1.Fx, in range of 1<= yR <SQRT2
  static void f_postProc(const T &x, TypeRe yR, T &result) {
    fp_struct<T> xg(x);
    if (xg.expv() & 0x01)
      yR *= SQRT2;
    fixed2Float(yR, fp_struct<T>::EXP_BIAS + (xg.expv() >> 1), 0, result);
  }
};

template <typename T>
const ap_ufixed<1 + Trait<T>::n_bits, 1> SqrtTrait<T>::SQRT2 =
    1.4142135623730950488016887242097;

// Reciprocal square root function
template <typename T> class RsqrtTrait {
public:
  typedef ap_ufixed<fp_struct<T>::SIG_BITS + 1, 1> FxType;
  typedef ap_fixed<Trait<T>::n_bits - Trait<T>::k_bits + 1,
                   -Trait<T>::k_bits + 1>
      TypeA;
  typedef ap_uint<Trait<T>::k_bits * 2 + 3> TypeP;
  typedef ap_fixed<1 + Trait<T>::n_bits - 2 * Trait<T>::k_bits,
                   1 - 2 * Trait<T>::k_bits>
      TypeY;
  typedef typename Trait<T>::LUT_RType TypeR;
  typedef typename Trait<T>::LUT_MType TypeMM;
  typedef ap_ufixed<3 * Trait<T>::k_bits + 2, 1> TypeMT;
  typedef ap_uint<Trait<T>::k_bits * 2 + 3> TypeS;
  typedef ap_ufixed<1 + Trait<T>::n_bits, 1> TypeRe;

  static const int C_A22 = 3, C_A23 = 3, C_A222 = 5;
  static const int C_SHIFT = 1, A_SHIFT = 3;
  static const TypeRe SQRT2; // = 1.4142135623730950488016887242097;
  // when x = inf, return 0
  static bool f_procInf(const T &x, T &result) {
    fp_struct<T> xg(x);
    if (xg.sign)
      return false;
    result = 0.0;
    return true;
  }
  // when x = 0, return inf
  static bool f_procZero(const T &x, T &result) {
    result = ::hls::copysign(fp_struct<T>::infinity(), x);
    return true;
  }
  // when input is negative except -0, return nan
  static bool f_preProc(const T &x, T &result) {
    fp_struct<T> xg(x);
    if (xg.sign && xg.exp) {
      result = ::hls::nan("");
      return true;
    }
    return false;
  }
  static bool f_procOne(const T &x, T &result) {
    fp_struct<T> xg(x);
    if (xg.expv() & 0x01)
      fixed2Float(SQRT2, fp_struct<T>::EXP_BIAS - (xg.expv() >> 1) - 1, 0,
                  result);
    else
      fixed2Float<FxType, T>(1, fp_struct<T>::EXP_BIAS - (xg.expv() >> 1), 0,
                             result);
    return true;
  }

  static void f_evalueA(TypeY yA, TypeA &A0) { A0 = yA - (A0 >> 1); }

  static void f_getM(int indexM, TypeMM &M0) {
    M0 = RsqrtMTable<typename Trait<T>::LUT_MType, T,
                     Trait<T>::LUTsize>::arrayM[indexM];
  }

  // yR is the rsqrt of 1.Fx, in range of inv(SQRT2)<= yR <1
  static void f_postProc(const T &x, TypeRe yR, T &result) {
    fp_struct<T> xg(x);
    if (xg.expv() & 0x01)
      yR *= SQRT2;
    else
      yR <<= 1;
    fixed2Float(yR, fp_struct<T>::EXP_BIAS - ((xg.expv() + 2) >> 1), 0, result);
  }
};

template <typename T>
const ap_ufixed<1 + Trait<T>::n_bits, 1> RsqrtTrait<T>::SQRT2 =
    1.4142135623730950488016887242097;

template <typename T, typename FUN> T proc_generic(T x) {
  fp_struct<T> xg(x);
  T result;
  // if x = nan
  if (::hls::__isnan(x)) {
    return ::hls::nan("");
  }
  // if x = inf
  if (::hls::__isinf(x) && FUN::f_procInf(x, result)) {
    return result;
  }
  // if x = 0
  if (xg.exp == 0 && FUN::f_procZero(x, result)) {
    return result;
  }
  // if x is special for the function
  if (FUN::f_preProc(x, result))
    return result;
  // if x's mantissa part is one
  if (xg.sig == 0 && FUN::f_procOne(x, result)) {
    return result;
  }

  // y is x's mantissa
  typename FUN::FxType y = 0;
  y[y.wl() - y.iwl()] = 1;
  y(y.wl() - 1 - y.iwl(), y.wl() - y.iwl() - fp_struct<T>::SIG_BITS) = xg.sig;

  // indexR depends on first k_bits of y
  int indexR = y(y.wl() - y.iwl() - 1, y.wl() - y.iwl() - Trait<T>::k_bits);
  typename FUN::TypeR R0 =
      RTable<typename FUN::TypeR, T, Trait<T>::LUTsize>::arrayR[indexR];

  // A0 is less than 2^ -k_bits
  typename FUN::TypeA A0, Ap;
  if (indexR == 0) {
    A0 = y - 1;
  } else
    A0 = y * R0 - 1;

  bool sign_bit = false;
  if (A0[A0.wl() - 1]) {
    Ap = -A0;
    sign_bit = true;
  } else
    Ap = A0;
  ap_uint<Trait<T>::k_bits> A2, A3;
  A2 = Ap(Ap.wl() - 2, Ap.wl() - Trait<T>::k_bits - 1);
  A3 = Ap(Ap.wl() - Trait<T>::k_bits - 2, Ap.wl() - Trait<T>::k_bits * 2 - 1);
  typename FUN::TypeP A22 = A2 * A2, A23 = A2 * A3,
                      A222 = static_cast<ap_uint<Trait<T>::k_bits>>(
                                 A22 >> Trait<T>::k_bits) *
                             A2;

  A22 *= FUN::C_A22;
  A23 *= FUN::C_A23;
  A222 *= FUN::C_A222;

  typename FUN::TypeS Asum;
  if (sign_bit) {
    Asum = A22 + (A23 >> (Trait<T>::k_bits - 1)) +
           (A222 >> (Trait<T>::k_bits + FUN::C_SHIFT));
  } else {
    Asum = A22 + (A23 >> (Trait<T>::k_bits - 1)) -
           (A222 >> (Trait<T>::k_bits + FUN::C_SHIFT));
  }
  typename FUN::TypeY yA = 0;
  yA(Trait<T>::k_bits * 2, 0) = Asum >> FUN::A_SHIFT;
  FUN::f_evalueA(yA, A0);

  typename FUN::TypeMM M0 = R0;
  typename FUN::TypeRe yR;
  if (indexR == 0)
    yR = A0 + 1;
  else {
    FUN::f_getM(indexR, M0);
    yR = A0 * static_cast<typename FUN::TypeMT>(M0);
    yR += M0;
  }

  FUN::f_postProc(x, yR, result);
  return result;
}

template <typename T> static T recip(T x) {
  return proc_generic<T, RecipTrait<T>>(x);
}
static half half_recip(half x) { return recip<half>(x); }

template <typename T> static T sqrt(T x) {
  return proc_generic<T, SqrtTrait<T>>(x);
}
static half half_sqrt(half x) { return sqrt<half>(x); }

template <typename T> static T rsqrt(T x) {
  return proc_generic<T, RsqrtTrait<T>>(x);
}
static half half_rsqrt(half x) { return rsqrt<half>(x); }

// TODO support to float and double
} // namespace hls_rsr

#endif
