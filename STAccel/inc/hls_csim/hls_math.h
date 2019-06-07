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
 *     (c) Copyright 2008-2012 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/**
 * @file hls_math.h
 * This file contains math.h related functions such as sin, cos, sqrt, ln, etc.
 */

#ifndef X_HLS_MATH_H
#define X_HLS_MATH_H

#ifndef __cplusplus
#error C++ is required to include this header file
#endif

#include "ap_int.h"
#include "hls/utils/x_hls_defines.h"
#include "hls/utils/x_hls_traits.h"
#include "hls/utils/x_hls_utils.h"
#include "hls_fpo.h"
#include <assert.h>
#include <stdint.h>

namespace hls {

/**
 * The Xilinx math.h library implements standard math.h functions
 * optimized for FPGA implementation.
 *
 */

#include "hls/hls_basic_math.h"

static double fabs(double x) { return generic_fabs<double>(x); }
static float fabs(float x) { return generic_fabs<float>(x); }
static half fabs(half x) { return generic_fabs<half>(x); }
static float fabsf(float x) { return generic_fabs<float>(x); }
static half half_fabs(half x) { return generic_fabs<half>(x); }

// defined in hls/utils/x_hls_utils.h
static double abs(double x) { return generic_fabs<double>(x); }
static float abs(float x) { return generic_fabs<float>(x); }
static half abs(half x) { return generic_fabs<half>(x); }
static float absf(float x) { return generic_fabs<float>(x); }
static half half_abs(half x) { return generic_fabs<half>(x); }

static double fma(double x, double y, double z) {
  return generic_fma<double>(x, y, z);
}
static float fma(float x, float y, float z) {
  return generic_fma<float>(x, y, z);
}
static half fma(half x, half y, half z) { return generic_fma<half>(x, y, z); }
static float fmaf(float x, float y, float z) {
  return generic_fma<float>(x, y, z);
}
static half half_fma(half x, half y, half z) {
  return generic_fma<half>(x, y, z);
}

static double mad(double x, double y, double z) {
  return generic_fma<double>(x, y, z);
}
static float mad(float x, float y, float z) {
  return generic_fma<float>(x, y, z);
}
static half mad(half x, half y, half z) { return generic_fma<half>(x, y, z); }
static float madf(float x, float y, float z) {
  return generic_fma<float>(x, y, z);
}
static half half_mad(half x, half y, half z) {
  return generic_fma<half>(x, y, z);
}

static double frexp(double x, int *exp) {
  return generic_frexp<double>(x, exp);
}
static float frexp(float x, int *exp) { return generic_frexp<float>(x, exp); }
static half frexp(half x, int *exp) { return generic_frexp<half>(x, exp); }
static float frexpf(float x, int *exp) { return generic_frexp<float>(x, exp); }
static half half_frexp(half x, int *exp) { return generic_frexp<half>(x, exp); }

static double ldexp(double x, int exp) { return generic_ldexp<double>(x, exp); }
static float ldexp(float x, int exp) { return generic_ldexp<float>(x, exp); }
static half ldexp(half x, int exp) { return generic_ldexp<half>(x, exp); }
static float ldexpf(float x, int exp) { return generic_ldexp<float>(x, exp); }
static half half_ldexp(half x, int exp) { return generic_ldexp<half>(x, exp); }

static double scalbn(double x, int exp) {
  return generic_ldexp<double>(x, exp);
}
static float scalbn(float x, int exp) { return generic_ldexp<float>(x, exp); }
static half scalbn(half x, int exp) { return generic_ldexp<half>(x, exp); }
static float scalbnf(float x, int exp) { return generic_ldexp<float>(x, exp); }
static half half_scalbn(half x, int exp) { return generic_ldexp<half>(x, exp); }

static double scalbln(double x, long int exp) {
  return generic_ldexp<double>(x, exp);
}
static float scalbln(float x, long int exp) {
  return generic_ldexp<float>(x, exp);
}
static half scalbln(half x, long int exp) {
  return generic_ldexp<half>(x, exp);
}
static float scalblnf(float x, long int exp) {
  return generic_ldexp<float>(x, exp);
}
static half half_scalbln(half x, long int exp) {
  return generic_ldexp<half>(x, exp);
}

static int ilogb(double x) { return generic_ilogb<double>(x); }
static int ilogb(float x) { return generic_ilogb<float>(x); }
static int ilogb(half x) { return generic_ilogb<half>(x); }
static int ilogbf(float x) { return generic_ilogb<float>(x); }
static int half_ilogb(half x) { return generic_ilogb<half>(x); }

static double fmax(double x, double y) { return generic_fmax<double>(x, y); }
static float fmax(float x, float y) { return generic_fmax(x, y); }
static half fmax(half x, half y) { return generic_fmax<half>(x, y); }
static float fmaxf(float x, float y) { return generic_fmax<float>(x, y); }
static half half_fmax(half x, half y) { return generic_fmax<half>(x, y); }

static double fmin(double x, double y) { return generic_fmin<double>(x, y); }
static float fmin(float x, float y) { return generic_fmin(x, y); }
static half fmin(half x, half y) { return generic_fmin<half>(x, y); }
static float fminf(float x, float y) { return generic_fmin<float>(x, y); }
static half half_fmin(half x, half y) { return generic_fmin<half>(x, y); }

static double fdim(double x, double y) { return generic_fdim<double>(x, y); }
static float fdim(float x, float y) { return generic_fdim(x, y); }
static half fdim(half x, half y) { return generic_fdim<half>(x, y); }
static float fdimf(float x, float y) { return generic_fdim<float>(x, y); }
static half half_fdim(half x, half y) { return generic_fdim<half>(x, y); }

static double maxmag(double x, double y) {
  return generic_maxmag<double>(x, y);
}
static float maxmag(float x, float y) { return generic_maxmag(x, y); }
static half maxmag(half x, half y) { return generic_maxmag<half>(x, y); }
static float maxmagf(float x, float y) { return generic_maxmag<float>(x, y); }
static half half_maxmag(half x, half y) { return generic_maxmag<half>(x, y); }

static double minmag(double x, double y) {
  return generic_minmag<double>(x, y);
}
static float minmag(float x, float y) { return generic_minmag(x, y); }
static half minmag(half x, half y) { return generic_minmag<half>(x, y); }
static float minmagf(float x, float y) { return generic_minmag<float>(x, y); }
static half half_minmag(half x, half y) { return generic_minmag<half>(x, y); }

#include "hls/hls_log_.h"
#include "hls/hls_normalize.h"
#include "hls/hls_round.h"
//#include "hls/hls_log.h"
#include "hls/hls_fmod_rem_quo.h"
#include "hls/hls_nextafter.h"
#include "hls/hls_pow.h"

// Trig functions have 2 different possible implementations.
#include "hls/hls_cordic.h"
#include "hls/hls_hotbm.h"

// sin
static double sin(double t_in) { return hotbm::sin(t_in); }
static float sin(float t_in) { return hotbm::sinf(t_in); }
static float sinf(float t_in) { return hotbm::sinf(t_in); }
static half half_sin(half t_in) { return hotbm::half_sin(t_in); }
static half sin(half t_in) { return ::hls::half_sin(t_in); }

// cos
static double cos(double t_in) { return hotbm::cos(t_in); }
static float cos(float t_in) { return hotbm::cosf(t_in); }
static float cosf(float t_in) { return hotbm::cosf(t_in); }
static half half_cos(half t_in) { return hotbm::half_cos(t_in); }
static half cos(half t_in) { return ::hls::half_cos(t_in); }

// fixed-point version
template <int W, int I> ap_fixed<W, 2> cosf(ap_fixed<W, I> t_in) {
  return hotbm::cosf(t_in);
}
template <int W, int I> ap_fixed<W, 2> sinf(ap_fixed<W, I> t_in) {
  return hotbm::sinf(t_in);
}
template <int W, int I> ap_fixed<W, 2> cos(ap_fixed<W, I> t_in) {
  return hotbm::cosf(t_in);
}
template <int W, int I> ap_fixed<W, 2> sin(ap_fixed<W, I> t_in) {
  return hotbm::sinf(t_in);
}

// opencl built-in functions
// sinpi
static double sinpi(double t_in) { return hotbm::sinpi(t_in); }
static float sinpif(float t_in) { return hotbm::sinpif(t_in); }
static half half_sinpi(half t_in) { return hotbm::half_sinpi(t_in); }

// cospi
static double cospi(double t_in) { return hotbm::cospi(t_in); }
static float cospif(float t_in) { return hotbm::cospif(t_in); }
static half half_cospi(half t_in) { return hotbm::half_cospi(t_in); }

// fixed-point version
template <int W, int I> ap_fixed<W, 2> cospif(ap_fixed<W, I> t_in) {
  return hotbm::cospif(t_in);
}
template <int W, int I> ap_fixed<W, 2> sinpif(ap_fixed<W, I> t_in) {
  return hotbm::sinpif(t_in);
}
template <int W, int I> ap_fixed<W, 2> cospi(ap_fixed<W, I> t_in) {
  return hotbm::cospif(t_in);
}
template <int W, int I> ap_fixed<W, 2> sinpi(ap_fixed<W, I> t_in) {
  return hotbm::sinpif(t_in);
}

// sincos
static void sincos(double x, double *sin, double *cos) {
  hotbm::sincos(x, sin, cos);
}
static void sincosf(float x, float *sin, float *cos) {
  hotbm::sincosf(x, sin, cos);
}
static void half_sincos(half x, half *sin, half *cos) {
  hotbm::half_sincos(x, sin, cos);
}

static double atan(double t_in) { return cordic::atan(t_in); }

static float atanf(float t_in) { return cordic::atanf(t_in); }

static half half_atan(half t_in) { return cordic::half_atan(t_in); }

static double atan2(double y_in, double x_in) {
  return cordic::atan2(y_in, x_in);
}

static float atan2f(float y_in, float x_in) {
  return cordic::atan2f(y_in, x_in);
}

static half half_atan2(half y_in, half x_in) {
  return cordic::half_atan2(y_in, x_in);
}

static double sinh(double t_in) { return cordic::sinh(t_in); }
static float sinhf(float t_in) { return cordic::sinhf(t_in); }
static half half_sinh(half t_in) { return cordic::half_sinh(t_in); }

static double cosh(double t_in) { return cordic::cosh(t_in); }
static float coshf(float t_in) { return cordic::coshf(t_in); }
static half half_cosh(half t_in) { return cordic::half_cosh(t_in); }

} // namespace hls

#include "hls_fpo.h"

#ifdef AESL_SYN
#ifdef __cplusplus
extern "C" {
#endif
double _ssdm_op_DRecip(double) __attribute__((nothrow));
float _ssdm_op_FRecip(float) __attribute__((nothrow));
double _ssdm_op_DRSqrt(double) __attribute__((nothrow));
float _ssdm_op_FRSqrt(float) __attribute__((nothrow));
#ifdef __cplusplus
}
#endif
#endif

namespace hls {

static float tanf(float t_in) {
  float s_out, c_out;
  fp_struct<float> fs = t_in;
  if (fs.exp <= 0x1) {
    // subnormal or zero or small number
    return t_in;
  } else {
    ::hls::sincosf(t_in, &s_out, &c_out);
    return HLS_FPO_DIVF(s_out, c_out);
  }
}

static double tan(double t_in) {
  double s_out, c_out;
  fp_struct<double> fs = t_in;
  if (fs.exp <= 0x1) {
    // subnormal or zero or small number
    return t_in;
  } else {
    ::hls::sincos(t_in, &s_out, &c_out);
    return HLS_FPO_DIV(s_out, c_out);
  }
}
static half half_tan(half t_in) {
  half s_out, c_out;
  fp_struct<half> fs = t_in;
  if (fs.exp <= 0x1) {
    // subnormal or zero or small number
    return t_in;
  } else {
    ::hls::half_sincos(t_in, &s_out, &c_out);
    return s_out / c_out;
  }
}
static half tan(half t_in) { return ::hls::half_tan(t_in); }

static float logf(float x) {
  return ::HLS_FPO_LOGF(x);
  // return hls::log_reduce::logf(x);
}
static float log2f(float x) {
  // return
  // HLS_FPO_MULF(logf(x), 1.442695040888963407359924681001892137426645954153f);
  return hls::log_reduce::log2f(x);
}
static float log10f(float x) {
  return HLS_FPO_MULF(logf(x),
                      0.434294481903251827651128918916605082294397005f);
  // return hls::log_reduce::log10f(x);
}
static float logbf(float x) { return hls::log_reduce::logbf(x); }

static double log(double x) {
  return ::HLS_FPO_LOG(x);
  // return hls::log_reduce::log(x);
}
static float log(float x) {
  return ::HLS_FPO_LOGF(x);
  // return hls::log_reduce::log(x);
}
static half log(half x) { return hls::log_reduce::log(x); }
static double log2(double x) {
  // return
  // HLS_FPO_MUL(log(x), 1.442695040888963407359924681001892137426645954153);
  return hls::log_reduce::log2(x);
}
static float log2(float x) { return hls::log_reduce::log2(x); }
static half log2(half x) { return hls::log_reduce::log2(x); }
static double log10(double x) {
  return HLS_FPO_MUL(log(x), 0.434294481903251827651128918916605082294397005);
  // return hls::log_reduce::log10(x);
}
static float log10(float x) {
  return HLS_FPO_MULF(logf(x),
                      0.434294481903251827651128918916605082294397005f);
  // return hls::log_reduce::log10(x);
}
static half log10(half x) { return hls::log_reduce::log10(x); }
static double logb(double x) { return hls::log_reduce::logb(x); }
static float logb(float x) { return hls::log_reduce::logb(x); }
static half logb(half x) { return hls::log_reduce::logb(x); }

static half half_log(half x) { return hls::log_reduce::half_log(x); }
static half half_log10(half x) { return hls::log_reduce::half_log10(x); }
static half half_log2(half x) { return hls::log_reduce::half_log2(x); }
static half half_logb(half x) { return hls::log_reduce::half_logb(x); }

static double log1p(double x) { return hls::log_reduce::log1p(x); }
static float log1p(float x) { return hls::log_reduce::log1p(x); }
static half log1p(half x) { return hls::log_reduce::log1p(x); }
static half half_log1p(half x) { return hls::log_reduce::log1p(x); }
static float log1pf(float x) { return hls::log_reduce::log1p(x); }

static half pow(half x, half y) { return hls::pow_reduce::pow(x, y); }
static float pow(float x, float y) { return hls::pow_reduce::pow(x, y); }
static double pow(double x, double y) { return hls::pow_reduce::pow(x, y); }
static float powf(float x, float y) { return hls::pow_reduce::pow(x, y); }
static half half_pow(half x, half y) { return hls::pow_reduce::pow(x, y); }

static half powr(half x, half y) { return hls::pow_reduce::powr(x, y); }
static float powr(float x, float y) { return hls::pow_reduce::powr(x, y); }
static double powr(double x, double y) { return hls::pow_reduce::powr(x, y); }
static float powrf(float x, float y) { return hls::pow_reduce::powr(x, y); }
static half half_powr(half x, half y) { return hls::pow_reduce::powr(x, y); }

static half pown(half x, int y) { return hls::pow_reduce::pown(x, y); }
static float pown(float x, int y) { return hls::pow_reduce::pown(x, y); }
static double pown(double x, int y) { return hls::pow_reduce::pown(x, y); }
static float pownf(float x, int y) { return hls::pow_reduce::pown(x, y); }
static half half_pown(half x, int y) { return hls::pow_reduce::pown(x, y); }

static half rootn(half x, int y) { return hls::pow_reduce::rootn(x, y); }
static float rootn(float x, int y) { return hls::pow_reduce::rootn(x, y); }
static double rootn(double x, int y) { return hls::pow_reduce::rootn(x, y); }
static float rootnf(float x, int y) { return hls::pow_reduce::rootn(x, y); }
static half half_rootn(half x, int y) { return hls::pow_reduce::rootn(x, y); }

#include "hls/hls_rsr.h"
#include "hls/hls_sqrt.h"
static double sqrt(double x) { return ::HLS_FPO_SQRT(x); }
static float sqrtf(float x) { return ::HLS_FPO_SQRTF(x); }
static half half_sqrt(half x) { return hls::hls_rsr::half_sqrt(x); }

static half half_recip(half x) { return hls::hls_rsr::half_recip(x); }
static half half_rsqrt(half x) { return hls::hls_rsr::half_rsqrt(x); }

static half cbrt(half x) { return hls::cbrt_generic(x); }
static float cbrt(float x) { return hls::cbrt_generic(x); }
static double cbrt(double x) { return hls::cbrt_generic(x); }
static float cbrtf(float x) { return hls::cbrt_generic(x); }
static half half_cbrt(half x) { return hls::cbrt_generic(x); }

static half hypot(half x, half y) { return hls::half_hypot(x, y); }
static float hypot(float x, float y) { return hls::hypotf(x, y); }

#ifdef AESL_SYN
static double recip(double x) { return _ssdm_op_DRecip(x); }

static float recipf(float x) { return _ssdm_op_FRecip(x); }

static double rsqrt(double x) { return _ssdm_op_DRSqrt(x); }

static float rsqrtf(float x) { return _ssdm_op_FRSqrt(x); }

#else
static double recip(double x) { return HLS_FPO_RECIP(x); }

static float recipf(float x) { return HLS_FPO_RECIPF(x); }

static double rsqrt(double x) { return HLS_FPO_RSQRT(x); }

static float rsqrtf(float x) { return HLS_FPO_RSQRTF(x); }
#endif

#include "hls/hls_asin_acos.h"
static double asin(double x) { return asin_acos_approx::asin(x); }
static double acos(double x) { return asin_acos_approx::acos(x); }
static float asin(float x) { return asin_acos_approx::asin(x); }
static float acos(float x) { return asin_acos_approx::acos(x); }
static half asin(half x) { return asin_acos_approx::asin(x); }
static half acos(half x) { return asin_acos_approx::acos(x); }
static half half_asin(half x) { return asin_acos_approx::asin(x); }
static half half_acos(half x) { return asin_acos_approx::acos(x); }
static float asinf(float x) { return asin_acos_approx::asinf(x); }
static float acosf(float x) { return asin_acos_approx::acosf(x); }

#include "hls/hls_exp_.h"
static double exp(double x) {
  return ::HLS_FPO_EXP(x);
  // return hls::exp_reduce_::exp(x);
}
static float exp(float x) {
  return ::HLS_FPO_EXPF(x);
  // return hls::exp_reduce_::exp(x);
}
static half exp(half x) { return hls::exp_reduce_::exp(x); }
static ap_fixed<16, 8> exp(ap_fixed<16, 8> x) {
  return hls::exp_reduce_::exp(x);
}
static ap_fixed<8, 4> exp(ap_fixed<8, 4> x) { return hls::exp_reduce_::exp(x); }
static double exp2(double x) {
  // return ::HLS_FPO_EXP(HLS_FPO_MUL(x,
  // 0.69314718055994530941723212145817656807550013436024));
  return hls::exp_reduce_::exp2(x);
}
static float exp2(float x) { return hls::exp_reduce_::exp2(x); }
static half exp2(half x) { return hls::exp_reduce_::exp2(x); }
static double exp10(double x) {
  // return
  // ::HLS_FPO_EXP(HLS_FPO_MUL(x, 2.30258509299404568401799145468436420760110148862875));
  return hls::exp_reduce_::exp10(x);
}
static float exp10(float x) { return hls::exp_reduce_::exp10(x); }
static half exp10(half x) { return hls::exp_reduce_::exp10(x); }

static float expf(float x) {
  // return ::HLS_FPO_EXPF(x);
  return hls::exp_reduce_::exp(x);
}
static float exp2f(float x) {
  // return ::HLS_FPO_EXPF(HLS_FPO_MULF(x,
  // 0.69314718055994530941723212145817656807550013436024f));
  return hls::exp_reduce_::exp2(x);
}
static float exp10f(float x) {
  // return
  // ::HLS_FPO_EXPF(HLS_FPO_MULF(x, 2.30258509299404568401799145468436420760110148862875f));
  return hls::exp_reduce_::exp10(x);
}
//#include "hls/hls_exp.h"
static half half_exp(half x) { return hls::exp_reduce_::half_exp(x); }
static half half_exp2(half x) { return hls::exp_reduce_::half_exp2(x); }
static half half_exp10(half x) { return hls::exp_reduce_::half_exp10(x); }

static double expm1(double x) { return hls::exp_reduce_::expm1(x); }
static float expm1(float x) { return hls::exp_reduce_::expm1(x); }
static half expm1(half x) { return hls::exp_reduce_::expm1(x); }
static float expm1f(float x) { return hls::exp_reduce_::expm1(x); }
static half half_expm1(half x) { return hls::exp_reduce_::expm1(x); }

#include "hls/hls_erf.h"
static double erf(double x) { return erf_approx::erf(x); }
static float erf(float x) { return erf_approx::erf(x); }
static half erf(half x) { return erf_approx::erf(x); }
static float erff(float x) { return erf_approx::erff(x); }
static double erfc(double x) { return erf_approx::erfc(x); }
static float erfc(float x) { return erf_approx::erfc(x); }
static half erfc(half x) { return erf_approx::erfc(x); }
static float erfcf(float x) { return erf_approx::erfcf(x); }

#include "hls/hls_gamma.h"
static float lgamma_rf(float x, int *signgamp) {
  return hls::lgamma_r(x, signgamp);
}
static half half_lgamma_r(half x, int *signgamp) {
  return hls::lgamma_r(x, signgamp);
}
static double lgamma(double x) {
  int *signgamp = 0;
  return hls::lgamma_r(x, signgamp);
}
static float lgammaf(float x) {
  int *signgamp = 0;
  return hls::lgamma_r(x, signgamp);
}
static half half_lgamma(half x) {
  int *signgamp = 0;
  return hls::lgamma_r(x, signgamp);
}
static float tgammaf(float x) { return hls::tgamma(x); }
static half half_tgamma(half x) { return hls::tgamma(x); }

// Do nothing with the result of the filter.
template <typename T> struct hls_filter_traits {
  typedef T T_data;
  typedef T T_accum;
  static T_data shift(T_accum in) { return in; }
};

struct hls_filter_traits_Q15 {
  typedef short T_data;
  typedef int T_accum;
  static T_data shift(T_accum in) { return (T_data)(in >> 15); }
};

/** Compute the dot product of x and y, returning the result.
 * @param x the first vector, with length n
 * @param y the second vector, with length n
 * @param n the size of the vectors.
 * @param T the type of the data
 * @param T_accum the type used for the accumulation
 */
template <typename T_accum, typename T> T_accum dot(T *x, T *y, int n) {
  // This volatile prevents the first tap from being constant-optimized.
  volatile T_accum r = 0;
  T_accum result = r;
dot_foreach_tap:
  for (int i = 0; i < n; i++) {
    result += x[i] * y[i];
  }
  return result;
}

/**
 * Block FIR filter, with data type T.  This function supports traits
 * which can be used to control the type used to perform the filter
 * accumultation.
 *
 * @param in the input data, with size incout.
 * @param state the filter state (size = numtaps), with the newest
 * sample in state[0].
 * @param out the filtered data, with size incout.
 * @param taps the filter taps (size = numtaps), stored in 'normal' order.
 * in and out are size incount.
 * @param incount
 * @param numtaps
 */
template <typename T, typename TRAITS>
void fir_blocksrl(T *in, T *taps, T *out, T *state, int incount, int numtaps) {
#pragma HLS inline self
fir_blocksrl_foreach_data:
  for (int n = 0; n < incount; n++) {
#pragma HLS expression_balance off
    for (int i = numtaps - 1; i > 0; i--) {
      state[i] = state[i - 1];
    }
    state[0] = in[n];
    out[n] = TRAITS::shift(dot<typename TRAITS::T_accum>(state, taps, numtaps));
  }
}

/**
 * Block FIR filter, with data type T.  This function should be used
 * when the type used for accumulation is the same as the type of the
 * output (e.g. 'float').
 *
 * @param in the input data, with size incout.
 * @param state the filter state (size = numtaps), with the newest
 * sample in state[0].
 * @param out the filtered data, with size incout.
 * @param taps the filter taps (size = numtaps), stored in 'normal' order.
 * in and out are size incount.
 * @param incount
 * @param numtaps
 */
template <typename T>
void fir_blocksrl(T *in, T *taps, T *out, T *state, int incount, int numtaps) {
  fir_blocksrl<T, hls_filter_traits<T>>(in, taps, out, state, incount, numtaps);
}

/**
 * Block FIR filter, with Q15 datatypes modeled as 'short'.  The
 * result always includes the required right shift by 15.
 *
 * @param in the input data, with size incout.
 * @param state the filter state (size = numtaps), with the newest
 * sample in state[0].
 * @param out the filtered data, with size incout.
 * @param taps the filter taps (size = numtaps), stored in 'normal' order.
 * in and out are size incount.
 * @param incount
 * @param numtaps
 */
static void fir_blocksrl_Q15(short *in, short *taps, short *out, short *state,
                             int incount, int numtaps) {
  fir_blocksrl<short, hls_filter_traits_Q15>(in, taps, out, state, incount,
                                             numtaps);
}

} // namespace hls

namespace hls {

#include "hls/hls_atanh.h"
#include "hls/hls_tanh.h"

static double tanh(double t_in) { return explog_based::tanh(t_in); }

static float tanh(float t_in) { return explog_based::tanh(t_in); }

static half tanh(half t_in) { return explog_based::tanh(t_in); }

static double atanh(double t_in) { return explog_based::atanh(t_in); }

static float atanh(float t_in) { return explog_based::atanh(t_in); }

static half atanh(half t_in) { return explog_based::atanh(t_in); }

#include "hls/hls_acosh.h"
#include "hls/hls_asinh.h"

static double acosh(double t_in) { return explog_based::acosh(t_in); }

static float acosh(float t_in) { return explog_based::acosh(t_in); }

static half acosh(half t_in) { return explog_based::acosh(t_in); }

static double asinh(double t_in) { return explog_based::asinh(t_in); }

static float asinh(float t_in) { return explog_based::asinh(t_in); }

static half asinh(half t_in) { return explog_based::asinh(t_in); }

} // namespace hls

#endif

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
