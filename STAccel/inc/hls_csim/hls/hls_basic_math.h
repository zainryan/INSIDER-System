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
 *     (c) Copyright 2008-2016 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/**
 * @file hls_basic_math.h
 * This file contains basic comparison-related functions.
 */

#ifndef X_HLS_MATH_H
#error hls_basic_math.h .h is not intended to be included directly.  Please include hls_math.h instead.
#endif

#ifndef X_HLS_BASIC_MATH_H
#define X_HLS_BASIC_MATH_H

template <typename T> int generic_signbit(T x) {
  fp_struct<T> s(x);
  return s.__signbit();
}
static int __signbit(float x) { return generic_signbit(x); }

static int __signbit(double x) { return generic_signbit(x); }

static int __signbit(half x) { return generic_signbit(x); }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type signbit(T x) {
  return x < 0 ? 1 : 0;
}

template <typename T>
typename enable_if<is_fptype<T>::value, int>::type signbit(T x) {
  return __signbit(x);
}

template <typename T> T generic_nan(const char *tagp) {
  fp_struct<T> nan;
  nan.sig = -1; // all ones
  nan.exp = -1; // all ones
  nan.sign = 0;
  return nan.to_ieee();
}
static double nan(const char *tagp) { return generic_nan<double>(tagp); }

static float nanf(const char *tagp) { return generic_nan<double>(tagp); }

static half half_nan(const char *tagp) { return generic_nan<half>(tagp); }

/**
 * Determines if given floating point number has a finite value.
 * i.e. normal, subnormal or zero, but not infinite or NAN
 *
 * @param x floating point vlaue.
 * @return nonzero value if arg has finite value, 0 otherwise.
 */
template <typename T> int generic_isfinite(T x) {
  fp_struct<T> fs = x;
  int ret = (fs.exp == fp_struct<T>::EXP_INFNAN) ? 0 : 1;
  return ret;
}
static int __isfinite(float x) { return generic_isfinite(x); }

static int __isfinite(double x) { return generic_isfinite(x); }

static int __isfinite(half x) { return generic_isfinite(x); }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type isfinite(T x) {
  return 1;
}

template <typename T>
typename enable_if<is_fptype<T>::value, int>::type isfinite(T x) {
  return __isfinite(x);
}

/**
 * Determines if given floating point number is positive or negative infinity.
 *
 * @param x floating point vlaue.
 * @return nonzero value if arg is infinite, 0 otherwise.
 */
template <typename T> int generic_isinf(T x) {
  fp_struct<T> fs = x;
  int ret = ((fs.exp == fp_struct<T>::EXP_INFNAN) && (fs.sig == 0x0)) ? 1 : 0;
  return ret;
}
static int __isinf(float x) { return generic_isinf(x); }

static int __isinf(double x) { return generic_isinf(x); }

static int __isinf(half x) { return generic_isinf(x); }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type isinf(T x) {
  return 0;
}

template <typename T>
typename enable_if<is_fptype<T>::value, int>::type isinf(T x) {
  return __isinf(x);
}

/**
 * Determines if given floating point number is not-a-number (NaN)
 *
 * @param x floating point vlaue.
 * @return nonzero value if arg is not-a-number (NaN), 0 otherwise.
 */
template <typename T> int generic_isnan(T x) {
  fp_struct<T> fs = x;
  int ret = ((fs.exp == fp_struct<T>::EXP_INFNAN) && (fs.sig != 0x0)) ? 1 : 0;
  return ret;
}
static int __isnan(float x) { return generic_isnan(x); }

static int __isnan(double x) { return generic_isnan(x); }

static int __isnan(half x) { return generic_isnan(x); }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type isnan(T x) {
  return 0;
}

template <typename T>
typename enable_if<is_fptype<T>::value, int>::type isnan(T x) {
  return __isnan(x);
}

/**
 * Determines if given floating point number is normal
 * i.e. is neither zero, subnormal, infinite or NaN.
 *
 * @param x floating point vlaue.
 * @return nonzero value if arg is normal, 0 otherwise.
 */

template <typename T> int generic_isnormal(T x) {
  fp_struct<T> fs = x;
  int ret = ((fs.exp != fp_struct<T>::EXP_INFNAN) && (fs.exp != 0x0)) ? 1 : 0;
  return ret;
}
static int __isnormal(float x) { return generic_isnormal(x); }

static int __isnormal(double x) { return generic_isnormal(x); }

static int __isnormal(half x) { return generic_isnormal(x); }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type isnormal(T x) {
  return x != 0 ? 1 : 0;
}

template <typename T>
typename enable_if<is_fptype<T>::value, int>::type isnormal(T x) {
  return __isnormal(x);
}

/**
 * Categorizes floating point value into the following categories:
 * zero, subnormal, normal, infinite, NaN, or implementation-defined category.
 *
 * @param x floating point vlaue.
 * @return FP_INFINITE, FP_NAN, FP_NORMAL, FP_SUBNORMAL, FP_ZERO
 */
#ifndef FP_NORMAL
#define FP_NORMAL 4
#endif

#ifndef FP_SUBNORMAL
#define FP_SUBNORMAL 3
#endif

#ifndef FP_ZERO
#define FP_ZERO 2
#endif

#ifndef FP_INFINITE
#define FP_INFINITE 1
#endif

#ifndef FP_NAN
#define FP_NAN 0
#endif

template <typename T> int generic_fpclassify(T x) {
  fp_struct<T> fs = x;
  int ret;
  if (fs.exp == 0x0) {
    ret = (fs.sig == 0x0) ? FP_ZERO : FP_SUBNORMAL;
  } else if (fs.exp == fp_struct<T>::EXP_INFNAN) {
    ret = (fs.sig == 0x0) ? FP_INFINITE : FP_NAN;
  } else {
    ret = FP_NORMAL;
  }
  return ret;
}
static int __fpclassifyf(float x) { return generic_fpclassify(x); }

static int __fpclassify(double x) { return generic_fpclassify(x); }

static int __fpclassify(half x) { return generic_fpclassify(x); }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type fpclassify(T x) {
  return x != 0 ? FP_NORMAL : FP_ZERO;
}

static int fpclassify(float x) { return __fpclassifyf(x); }

static int fpclassify(double x) { return __fpclassify(x); }

static int fpclassify(half x) { return __fpclassify(x); }

static double copysign(double x, double y) {
  fp_struct<double> xs(x), ys(y);
  xs.sign = ys.sign;
  return xs.to_ieee();
}

static float copysignf(float x, float y) {
  fp_struct<float> xs(x), ys(y);
  xs.sign = ys.sign;
  return xs.to_ieee();
}
static half half_copysign(half x, half y) {
  fp_struct<half> xs(x), ys(y);
  xs.sign = ys.sign;
  return xs.to_ieee();
}

#ifdef __cplusplus
static float copysign(float x, float y) { return copysignf(x, y); }
static half copysign(half x, half y) { return ::hls::half_copysign(x, y); }
#endif

template <typename T> T generic_fabs(T x) { return copysign(x, (T)0); }

template <typename T> T generic_fma(T x, T y, T z) { return x * y + z; }

// opencl relational function
template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type isequal(T x, T y) {
  return x == y;
}

static int isequal(double x, double y) { return HLS_FPO_EQUAL(x, y); }

static int isequal(float x, float y) { return HLS_FPO_EQUALF(x, y); }

static int isequal(half x, half y) { return x == y; }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type isgreater(T x, T y) {
  return x > y;
}

static int isgreater(double x, double y) { return HLS_FPO_GREATER(x, y); }

static int isgreater(float x, float y) { return HLS_FPO_GREATERF(x, y); }

static int isgreater(half x, half y) { return x > y; }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type isgreaterequal(T x,
                                                                        T y) {
  return x >= y;
}

static int isgreaterequal(double x, double y) {
  return HLS_FPO_GREATEREQUAL(x, y);
}

static int isgreaterequal(float x, float y) {
  return HLS_FPO_GREATEREQUALF(x, y);
}

static int isgreaterequal(half x, half y) { return x >= y; }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type isless(T x, T y) {
  return x < y;
}

static int isless(double x, double y) { return HLS_FPO_LESS(x, y); }

static int isless(float x, float y) { return HLS_FPO_LESSF(x, y); }

static int isless(half x, half y) { return x < y; }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type islessequal(T x, T y) {
  return x <= y;
}

static int islessequal(double x, double y) { return HLS_FPO_LESSEQUAL(x, y); }

static int islessequal(float x, float y) { return HLS_FPO_LESSEQUALF(x, y); }

static int islessequal(half x, half y) { return x <= y; }

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type islessgreater(T x,
                                                                       T y) {
  return (x < y) || (x > y);
}

static int islessgreater(double x, double y) {
  return HLS_FPO_LESS(x, y) || HLS_FPO_GREATER(x, y);
}

static int islessgreater(float x, float y) {
  return HLS_FPO_LESSF(x, y) || HLS_FPO_GREATERF(x, y);
}

static int islessgreater(half x, half y) { return (x < y) || (x > y); }

// opencl relational function
template <typename T>
typename enable_if<is_arithmetic<T>::value, int>::type isnotequal(T x, T y) {
  return x != y;
}

static int isnotequal(double x, double y) { return HLS_FPO_NOTEQUAL(x, y); }

static int isnotequal(float x, float y) { return HLS_FPO_NOTEQUALF(x, y); }

static int isnotequal(half x, half y) { return x != y; }

// opencl relational function
template <typename T>
typename enable_if<is_arithmetic<T>::value, int>::type isordered(T x, T y) {
  return (((!__isnan(x)) && (!__isnan(y))) ? 1
                                           : 0); // &&(in0==in0)&&(in1==in1));
}

template <typename T>
typename enable_if<is_arithmetic<T>::value, int>::type isunordered(T x, T y) {
  return ((__isnan(x) || __isnan(y)) ? 1 : 0);
}

/**
 * fmax returns the maximum of two arguments.
 * If both arguments are 0/ -0, fmax always returns the 2nd argument
 * If both argumetns are nan, fmax always returns the 1st argument but casted
 * to be of type QNaN meaning bit 23 is a 1.
 * If only one argument is nan, fmax always returns the other argument.
 */
template <typename T> T generic_fmax(T x, T y) {
  fp_struct<T> x_fp(x);
  fp_struct<T> y_fp(y);
  ap_uint<fp_struct<T>::EXP_BITS> max_exponent = -1;
  T res;
  if (x_fp.exp == 0 && x_fp.sig == 0 && y_fp.exp == 0 &&
      y_fp.sig == 0) { // 0, -0: always return 2nd arg
    res = y;
  } else if (__isnan(x)) { // 1st arg: nan
    if (__isnan(y)) { // 1st and 2nd arg: nan, cast to QNaN and choose 1st arg
      x_fp.sig[fp_struct<T>::SIG_BITS - 1] = 1;
      res = x_fp.to_ieee();
    } else { // 1st arg: nan, 2nd: ~nan
      res = y;
    }
  } else if (__isnan(y)) { // 2nd arg: nan, 1st arg: ~nan
    res = x;
  } else {
    // res = (x > y) ? x : y;
    bool ymaggreater = x_fp.to_int() < y_fp.to_int();
    if (y_fp.sign && x_fp.sign)
      ymaggreater = !ymaggreater;
    res = ymaggreater ? y : x;
  }
  return res;
}

/**
 * fmax returns the maximum of two arguments.
 * If both arguments are 0/ -0, fmax always returns the 2nd argument
 * If both argumetns are nan, fmax always returns the 1st argument but casted
 * to be of type QNaN meaning bit 23 is a 1.
 * If only one argument is nan, fmax always returns the other argument.
 */
template <typename T> T generic_fmin(T x, T y) {
  fp_struct<T> x_fp(x);
  fp_struct<T> y_fp(y);
  ap_uint<fp_struct<T>::EXP_BITS> max_exponent = -1;
  T res;
  if (x_fp.exp == 0 && x_fp.sig == 0 && y_fp.exp == 0 &&
      y_fp.sig == 0) { // 0, -0: always return 2nd arg
    res = y;
  } else if (__isnan(x)) { // 1st arg: nan
    if (__isnan(y)) { // 1st and 2nd arg: nan, cast to QNaN and choose 1st arg
      x_fp.sig[fp_struct<T>::SIG_BITS - 1] = 1;
      res = x_fp.to_ieee();
    } else { // 1st arg: nan, 2nd: ~nan
      res = y;
    }
  } else if (__isnan(y)) { // 2nd arg: nan, 1st arg: ~nan
    res = x;
  } else {
    // res = (x < y) ? x : y;
    bool ymaggreater = x_fp.to_int() < y_fp.to_int();
    if (y_fp.sign && x_fp.sign)
      ymaggreater = !ymaggreater;
    res = ymaggreater ? x : y;
  }
  return res;
}

template <typename T> T generic_fdim(T x, T y) {
  T res = 0;
  if (__isnan(x))
    res = x;
  else if (__isnan(y))
    res = y;
  else if (x > y)
    res = x - y;
  return res;
}

template <typename T> T generic_maxmag(T x, T y) {
  if (generic_fabs(y) > generic_fabs(x))
    return y;
  else
    return x;
}

template <typename T> T generic_minmag(T x, T y) {
  if (generic_fabs(y) < generic_fabs(x))
    return y;
  else
    return x;
}

// opencl relational function
template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type
any(typename enable_if<is_integraltype<T>::value, T>::type x) {
  return (x < 0);
}

template <typename T>
typename enable_if<is_integraltype<T>::value, int>::type
all(typename enable_if<is_integraltype<T>::value, T>::type x) {
  return (x < 0);
}

template <typename T>
typename enable_if<is_integraltype<T>::value, T>::type bitselect(T x, T y,
                                                                 T z) {
  T r = (z & y) | (~z & x);
  return r;
}

template <typename T>
typename enable_if<is_fptype<T>::value, T>::type bitselect(T x, T y, T z) {
  fp_struct<T> fp_x(x);
  fp_struct<T> fp_y(y);
  fp_struct<T> fp_z(z);
  ap_uint<Type_BitWidth<T>::Value> r;
  r = (fp_z.data() & fp_y.data()) | (~fp_z.data() & fp_x.data());
  fp_struct<T> fp_r(r);
  return fp_r.to_ieee();
}

// in opencl_1.x doc, type of z is i/ugentype
template <typename T>
typename enable_if<is_arithmetic<T>::value, T>::type select(T x, T y, bool z) {
  T r = z ? y : x;
  return r;
}

template <typename T> T generic_frexp(T x, int *exp) {
  // The maximum exponent possible for type T.
  ap_uint<fp_struct<T>::EXP_BITS> max_exponent = -1;
  fp_struct<T> xs = x;
  bool zero = (xs.exp == 0x0);
  bool nan = (xs.exp == max_exponent);
  *exp = (zero || nan) ? 0 : xs.expv() + 1;
  if (zero) {
    // zero or subnormal
    return (T)0.0;
  } else if (nan) {
    // nan or infinity, return input.
    return x;
  } else {
    xs.exp = fp_struct<T>::EXP_BIAS - 1;
    return xs.to_ieee();
  }
}

template <typename T> T generic_ldexp(T x, int exp) {
  // The maximum exponent possible for type T.
  ap_uint<fp_struct<T>::EXP_BITS> max_exponent = -1;
  fp_struct<T> xs = x;
  exp += xs.exp;
  bool ininf = (xs.exp == max_exponent);
  bool zero = (xs.exp == 0) || (exp <= 0);
  bool inf = ininf || (exp >= max_exponent);
  if (__isnan(x)) {
    return x;
  } else if (zero && !ininf) {
    return ::hls::copysign((T)0.0, x);
  } else if (inf) {
    // infinity, return input.
    xs.sig = 0x0;
    xs.exp = max_exponent;
  } else {
    xs.exp = exp;
  }
  return xs.to_ieee();
}

template <typename T> int generic_ilogb(T x) {
  fp_struct<T> xs(x);
  if (xs.exp == 0) {
    if (xs.sig == 0)
      return FP_ILOGB0;
    else
      return FP_ILOGBNAN;
    //        unsigned int wf = fp_struct<T>::SIG_BITS;
    //        unsigned int zeros;
    //        #pragma unroll
    //        for ( zeros = 0; zeros < wf; zeros++ )
    //            if ( xs.sig[wf-zeros-1] ) break;
    //        int ret = -127 - zeros;
    //        return ret;
  }
  if (xs.exp == fp_struct<T>::EXP_INFNAN) {
    if (xs.sig == 0)
      return INT_MAX;
    else
      return FP_ILOGBNAN;
  }
  return xs.expv();
}

#endif
