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
 *     (c) Copyright 2008-2010 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/**
 * @file cordic.h
 * This file contains the CORDIC function.
 */

#ifndef X_HLS_CORDIC_H
#define X_HLS_CORDIC_H

//#include "utils/x_hls_defines.h"
//#include "utils/x_hls_utils.h"

#include "hls_big_mult.h"
#include "hls_cordic_tables.h"
#include "hls_range_redux.h"

/**
 * The CORDIC function implements a configurable optimized CORDIC block.
 * The block naturally operates on fixed point data but the input data width and
 * associated number of iterations can be arbitrarily extended.
 *
 *
 */

/* @{ */

namespace cordic {

template <typename T> class _const {};
template <> class _const<half> {
public:
  static const uint16_t zero = 0x0000;
  static const uint16_t pi_4 = 0x3a48;
  static const uint16_t pi_2 = 0x3e48;
  static const uint16_t _3pi_4 = 0x40b6;
  static const uint16_t pi = 0x4248;
};
template <> class _const<float> {
public:
  static const uint32_t zero = 0x00000000;
  static const uint32_t pi_4 = 0x3F490FDB;
  static const uint32_t pi_2 = 0x3FC90FDB;
  static const uint32_t _3pi_4 = 0x4016CBE4;
  static const uint32_t pi = 0x40490FDB;
};

template <> class _const<double> {
public:
  static const uint64_t zero = 0x0000000000000000;
  static const uint64_t pi_4 = 0x3FE921FB54442D18;
  static const uint64_t pi_2 = 0x3FF921FB54442D18;
  static const uint64_t _3pi_4 = 0x4002D97C7F3321D2;
  static const uint64_t pi = 0x400921FB54442D18;
};

/**
 * Performs selectable add/sub operation.
 *
 * @param a First input variable.
 * @param b Second input variable.
 * @param add Add/sub selector. 1 = add. 0 = subtract.
 *
 * @return Result of addsub operation.
 */
template <int II1, int LAT1, class T2, class T3, class T4>
typename x_traits<T2, T3>::ADD_T addsub(T2 a, T3 b, T4 add) {
  if (II1 == 0) {
    if (LAT1 == 1) {
#pragma HLS INTERFACE ap_none port = return register
    }
  }
#pragma HLS LATENCY MAX = LAT1

  typename x_traits<T2, T3>::ADD_T tmp;
  if (add) {
    tmp = a + b;
  } else {
    tmp = a - b;
  }
  return tmp;
}

/**
 * Main CORDIC sub-function (version 1) which performs the shift/addsub
 * operations. K =~ 1.64676 MODE 0: 'Rotation' : rotate by angle z.  x' <-
 * K*(x*cos(z) - y*sin(z)), y' <- K*(x*sin(z) + y*cos(z)) MODE 1: 'Vectoring':
 * rotate y to zero.   x' <- sqrt(x^2 + y^2)  , z' <- atan(y/x)
 */
template <int N, int MODE, int II1, int LAT1, int W, int I, int W2, int I2>
void cordic_circ_v1(ap_fixed<W, I> &x, ap_fixed<W, I> &y, ap_fixed<W2, I2> &z) {
#pragma HLS INLINE SELF
#ifdef DEBUG
  std::cout << "x: " << x.to_string(16) << ", y: " << y.to_string(16)
            << ", z: " << z.to_string(16) << std::endl;
#endif

  typedef ap_fixed<W, I> T;

  const int n = (N > W) ? W : N;
  const int LAT2 = LAT1 / N;

  int k;
  T tx, ty;
  ap_fixed<W2, I2> tz;
  ap_uint<1> d;

  for (k = 0; k < n; ++k) {
#ifdef DEBUG
    std::cout << "k: " << std::dec << k;
#endif
    if (MODE == 1) {
      d = ~y[W - 1];
#ifdef DEBUG
      std::cout << ", x: " << std::hex << x.to_string(16) << " " << std::dec
                << x.to_string(10);
      std::cout << ", y: " << std::hex << y.to_string(16) << "(" << y[W - 1]
                << "," << d << ")";
      std::cout << ", z: " << std::hex << z.to_string(16) << " " << std::dec
                << z.to_string(10);
      std::cout << std::endl;
#endif
    } else { // == 0
      d = z[W - 1];
#ifdef DEBUG
      std::cout << ", x: " << std::hex << x.to_string(16) << " " << std::dec
                << x.to_string(10);
      std::cout << ", y: " << std::hex << y.to_string(16) << " " << std::dec
                << y.to_string(10);
      std::cout << ", z: " << std::hex << z.to_string(16) << "(" << z[W - 1]
                << "," << d << ")";
      std::cout << std::endl;
#endif
    }
    T y_s = y >> k;
    tx = addsub<II1, LAT2>(x, y_s, d);
    T x_s = x >> k;
    ty = addsub<II1, LAT2>(y, x_s, !d);
    ap_fixed<W2, I2, AP_RND> z_s;
    z_s = cordic_ctab_table_128[k]; // Round here.
    tz = addsub<II1, LAT2>(z, z_s, d);
    x = tx;
    y = ty;
    z = tz;
  }
#ifdef DEBUG
  std::cout << "x: " << x.to_string(16) << ", y: " << y.to_string(16)
            << ", z: " << z.to_string(16) << std::endl;
#endif
}

/**
 * Main hyperbolic CORDIC sub-function (version 1) which performs the
 * shift/addsub operations. K =~ 0.82816 MODE 0: 'Rotation' : rotate by angle z.
 * x' <- K*(x*cosh(z) + y*sinh(z)), y' <- x*sinh(z) + y*cosh(z) MODE 1:
 * 'Vectoring': rotate y to zero.   x' <- K*sqrt(x^2 - y^2)  , z' <-
 * z+atanh(y/x)
 */
template <int W, int N, int MODE, int II1, int LAT1, class T, class T2>
void cordic_hyperb_v1(T &x, T &y, T2 &z) {
#pragma HLS INLINE SELF
#ifdef DEBUG
  std::cout << "x: " << x.to_string(16) << ", y: " << y.to_string(16)
            << ", z: " << z.to_string(16) << std::endl;
#endif

  const int n = (N > W) ? W : N;
  const int n1 = n + n / 4;
  const int LAT2 = LAT1 / N;

  int k1;
  T tx, ty;
  T2 tz;
  ap_uint<1> d;

  for (k1 = 0; k1 < n1; ++k1) {
    int k = k1 - (k1 / 4) + 1;
#ifdef DEBUG
    std::cout << "k: " << std::dec << k << ", k1: " << std::dec << k1;
#endif
    if (MODE == 1) {
      d = ~y[n - 1];
#ifdef DEBUG
      std::cout << ", x: " << std::hex << x.to_string(16) << " " << std::dec
                << x.to_string(10);
      std::cout << ", y: " << std::hex << y.to_string(16) << "(" << y[W - 1]
                << "," << d << ")";
      std::cout << ", z: " << std::hex << z.to_string(16) << " " << std::dec
                << z.to_string(10);
      std::cout << std::endl;
#endif
    } else { // == 0
      d = z[n - 1];
#ifdef DEBUG
      std::cout << ", x: " << std::hex << x.to_string(16) << " " << std::dec
                << x.to_string(10);
      std::cout << ", y: " << std::hex << y.to_string(16) << " " << std::dec
                << y.to_string(10);
      std::cout << ", z: " << std::hex << z.to_string(16) << "(" << z[n - 1]
                << "," << d << ")";
      std::cout << ", zs: " << cordic_hyperb_table_128[k - 1].to_string(16);
      std::cout << std::endl;
#endif
    }
    T y_s = y >> k;
    tx = addsub<II1, LAT2>(x, y_s, !d);
    T x_s = x >> k;
    ty = addsub<II1, LAT2>(y, x_s, !d);
    T z_s = cordic_hyperb_table_128[k - 1];
    tz = addsub<II1, LAT2>(z, z_s, d);
    x = tx;
    y = ty;
    z = tz;
  }
#ifdef DEBUG
  std::cout << "x: " << x.to_string(16) << ", y: " << y.to_string(16)
            << ", z: " << z.to_string(16) << std::endl;
#endif
}

template <typename T> struct cordic_sincos_traits {};

template <> struct cordic_sincos_traits<double> {
  typedef double T;
  static const int p1 = 61;
  static const int g = 5;
  static const int SMALL_ANGLE_THRESHOLD_BITS = 27;
  // For cordic, the # fractional bits we need is SIG_BITS + the
  // number of potential zeros + some guard bits
  static const int CW = fp_struct<T>::SIG_BITS + SMALL_ANGLE_THRESHOLD_BITS + g;
};

template <> struct cordic_sincos_traits<float> {
  typedef float T;
  static const int p1 = 29;
  static const int g = 5;
  static const int SMALL_ANGLE_THRESHOLD_BITS = 12;
  // For cordic, the # fractional bits we need is SIG_BITS + the
  // number of potential zeros + some guard bits
  static const int CW = fp_struct<T>::SIG_BITS + SMALL_ANGLE_THRESHOLD_BITS + g;
};

template <> struct cordic_sincos_traits<half> {
  typedef half T;
  static const int p1 = 12;
  static const int g = 5;
  static const int SMALL_ANGLE_THRESHOLD_BITS = 5;
  // For cordic, the # fractional bits we need is SIG_BITS + the
  // number of potential zeros + some guard bits
  static const int CW = fp_struct<T>::SIG_BITS + SMALL_ANGLE_THRESHOLD_BITS + g;
};

/*
 * CORDIC-based sin/cos using payne-hanek range-reduction algorithm
 * single-precision float
 */
template <typename T> void cordic_sincos_generic(T t_in, T &s_out, T &c_out) {
  const int p1 = cordic_sincos_traits<T>::p1;
  const int bits = cordic_sincos_traits<T>::SMALL_ANGLE_THRESHOLD_BITS;
  // For cordic, the # fractional bits we need is SIG_BITS + the
  // number of potential zeros + some guard bits
  const int CW = cordic_sincos_traits<T>::CW;

  fp_struct<T> din(t_in);

  if (din.exp == fp_struct<T>::EXP_INFNAN) { // +inf, -inf, nan
    s_out = hls::nan("");
    c_out = hls::nan("");
    return;
  } else if (din.exp < (fp_struct<T>::EXP_BIAS - bits)) {
    // Small input approximation
    c_out = (T)1.0;
    s_out = t_in;
    return;
  }

  ap_uint<3> k;
  ap_ufixed<CW, 0> fout;
  // Mx could be narrower (~SIG_BITS), but the range reduction function below
  // uses the width of Mx to perform the computation for fout.
  // HOTBM requires more precision from Mx than fout.  For cordic,
  // the situation is reversed.
  ap_ufixed<CW, 0> Mx;
  ap_int<fp_struct<T>::EXP_BITS> Ex, yprescale;
  range_redux_payne_hanek_hotbm<p1>(t_in, k, fout, Mx, Ex);
  ap_ufixed<CW, 0> piover4(
      "0.78539816339744830961566084581987572104929234984377");

  ap_fixed<CW + 3, 3> x, y, z;
  if (Ex < -bits) {
    x = (T)1.0;
    y = Mx * piover4;
    yprescale = -Ex;
#ifdef DEBUG
    std::cout << "close after range reduction: " << fout << " "
              << fout.to_string(16) << "\n";
#endif
  } else {
#ifdef DEBUG
    std::cout << "k: " << k << " din: " << din << " fout: " << fout << " "
              << fout.to_string(16) << "\n";
#endif
    x = ap_fixed<CW + 3, 3, AP_RND>(
        "0.6072529350088812561694467525116341097505455558743"); // cordic
                                                                // constant
    y = 0;
    // This is a moderately large multiplier.  However, the cordic
    // coefficients could alternatively be scaled by piover4.
    z = fout * piover4;
    cordic_circ_v1<CW + 3, 0, 0, 0>(x, y, z);
    yprescale = 0;
  }

#ifdef DEBUG
  std::cout << "scaled_y: " << y << " yprescale: " << yprescale
            << " piover4: " << piover4 << "\n";
#endif

  T tmp_s_out, tmp_c_out;
  scaled_fixed2ieee(y, tmp_s_out, yprescale);
  scaled_fixed2ieee(x, tmp_c_out, 0);
  //    T tmp_c_out = x; // this doesn't work!;

#ifdef DEBUG
  std::cout << "tmp_s_out: " << std::scientific << tmp_s_out << "(" << std::hex
            << *(int *)(&tmp_s_out) << "), "
            << "tmp_c_out: " << std::scientific << tmp_c_out << "(" << std::hex
            << *(int *)(&tmp_c_out) << ")\n";

#endif

  // This could probably be simplified
  int sel = (din.__signbit()) ? 7 - k.to_uint() : k.to_uint();
  switch (sel) {
  case 0:
    s_out = tmp_s_out;
    c_out = tmp_c_out;
    break;
  case 1:
    s_out = tmp_c_out;
    c_out = tmp_s_out;
    break;
  case 2:
    s_out = tmp_c_out;
    c_out = -tmp_s_out;
    break;
  case 3:
    s_out = tmp_s_out;
    c_out = -tmp_c_out;
    break;
  case 4:
    s_out = -tmp_s_out;
    c_out = -tmp_c_out;
    break;
  case 5:
    s_out = -tmp_c_out;
    c_out = -tmp_s_out;
    break;
  case 6:
    s_out = -tmp_c_out;
    c_out = tmp_s_out;
    break;
  case 7:
    s_out = -tmp_s_out;
    c_out = tmp_c_out;
    break;
  default: // case 0
    s_out = tmp_s_out;
    c_out = tmp_c_out;
  }
}

/*
* CORDIC-based sin/cos using payne-hanek range-reduction algorithm

static void
cordic_sincos_generic(
    double t_in,
    double &s_out,
    double &c_out)
{
    const int p1 = 16;
    const int CW = 78;

    fp_struct<double> din(t_in);

    ap_uint<3> k;
        ap_uint<CW> dout;

    range_redux_payne_hanek<p1,CW,double>(t_in,k,dout);

    ap_int<CW> dina1, souta, couta;
    dina1 = dout >> 2;
#ifdef DEBUG
    printf("dout: %s, dina1: %s, k:
%d\n",dout.to_string(16).c_str(),dina1.to_string(16).c_str(),k.to_uint());
#endif
        ap_fixed<CW+1,3> x, y, z;
        x.range() = ref_cordic(99,100-CW) >> 2;
        y = 0;
        z.range() = dina1;
        cordic_circ_v1<CW,0,0,0>(x,y,z);
        couta = x.range();
        souta = y.range();
#ifdef DEBUG
    ap_fixed<CW,2> dina1_fxd; dina1_fxd.range() = dina1; double dina1_d =
dina1_fxd; ap_fixed<CW,2> souta_fxd; souta_fxd.range() = souta; double souta_d =
souta_fxd; ap_fixed<CW,2> couta_fxd; couta_fxd.range() = couta; double couta_d =
couta_fxd;

    std::cout << "dina1: " << std::scientific << dina1_d
              << "(" << dina1_fxd.to_string(16).c_str() << "), "
              << "souta: " << std::scientific << souta_d
              << "(" << souta_fxd.to_string(16).c_str() << "), "
              << "couta: " << std::scientific << couta_d
              << "(" << couta_fxd.to_string(16).c_str() << ")\n";
#endif

    if(dina1 != 0) {
        souta = souta << 1;
        couta = couta << 1;
    } else {
#ifdef DEBUG
        std::cout << "dina1 is all 0s\n";
#endif
        souta = 0;
        couta = ((ap_uint<CW>)0x1 << (CW-1)) - dina1;
    }
    double tmp_s_out, tmp_c_out;
    tmp_s_out = convToDouble<CW>(souta);
    tmp_c_out = convToDouble<CW>(couta);

    int sel = (din.__signbit()) ? 7 - k.to_uint() : k.to_uint();
    switch(sel) {
        case 0:
            s_out =  tmp_s_out;
            c_out =  tmp_c_out;
            break;
        case 1:
            s_out =  tmp_c_out;
            c_out =  tmp_s_out;
            break;
        case 2:
            s_out =  tmp_c_out;
            c_out = -tmp_s_out;
            break;
        case 3:
            s_out =  tmp_s_out;
            c_out = -tmp_c_out;
            break;
        case 4:
            s_out = -tmp_s_out;
            c_out = -tmp_c_out;
            break;
        case 5:
            s_out = -tmp_c_out;
            c_out = -tmp_s_out;
            break;
        case 6:
            s_out = -tmp_c_out;
            c_out =  tmp_s_out;
            break;
        case 7:
            s_out = -tmp_s_out;
            c_out =  tmp_c_out;
            break;
        default: // case 0
            s_out = tmp_s_out;
            c_out = tmp_c_out;
    }

    if(din.exp == fp_struct<double>::EXP_INFNAN) { // +inf, -inf, nan
        s_out = hls::nan("");
        c_out = hls::nan("");
    } else if(din.exp < (fp_struct<double>::EXP_BIAS-25)) {  // exp=-24 ->
ulp=3, exp=-25 -> ulp=1
        // small numbers
        c_out = 1.0;
        s_out = t_in;
    }
}
*/

static void sincos(double t_in, double *s, double *c) {
  double s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  *s = s_out;
  *c = c_out;
}
static double sin(double t_in) {
  double s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  return s_out;
}
static double cos(double t_in) {
  double s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  return c_out;
}

static void sincosf(float t_in, float *s, float *c) {
  float s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  *s = s_out;
  *c = c_out;
}

static float sinf(float t_in) {
  float s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  return s_out;
}

static float cosf(float t_in) {
  float s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  return c_out;
}

static void half_sincos(half t_in, half *s, half *c) {
  half s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  *s = s_out;
  *c = c_out;
}
static half half_sin(half t_in) {
  half s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  return s_out;
}
static half half_cos(half t_in) {
  half s_out, c_out;
  cordic_sincos_generic(t_in, s_out, c_out);
  return c_out;
}

/*
 * CORDIC-based atan restricted to range 0 to 1
 * single-precision float
 */

template <typename T> struct atan_traits {};

template <> struct atan_traits<half> {
  static const int SMALL_ANGLE_THRESHOLD_BITS = 7;
  // 40 bits of precision requires 4 guard bits.
  static const int GUARD_BITS = 4;
  static half div(half y, half x) { return y / x; }
};
template <> struct atan_traits<float> {
  static const int SMALL_ANGLE_THRESHOLD_BITS = 11;
  // 40 bits of precision requires 4 guard bits.
  static const int GUARD_BITS = 4;
  static float div(float y, float x) { return HLS_FPO_DIVF(y, x); }
};
template <> struct atan_traits<double> {
  static const int SMALL_ANGLE_THRESHOLD_BITS = 27;
  // 80 bits of precision requires 5 guard bits.
  static const int GUARD_BITS = 5;
  static double div(double y, double x) { return HLS_FPO_DIV(y, x); }
};

/*
 * CORDIC-based atan
 * Assumptions: Range of t_in is [0..1], corresponding to atan(din) in [0..pi/4]
 */
template <typename T> T atan_generic(T t_in) {
  fp_struct<T> fps1(t_in);

  if (fps1.exp <
      fp_struct<T>::EXP_BIAS - atan_traits<T>::SMALL_ANGLE_THRESHOLD_BITS) {
    // small angle approximation: atan(din) ~= din;
#ifdef DEBUG
    std::cout << "small angle\n";
#endif
    return t_in;
  }

  // This comes from (approximately, for float):
  // 1) We know that there is a 1 in the first 12 bits, since we're not doing
  // the small angle approximation. 2) After the first 12 bits, we need 24
  // significant bits for single precision mantissa 3) We know there is going to
  // be some rounding error.  The rounding error is very small after a number of
  // terms, so we can bound this error based on the angle constants in the
  // cordic table. 40 bits of precision requires 4 guard bits. 12 + 24 + 4 = 40.
  const int CW = atan_traits<T>::SMALL_ANGLE_THRESHOLD_BITS + 1 +
                 fp_struct<T>::SIG_BITS + atan_traits<T>::GUARD_BITS;

  // The maximum mantissa value is 1.FFFFF...  (basically 2).
  // The maximum value of x or y is sqrt(2*2+1*1)*1.67 ~= 3.73, which requires 2
  // bits + sign bit. z has a max value of pi/4 ~= .785, so just a sign bit is
  // enough.
  ap_fixed<CW + 3, 3> x, y;
  ap_fixed<CW + 1, 1> z;

  int exp = fps1.expv();
  ap_ufixed<CW + 1, 1> X = fps1.mantissa();
  // This is a negative shift
  X = X << exp;
#ifdef DEBUG
  std::cout << "t_in: " << std::scientific << t_in << ", exp: " << std::dec
            << exp << ", X: " << X << " " << X.to_string(16).c_str()
            << std::endl;
#endif

  x = 1;
  y = X;
  z = 0;

  cordic_circ_v1<CW + 3, 1, 0, 0>(x, y, z);

  return z; // convToFloat<CW,3>(z);
}

/**
 * atan(x) = pi/2 - atan(1/x)
 */
template <typename T> T atan_cordic(T x) {
  fp_struct<T> xs(x);
  ap_uint<1> neg = xs.sign;
  xs.sign = 0;
  T absx = xs.to_ieee();

  const T pi_over_2 = fp_struct<T>(_const<T>::pi_2).to_ieee();
  const T pi_over_4 = fp_struct<T>(_const<T>::pi_4).to_ieee();
  T res;
  if (xs.exp == fp_struct<T>::EXP_INFNAN) { // +inf, -inf, nan
    if (xs.sig == 0) {                      // +inf, - inf
      res = pi_over_2;
    } else {
      res = hls::nan("");
    }
  } else if (absx == (T)1.0) {
    // special case for input == 1.0
    res = pi_over_4;
  } else {
    // octant 0 or 1
    bool octant1 = xs.exp >= fp_struct<T>::EXP_BIAS; // (absx > (T)1.0);
    // Range of din is [0..1], corresponding to atan(din) in [0..pi/4]
    T one_over_x = (T)1.0 / absx;
    T din = octant1 ? one_over_x : absx;
    res = atan_generic(din);
    if (octant1) {
#ifdef DEBUG
      std::cout << "octant 1\n";
#endif
      // octant 1 : atan(x) = pi/2-atan(din0);
      res = pi_over_2 - res;
    } else {
#ifdef DEBUG
      std::cout << "octant 0\n";
#endif
    }
  }

  fp_struct<T> tmpout(res);
  tmpout.sign = neg;
  return tmpout.to_ieee();
  ;
}

static half half_atan(half t_in) { return atan_cordic(t_in); }
static float atanf(float t_in) { return atan_cordic(t_in); }
static double atan(double t_in) { return atan_cordic(t_in); }

/*
 * x_in and y_in are both positive and y_in < x_in
 * output is in [0..pi/4]
 */
template <typename T> T atan2_generic(T y_in, T x_in) {
  // normalize y_in and x_in
  fp_struct<T> fps_x(x_in);
  fp_struct<T> fps_y(y_in);

  // Note that there is some subnormal handling that is probably duplicated
  // in the floating point division core.
  if (fps_y.exp + atan_traits<T>::SMALL_ANGLE_THRESHOLD_BITS < fps_x.exp) {
    // small angle approximation: atan(din) ~= din;
#ifdef DEBUG
    std::cout << "division: y_exp = " << fps_y.exp << " x_exp = " << fps_x.exp
              << "\n";
#endif
    return atan_traits<T>::div(y_in, x_in);
  }
  if (fps_y.exp == 0) {
    return (T)0;
  }

  // This comes from (approximately, for float):
  // 1) We know that there is a 1 in the first 12 bits, since we're not doing
  // the small angle approximation. 2) After the first 12 bits, we need 24
  // significant bits for single precision mantissa 3)hls::__isinf(x_in) We know
  // there is going to be some rounding error.  The rounding error is very small
  // after a number of terms, so we can bound this error based on the angle
  // constants in the cordic table. 40 bits of precision requires 4 guard bits.
  // 12 + 24 + 4 = 40.
  const int CW = atan_traits<T>::SMALL_ANGLE_THRESHOLD_BITS + 1 +
                 fp_struct<T>::SIG_BITS + atan_traits<T>::GUARD_BITS;

  // The maximum mantissa value is 1.FFFFF...  (basically 2).
  // The maximum value of x or y is 2*sqrt(2)*1.67 ~= 4.72, which requires 3
  // bits + sign bit. z has a max value of pi/4 ~= .785, so just a sign bit is
  // enough.
  ap_fixed<CW + 4, 4> x, y;
  ap_fixed<CW + 1, 1> z;

  // This is the number of bits we need to shift left if
  // both x and y are subnormals, in order to avoid loss of precision
  // int lz = (fps_x.sig | fps_y.sig).countLeadingZeros();

  int d_exp = fps_x.exp - fps_y.exp;
  // Handle shifting correctly if y is subnormal and x is not.

  x = fps_x.mantissa();
  y = fps_y.mantissa();
  //    if (!x_subnormal && y_subnormal) d_exp--;

  y >>= d_exp;
  z = 0;
  // if(both_subnormal) y <<= lz;
  // if(both_subnormal) x <<= lz;

  cordic_circ_v1<CW + 3, 1, 0, 0>(x, y, z);

  return z; // convToFloat(z);
}

/*
 * x_in and y_in are both positive (first quadrant)
 * output is in [0..pi/2]
 */
template <class T> static T atan2_cordic_Q1(T y_in, T x_in) {
  //#pragma HLS ALLOCATION instances=atan2_generic limit=1 function
  fp_struct<T> fpsx(x_in);
  fp_struct<T> fpsy(y_in);
  if (fpsx.data() == fpsy.data())
    return fp_struct<T>(_const<T>::pi_4).to_ieee();
  else if (y_in > x_in)
    return fp_struct<T>(_const<T>::pi_2).to_ieee() - atan2_generic(x_in, y_in);
  else
    return atan2_generic(y_in, x_in);
}

template <class T> static T atan2_cordic(T y_in, T x_in) {
  fp_struct<T> fpsx(x_in);
  fp_struct<T> fpsy(y_in);
  if (hls::__isnan(y_in) || hls::__isnan(x_in)) { // y or x is NaN
    return hls::nan("");
  }

  // process y or x is zero
  if (fpsy.exp == 0 && fpsy.sig == 0 && fpsx.__signbit())
    return hls::copysign(fp_struct<T>(_const<T>::pi).to_ieee(), y_in);
  if (fpsy.exp == 0 && fpsy.sig == 0 && ~fpsx.__signbit())
    return hls::copysign(fp_struct<T>(_const<T>::zero).to_ieee(), y_in);
  if (fpsx.exp == 0 && fpsx.sig == 0)
    return hls::copysign(fp_struct<T>(_const<T>::pi_2).to_ieee(), y_in);

  // Flush subnormals to zero.
  if (fpsx.exp == 0) {
    fpsx.sig = 0;
  }
  if (fpsy.exp == 0) {
    fpsy.sig = 0;
  }
  if (fpsy.exp == 0 && fpsx.__signbit())
    return hls::copysign(fp_struct<T>(_const<T>::pi).to_ieee(), y_in);
  if (fpsy.exp == 0 && ~fpsx.__signbit())
    return hls::copysign(fp_struct<T>(_const<T>::zero).to_ieee(), y_in);
  if (fpsx.exp == 0)
    return hls::copysign(fp_struct<T>(_const<T>::pi_2).to_ieee(), y_in);

  int m = ((fpsx.sign.to_int() << 1) |
           fpsy.sign.to_int()); // m: bit_1: sign of x; bit_0: sign of y

  //     if ( fpsy.exp == 0 ) { // y = +-0 or subnormal
  //         switch(m) {
  //             case 0: return fp_struct<T>(_const<T>::zero).to_ieee(); //
  //             atan2(+0, +x) = +0 case 1: return
  //             -fp_struct<T>(_const<T>::zero).to_ieee(); // atan2(-0, +x) = -0
  //             case 2: return fp_struct<T>(_const<T>::pi).to_ieee(); //
  //             atan2(+0, -x) = pi case 3: return
  //             -fp_struct<T>(_const<T>::pi).to_ieee(); // atan2(-0, -x) = -pi
  //         }
  //     } else if ( fpsx.exp == 0 ) { // x = +-0 or subnormal, y != 0
  //         switch(m&1) {
  //             case 0: return fp_struct<T>(_const<T>::pi_2).to_ieee(); //
  //             atan2(+y, +-0) = pi/2 case 1: return
  //             -fp_struct<T>(_const<T>::pi_2).to_ieee(); // atan2(-y, +-0) =
  //             -pi/2
  //         }
  //     } else
  if (hls::__isinf(x_in) && hls::__isinf(y_in)) { // x, y = +-INF
    switch (m) {
    case 0:
      return fp_struct<T>(_const<T>::pi_4)
          .to_ieee(); // atan2(+INF, +INF) = pi/4
    case 1:
      return -fp_struct<T>(_const<T>::pi_4)
                  .to_ieee(); // atan2(-INF, +INF) = -pi/4
    case 2:
      return fp_struct<T>(_const<T>::_3pi_4)
          .to_ieee(); // atan2(+INF, -INF) = 3*pi/4
    case 3:
      return -fp_struct<T>(_const<T>::_3pi_4)
                  .to_ieee(); // atan2(-INF, -INF) = -3*pi/4
    }
  } else if (hls::__isinf(x_in)) { // x = +- INF, y != +- INF
    switch (m) {
    case 0:
      return fp_struct<T>(_const<T>::zero).to_ieee(); // atan2(+y, +INF) = +0
    case 1:
      return -fp_struct<T>(_const<T>::zero).to_ieee(); // atan2(-y, +INF) = -0
    case 2:
      return fp_struct<T>(_const<T>::pi).to_ieee(); // atan2(+y, -INF) = pi
    case 3:
      return -fp_struct<T>(_const<T>::pi).to_ieee(); // atan2(-y, -INF) = -pi
    }
  } else if (hls::__isinf(y_in)) { // y = +- INF, x != +- INF
    switch (m & 1) {
    case 0:
      return fp_struct<T>(_const<T>::pi_2).to_ieee(); // atan2(+INF, +-x) = pi/2
    case 1:
      return -fp_struct<T>(_const<T>::pi_2)
                  .to_ieee(); // atan2(-INF, +-x) = -pi/2
    }
  } else { // x, y != +-INF
    T a = hls::abs(y_in);
    T b = hls::abs(x_in);
    T c = atan2_cordic_Q1(a, b);
    T d = fp_struct<T>(_const<T>::pi).to_ieee() - c;
    switch (m) {
    case 0:      // quadrant I
      return c;  // atan2_cordic_floatdouble(y_in, x_in);
    case 1:      // quadrant IV
      return -c; //-atan2_cordic_floatdouble(-y_in, x_in);
    case 2:      // quadrant II
      return d;  // fp_struct<T>(_const<T>::pi).to_ieee() -
                // atan2_cordic_floatdouble(y_in, -x_in);
    case 3:      // quadrant III
      return -d; // atan2_cordic_floatdouble(-y_in, -x_in) -
                 // fp_struct<T>(_const<T>::pi).to_ieee();
    }
  }
  return 0;
}

static half half_atan2(half y_in, half x_in) {
  return atan2_cordic(y_in, x_in);
}

static float atan2f(float y_in, float x_in) { return atan2_cordic(y_in, x_in); }

static double atan2(double y_in, double x_in) {
  return atan2_cordic(y_in, x_in);
}

template <typename T> struct hyperbolic_traits {
  /*
   * H_INT_MAX: maximum value for which sinh(x) = inf. float(7), double(9),
   * theoretical(11) Basically: ceil(log2(H_MAX_INPUT)); H_DEC_MIN: minimum
   * value fo rwhich sinh(x) = x.   float(-12), double(-28) N: Fractional bits
   * of precision for cordic.  ~SIG_BITS + H_DEC_MIN + guard bits.
   */
};

template <> struct hyperbolic_traits<half> {
  // max = 65504.  asinh(65504) ~= 11.783.
  static const typename fp_struct<half>::inttype H_MAX_INPUT =
      0x49e4; // 11.783013669079;
  static const int H_INT_MAX = 4;
  static const int H_DEC_MIN = 5;
  static const int N = fp_struct<half>::SIG_BITS + H_DEC_MIN + 2;
};
template <> struct hyperbolic_traits<float> {
  static const typename fp_struct<float>::inttype H_MAX_INPUT =
      0x42b2d4fc; // 710.48;
  static const int H_INT_MAX = 7;
  static const int H_DEC_MIN = 12;
  static const int N = fp_struct<float>::SIG_BITS + H_DEC_MIN + 4;
};
template <> struct hyperbolic_traits<double> {
  static const typename fp_struct<double>::inttype H_MAX_INPUT =
      0x408633ce8fb9f87d; // asinh(std::numeric_limits<double>::max());
  static const int H_INT_MAX = 10;
  static const int H_DEC_MIN = 28;
  static const int N = fp_struct<double>::SIG_BITS + H_DEC_MIN + 5;
};

/**
 * Range reduces input to between 0 and ln2 by
 * solving for k and r in x = k*ln2 + r, where x = t_in.
 *
 * Theoretical max value of x is 710.48 where any larger value gives sinh(x) =
 * inf In practcie, for sinhf(float), x,max < 2^7 and sinh(double), x,max < 2^9
 *
 * k = (int)(x/ln2); k,max = 1025 (11 bits)
 * y = (dec)(x/ln2)*ln2
 *
 * If t_in > X, the final sinh/cosh result will be inf so we set k = 130 to
 * indicate that downstream.
 *
 * H_INT_MAX: maximum value for which sinh(x) = inf. float(7), double(9),
 * theoretical(11) H_DEC_MIN: minimum value fo rwhich sinh(x) = x.   float(-12),
 * double(-28) k,max = x,max/ln2 or approx x,width+1
 *
 * Assumptions:
 * T is a floating point type.
 * 0 < t <= H_MAX_INPUT
 */
template <typename T>
void hyperb_range_redux(T t, ap_uint<hyperbolic_traits<T>::H_INT_MAX + 1> &k,
                        ap_ufixed<hyperbolic_traits<T>::N, 0> &t_out) {
  fp_struct<T> ts(t);

  const int F1 = hyperbolic_traits<T>::N;
  const int I1 = hyperbolic_traits<T>::H_INT_MAX;
  const int W1 = I1 + F1;

  ap_ufixed<W1, I1> t_f = ts.mantissa();
  // May be negative shift
  t_f <<= ts.expv();

  ap_ufixed<F1, 1> inv_ln2(
      "0x1.71547652b82fe1777d0ffda0d23a7d11d6aef551cp0"); // ~= 1.44269;
  ap_ufixed<F1, 0> ln2(
      "0xb.17217f7d1cf79abc9e3b39803f2f6af40f343267p-4"); //~= 0.693147

  ap_ufixed<W1 + F1, I1 + 1> prod;
  big_mult(t_f, inv_ln2, prod);

  // Strip off the integer bits and the fractional bits.
  k = prod;
  ap_ufixed<F1, 0> prod_dec = prod;

  ap_ufixed<2 * (F1), 0> tmp_t_out;
  big_mult(prod_dec, ln2, tmp_t_out);
  t_out = tmp_t_out;

#ifdef DEBUG
  std::cout << "t: " << std::hex << t << " " << ts.data()
            << ", t_f: " << t_f.to_string(10) << " " << t_f.to_string(16)
            << std::endl;
  std::cout << "expv: " << ts.expv() << std::endl;
  std::cout << "range redux: t: " << std::scientific << t
            << ", ln2: " << ln2.to_string(10)
            << ", inv_ln2: " << inv_ln2.to_string(10) << ", k: " << std::dec
            << k << ", prod: " << prod.to_string(10)
            << ", prod_dec: " << prod_dec.to_string(10)
            << ", t_out: " << std::scientific << t_out << std::endl;
  ap_ufixed<W1, I1> r = (k * ln2 + t_out);
  std::cout << "t =~ k * ln2 + t_out: " << t << " =~ " << r.to_string(16) << " "
            << r.to_string(10) << "\n";
#endif
}

template <typename T>
void sinh_cosh_range_redux_cordic(T d, T &s_out, T &c_out) {
  fp_struct<T> ds(d);
  // Record the sign of the input and force it to be positive.
  bool sign = ds.sign;
  ds.sign = 0;

  if (ds.to_ieee() >
      fp_struct<T>(hyperbolic_traits<T>::H_MAX_INPUT).to_ieee()) {
    // input is large, return infinity
    ds.exp = fp_struct<T>::EXP_INFNAN;
    ds.sig = 0;
    ds.sign = sign;
    s_out = ds.to_ieee();
    ds.sign = 0;
    c_out = ds.to_ieee();
    return;
  } else if (ds.exp == fp_struct<T>::EXP_INFNAN) { // +inf, -inf, nan
    // if the input is infinity or nan, then the output corresponds
    ds.sign = sign;
    s_out = ds.to_ieee();
    ds.sign = 0;
    c_out = ds.to_ieee();
    return;
  } else if (ds.exp <
             (fp_struct<T>::EXP_BIAS - hyperbolic_traits<T>::H_DEC_MIN)) {
    // if input is small, return the input.
    s_out = d;
    c_out = (T)1.0;
    return;
  }

  ap_uint<hyperbolic_traits<T>::H_INT_MAX + 1> k;
  ap_ufixed<hyperbolic_traits<T>::N, 0> r;
  hyperb_range_redux(d, k, r);

  const int N = hyperbolic_traits<T>::N;

  ap_fixed<N, 2> tmp_x(1.0 / 8.28133692173427360395e-01);
  ap_fixed<N, 2> tmp_y(0);
  ap_fixed<N, 2> tmp_z(r);
  cordic_hyperb_v1<N, N, 0, 0, 0>(tmp_x, tmp_y, tmp_z);
  T x = tmp_x; // ~cosh(r);
  T y = tmp_y; // ~sinh(r);

#ifdef DEBUG
  {
    std::cout << "after calc: "
              << "cosh(t_out) = x: " << x << "(" << std::hex
              << fp_struct<T>(x).data() << "), "
              << "sinh(t_out) = y: " << y << "(" << std::hex
              << fp_struct<T>(y).data() << ")" << std::endl;
    T z_test = r;
    T x_test_d = cosh((double)z_test);
    T y_test_d = sinh((double)z_test);
    std::cout << "Reference: "
              << "x: " << x_test_d << "(" << std::hex
              << fp_struct<T>(x_test_d).data() << "), "
              << "y: " << y_test_d << "(" << std::hex
              << fp_struct<T>(y_test_d).data() << ")" << std::endl;
  }
#endif

  T two_p_plus_k, two_p_minus_k;
  // extra -1 in the exponent, since we want to compute
  // (exp(k*ln(2)) +- exp(-k*ln(2)))/2 = (2^k +- 2^-k)/2 = 2^(k-1) +- 2^(-k-1)
  unsigned int pos_exp = (int)(k + fp_struct<T>::EXP_BIAS - 1);
  unsigned int neg_exp = (k >= fp_struct<T>::EXP_BIAS)
                             ? 0
                             : (int)(-k + fp_struct<T>::EXP_BIAS - 1);
  fp_struct<T> two_p_plus_s((typename fp_struct<T>::inttype)0);
  fp_struct<T> two_p_minus_s((typename fp_struct<T>::inttype)0);
  two_p_plus_s.exp = pos_exp;
  two_p_minus_s.exp = neg_exp;
  two_p_plus_k = two_p_plus_s.to_ieee();
  two_p_minus_k = two_p_minus_s.to_ieee();

  T sinhkln2 = two_p_plus_k - two_p_minus_k;
  T coshkln2 = two_p_plus_k + two_p_minus_k;

  // based on the half angle identity for sinh/cosh, and t = k*ln2 + 2;
  T s_out_tmp = coshkln2 * y + sinhkln2 * x;
  T c_out_tmp = coshkln2 * x + sinhkln2 * y;

#ifdef DEBUG
  {
    std::cout << "k: " << std::dec << k << ", pos_exp: " << pos_exp
              << ", neg_exp: " << neg_exp << std::endl;
    std::cout << std::hex << "two_p_plus_k: " << two_p_plus_k << ", "
              << "two_p_minus_k: " << two_p_minus_k << std::dec << std::endl;
    std::cout << std::hex << "coshkln2: " << coshkln2 << "(" << std::hex
              << fp_struct<T>(coshkln2).data() << "), "
              << "sinhkln2: " << sinhkln2 << "(" << std::hex
              << fp_struct<T>(sinhkln2).data() << ")" << std::dec << std::endl;
    double ln2 = 0.69314718055994530941723212145818;
    double t = k * ln2;
    T x_test_d = cosh(t);
    T y_test_d = sinh(t);
    std::cout << "Reference: "
              << "coshkln2: " << x_test_d << "(" << std::hex
              << fp_struct<T>(x_test_d).data() << "), "
              << "sinhkln2: " << y_test_d << "(" << std::hex
              << fp_struct<T>(y_test_d).data() << ")" << std::endl;
    std::cout << std::hex << "s_out_tmp: " << s_out_tmp << ", "
              << "c_out_tmp: " << c_out_tmp << std::dec << std::endl;
  }
#endif

  fp_struct<T> s_fp(s_out_tmp);
  fp_struct<T> c_fp(c_out_tmp);
  s_fp.sign = sign;
  c_fp.sign = 0;

  s_out = s_fp.to_ieee();
  c_out = c_fp.to_ieee();
}

static half half_sinh(half t_in) {
  half s_out, c_out;
  sinh_cosh_range_redux_cordic(t_in, s_out, c_out);
  return s_out;
}

static half half_cosh(half t_in) {
  half s_out, c_out;
  sinh_cosh_range_redux_cordic(t_in, s_out, c_out);
  return c_out;
}

static float sinhf(float t_in) {
  float s_out, c_out;
  sinh_cosh_range_redux_cordic(t_in, s_out, c_out);
  return s_out;
}

static float coshf(float t_in) {
  float s_out, c_out;
  sinh_cosh_range_redux_cordic(t_in, s_out, c_out);
  return c_out;
}

static double sinh(double t_in) {
  double s_out, c_out;
  sinh_cosh_range_redux_cordic(t_in, s_out, c_out);
  return s_out;
}

static double cosh(double t_in) {
  double s_out, c_out;
  sinh_cosh_range_redux_cordic(t_in, s_out, c_out);
  return c_out;
}

} // namespace cordic

/* @}*/

#endif

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
