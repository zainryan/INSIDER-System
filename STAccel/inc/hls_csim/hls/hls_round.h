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
 * @file hls_round.h
 * This file contains rounding related functions such as round, trunc, floor,
 * ceil.
 */

template <typename T>
void init_mask_table(
    typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS]) {
  // Note that this is a little wierd since we are shifted by BIAS, so
  // mask_table[i] = mask(exp-BIAS+1);
  const typename fp_struct<T>::inttype t = (1LL << fp_struct<T>::SIG_BITS) - 1;
  for (unsigned int i = 0; i < fp_struct<T>::BITS - 2; i++) {
    mask_table[i] = t >> (i + 1);
  }
  mask_table[fp_struct<T>::BITS - 2] = t; // Special case for round()
  mask_table[fp_struct<T>::BITS - 1] =
      t; // mask(fp_struct<double>::EXP_BIAS), stuffed into index N-1.
}

template <typename T>
void init_one_half_table(
    typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS]) {
  // Note that this is a little wierd since we are shifted by BIAS, so
  // mask_table[i] = mask(exp-BIAS+1);
  const typename fp_struct<T>::inttype t = 1LL << (fp_struct<T>::SIG_BITS - 1);
  for (unsigned int i = 0; i < fp_struct<T>::BITS - 2; i++) {
    mask_table[i] = t >> (i + 1);
  }
  mask_table[fp_struct<T>::BITS - 2] = t << 1; // Special case for round()
  mask_table[fp_struct<T>::BITS - 1] =
      t; // one_half(fp_struct<double>::EXP_BIAS), stuffed into index N-1.
}

template <typename T>
void init_one_half_minus_one_table(
    typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS]) {
  // Note that this is a little wierd since we are shifted by BIAS, so
  // mask_table[i] = mask(exp-BIAS+1);
  const typename fp_struct<T>::inttype t =
      (1LL << (fp_struct<T>::SIG_BITS - 1));
  for (unsigned int i = 0; i < fp_struct<T>::SIG_BITS - 1; i++) {
    mask_table[i] = (t >> (i + 1)) - 1;
  }
  for (unsigned int i = fp_struct<T>::SIG_BITS - 1; i < fp_struct<T>::BITS - 2;
       i++) {
    mask_table[i] = 0;
  }
  mask_table[fp_struct<T>::BITS - 2] = (t << 1) - 1; // Special case for round()
  mask_table[fp_struct<T>::BITS - 1] =
      t - 1; // one_half(fp_struct<double>::EXP_BIAS), stuffed into index N-1.
}

template <typename T> T generic_floor(T x) {
  typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS];
  init_mask_table<T>(mask_table);
  fp_struct<T> xs(x);
  if (xs.exp < fp_struct<T>::EXP_BIAS) {
    if (xs.__signbit() &&
        !(xs.sig == 0 && xs.exp == 0)) { // negative and not zero
      return T(-1.0);
    } else {
      return ::hls::copysign((T)0.0, x);
    }
  } else if ((xs.exp > fp_struct<T>::EXP_BIAS + fp_struct<T>::SIG_BITS)) {
    // In this special case, we know there are no fractional bits.
    return x;
  } else {
    typename fp_struct<T>::inttype mask;
    // Find the mask.  These are 1's where there are fractional bits.
    // Note that the other bits should be zero, because of the special cases
    // above.
    ap_uint<UnsignedBitWidth<fp_struct<T>::BITS>::Value> index =
        xs.exp(UnsignedBitWidth<fp_struct<T>::BITS>::Value - 1, 0);
    mask = mask_table[index];

    // Deal with negative numbers
    if (xs.__signbit()) {
      xs = typename fp_struct<T>::data_type(xs.data() + mask);
    }

    xs.sig = xs.sig & (~mask);
    return xs.to_ieee();
  }
}

static double floor(double x) { return generic_floor<double>(x); }

static float floorf(float x) { return generic_floor<float>(x); }

static half half_floor(half x) { return generic_floor<half>(x); }

#ifdef __cplusplus
static float floor(float x) { return floorf(x); }
static half floor(half x) { return generic_floor<half>(x); }
#endif

template <typename T> T generic_ceil(T x) {
  typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS];
  init_mask_table<T>(mask_table);
  fp_struct<T> xs(x);

  if (xs.exp < fp_struct<T>::EXP_BIAS) {
    if (!xs.__signbit() &&
        !(xs.sig == 0 && xs.exp == 0)) { // positive and not zero
      return T(1.0);
    } else {
      return ::hls::copysign((T)0.0, x);
    }
  } else if (xs.exp > (fp_struct<T>::EXP_BIAS + fp_struct<T>::SIG_BITS)) {
    // In this special case, we know there are no fractional bits.
    return x;
  } else {
    typename fp_struct<T>::inttype mask;
    // Find the mask.  These are 1's where there are fractional bits.
    // Note that the other bits should be zero, because of the special cases
    // above.
    ap_uint<UnsignedBitWidth<fp_struct<T>::BITS>::Value> index =
        xs.exp(UnsignedBitWidth<fp_struct<T>::BITS>::Value - 1, 0);
    mask = mask_table[index];

    // Deal with positive numbers
    if (!xs.__signbit()) {
      xs = typename fp_struct<T>::data_type(xs.data() + mask);
    }

    xs.sig = xs.sig & (~mask);
    return xs.to_ieee();
  }
}

static double ceil(double x) { return generic_ceil<double>(x); }

static float ceilf(float x) { return generic_ceil<float>(x); }

static half half_ceil(half x) { return generic_ceil<half>(x); }
#ifdef __cplusplus
static float ceil(float x) { return ceilf(x); }
static half ceil(half x) { return generic_ceil<half>(x); }
#endif

template <typename T> T generic_trunc(T x) {
  typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS];
  init_mask_table<T>(mask_table);
  fp_struct<T> xs(x);

  if (xs.exp < fp_struct<T>::EXP_BIAS) {
    return ::hls::copysign((T)0.0, x);
  } else if ((xs.exp > fp_struct<T>::EXP_BIAS + fp_struct<T>::SIG_BITS)) {
    // In this special case, we know there are no fractional bits.
    return x;
  } else {
    typename fp_struct<T>::inttype mask;
    // Find the mask.  These are 1's where there are fractional bits.
    // Note that the other bits should be zero, because of the special cases
    // above.
    ap_uint<UnsignedBitWidth<fp_struct<T>::BITS>::Value> index =
        xs.exp(UnsignedBitWidth<fp_struct<T>::BITS>::Value - 1, 0);
    mask = mask_table[index];
    xs.sig = xs.sig & (~mask);
    return xs.to_ieee();
  }
}

static double trunc(double x) { return generic_trunc<double>(x); }

static float truncf(float x) { return generic_trunc<float>(x); }

static half half_trunc(half x) { return generic_trunc<half>(x); }

#ifdef __cplusplus
static float trunc(float x) { return truncf(x); }
static half trunc(half x) { return generic_trunc<half>(x); }
#endif

// For round, we deal with three cases, the first is for small numbers <0.5,
// which get rounded to 0.0. The second case if for large numbers where nothing
// happens.  The third deals with the 'normal' case where we add one half and
// then drop the fractional bits.  There are two things to observe: 1) When we
// add one half, we sometimes roll over and increment the exponent, by one.  The
// exponent integer bits of the resulting number is correct, but the fractional
// bits aren't.  This is OK, since we drop the fractional bits. 2) The 'missing
// case' of [0.5, 1.0) is handled in the third case, where adding one to the
// exponent (making it zero) and then dropping all of the fractional bits,
// resulting in 1.0f. As a result there is an extra 'dummy' entry in the tables
// for round, that is not used for the other operators.
template <typename T> T generic_round(T x) {
  typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS];
  init_mask_table<T>(mask_table);
  typename fp_struct<T>::inttype one_half_table[fp_struct<T>::BITS];
  init_one_half_table<T>(one_half_table);
  fp_struct<T> xs(x);

  if (xs.exp < fp_struct<T>::EXP_BIAS - 1) {
    // Then we have a number less than 0.5
    return ::hls::copysign((T)0.0, x);
  } else if ((xs.exp > fp_struct<T>::EXP_BIAS + fp_struct<T>::SIG_BITS)) {
    // In this special case, we know there are no fractional bits.
    return x;
  } else {
    typename fp_struct<T>::inttype mask;
    typename fp_struct<T>::inttype one_half;
    // Find the mask.  These are 1's where there are fractional bits.
    // Note that the other bits should be zero, because of the special cases
    // above.
    ap_uint<UnsignedBitWidth<fp_struct<T>::BITS>::Value> index =
        xs.exp(UnsignedBitWidth<fp_struct<T>::BITS>::Value - 1, 0);
    mask = mask_table[index];
    one_half = one_half_table[index];

    xs = typename fp_struct<T>::data_type(xs.data() + one_half);

    xs.sig = xs.sig & (~mask);
    return xs.to_ieee();
  }
}

static double round(double x) { return generic_round<double>(x); }

static float roundf(float x) { return generic_round<float>(x); }

static half half_round(half x) { return generic_round<half>(x); }

#ifdef __cplusplus
static float round(float x) { return roundf(x); }
static half round(half x) { return generic_round<half>(x); }
#endif

// For round, we deal with three cases, the first is for small numbers <=0.5,
// which get rounded to 0.0. The second case if for large numbers where nothing
// happens.  The third deals with the 'normal' case where we add one half and
// then drop the fractional bits.  There are two things to observe: 1) When we
// add one half, we sometimes roll over and increment the exponent, by one.  The
// exponent integer bits of the resulting number is correct, but the fractional
// bits aren't.  This is OK, since we drop the fractional bits. 2) The 'missing
// case' of (0.5, 1.0) is handled in the third case, where adding one to the
// exponent (making it zero) and then dropping all of the fractional bits,
// resulting in 1.0f. As a result there is an extra 'dummy' entry in the tables
// for round, that is not used for the other operators. rint differs from
// round() in that we have to round ties to nearest even, rather than always
// rounding to +-infinity.  This is done by picking off the bit corresponding to
// the ones place and adding it in.
template <typename T> T generic_rint(T x) {
  typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS];
  init_mask_table<T>(mask_table);
  typename fp_struct<T>::inttype one_half_minus_one_table[fp_struct<T>::BITS];
  init_one_half_minus_one_table<T>(one_half_minus_one_table);
  fp_struct<T> xs(x);

  if (xs.exp < fp_struct<T>::EXP_BIAS - 1 ||
      (xs.exp == (fp_struct<T>::EXP_BIAS - 1) && xs.sig == 0)) {
    // Then we have a number less than or equal to 0.5
    return ::hls::copysign((T)0.0, x);
  } else if ((xs.exp >= fp_struct<T>::EXP_BIAS + fp_struct<T>::SIG_BITS)) {
    // In this special case, we know there are no fractional bits.
    return x;
  } else {
    typename fp_struct<T>::inttype mask;
    typename fp_struct<T>::inttype one_half;
    // Find the mask.  These are 1's where there are fractional bits.
    // Note that the other bits should be zero, because of the special cases
    // above.
    ap_uint<UnsignedBitWidth<fp_struct<T>::BITS>::Value> index =
        xs.exp(UnsignedBitWidth<fp_struct<T>::BITS>::Value - 1, 0);
    mask = mask_table[index];
    one_half = one_half_minus_one_table[index];

    int index_table[fp_struct<T>::BITS];
    for (unsigned int i = 0; i < fp_struct<T>::BITS - 2; i++) {
      index_table[i] = int(fp_struct<T>::SIG_BITS - 1 - i);
    }
    index_table[fp_struct<T>::BITS - 2] =
        int(fp_struct<T>::SIG_BITS) -
        2; //(t << 1)-1; // Special case for round()
    // This index is actually the LSB of the exponent.  There are
    // only two numbers where this matters: one is 1.5, which has
    // an exponent equal to EXP_BIAS.  Since this is always odd,
    // we happen to pick the correct value which is the implicit 1
    // in the mantissa.
    index_table[fp_struct<T>::BITS - 1] = int(fp_struct<T>::SIG_BITS);

    if (xs.data()[index_table[index]])
      one_half += 1; // Only round up if the next higher bit is odd.

#ifdef DEBUG
    std::cout << "index=" << std::dec << index << "\n";
    std::cout << "i =" << std::dec << index_table[index] << "\n";
    std::cout << "mask = " << std::hex << mask << "\n";
    std::cout << " xs.sig[index] = " << xs.data()[index_table[index]] << "\n";
    std::cout << "one_half = " << std::hex << one_half << "\n";
#endif

    xs = typename fp_struct<T>::data_type(xs.data() + one_half);

    xs.sig = xs.sig & (~mask);
    return xs.to_ieee();
  }
}

static double rint(double x) { return generic_rint<double>(x); }

static float rintf(float x) { return generic_rint<float>(x); }

static half half_rint(half x) { return generic_rint<half>(x); }

static double nearbyint(double x) { return generic_rint<double>(x); }

static float nearbyintf(float x) { return generic_rint<float>(x); }

static half half_nearbyint(half x) { return generic_rint<half>(x); }

template <typename T, ap_q_mode ROUNDING, typename FP>
T cast_IEEE754(
    FP x, bool detect_overflow = false,
    typename enable_if<std::numeric_limits<T>::is_signed, bool>::type = true) {
  fp_struct<FP> xs(x);
  ap_ufixed<1 + 2 * fp_struct<FP>::SIG_BITS + 8 * sizeof(T),
            fp_struct<FP>::SIG_BITS + 8 * sizeof(T)>
      mantissa = xs.mantissa();

  ap_ufixed<1 + 2 * fp_struct<FP>::SIG_BITS + 8 * sizeof(T),
            fp_struct<FP>::SIG_BITS + 8 * sizeof(T)>
      smantissa = mantissa << xs.expv();

  ap_ufixed<1, 8 * sizeof(T), AP_TRN, AP_SAT> overflow_bits = smantissa;
  ap_ufixed<8 * sizeof(T) - 1, 8 * sizeof(T) - 1, ROUNDING> val = smantissa;

  bool overflow;
  if (fp_struct<FP>::SIG_BITS + 8 * sizeof(T) >=
      (fp_struct<FP>::EXP_INFNAN - fp_struct<FP>::EXP_BIAS)) {
    // Basically, this means that the exponent width of T will not result in a
    // shift past the width of smantissa.  The only way to overflow without
    // overflow_bits being set is to have an infinity. Basically for half
    // precision
    overflow = (xs.exp == fp_struct<FP>::EXP_INFNAN) || (overflow_bits != 0);
  } else {
    // Basically, this means that the exponent width of T will result in a shift
    // past the width of smantissa.
    overflow = xs.expv() >= (signed)(fp_struct<FP>::SIG_BITS + 8 * sizeof(T)) ||
               (overflow_bits != 0);
  }

  ap_int<8 * sizeof(T)> minval = 0;
  minval[8 * sizeof(T) - 1] = 1;
  ap_int<8 * sizeof(T)> maxval = -1;
  maxval[8 * sizeof(T) - 1] = 0;
  if (overflow && detect_overflow)
    return xs.__signbit() ? minval : maxval;

#ifdef DEBUG
  std::cout << std::hex << "overflow_bits = " << overflow_bits.to_string(16)
            << std::dec << "\n";
  std::cout << std::hex << "mantissa = " << mantissa.to_string(16) << std::dec
            << "\n";
  std::cout << std::hex << "smantissa = " << smantissa.to_string(16) << std::dec
            << "\n";
  std::cout << std::hex << "val = " << val.to_string(16) << std::dec << "\n";
#endif

  ap_fixed<8 * sizeof(T), 8 * sizeof(T)> result = val;
  if (xs.__signbit())
    result = -val;
  // std::cout << std::hex << "result = " << result.to_string(16) << std::dec <<
  // "\n";
  return result;
}

template <typename T, ap_q_mode ROUNDING, typename FP>
T cast_IEEE754(
    FP x, bool detect_overflow = false,
    typename enable_if<!std::numeric_limits<T>::is_signed, bool>::type = true) {
  fp_struct<FP> xs(x);
  ap_ufixed<1 + 2 * fp_struct<FP>::SIG_BITS + 8 * sizeof(T),
            fp_struct<FP>::SIG_BITS + 8 * sizeof(T)>
      mantissa = xs.mantissa();

  ap_ufixed<1 + 2 * fp_struct<FP>::SIG_BITS + 8 * sizeof(T),
            fp_struct<FP>::SIG_BITS + 8 * sizeof(T)>
      smantissa = mantissa << xs.expv();

  ap_ufixed<1, 8 * sizeof(T) + 1, AP_TRN, AP_SAT> overflow_bits = smantissa;
  ap_ufixed<8 * sizeof(T), 8 * sizeof(T), ROUNDING> val = smantissa;

  bool overflow;
  if (fp_struct<FP>::SIG_BITS + 8 * sizeof(T) >=
      (fp_struct<FP>::EXP_INFNAN - fp_struct<FP>::EXP_BIAS)) {
    // Basically, this means that the exponent width of T will not result in a
    // shift past the width of smantissa.  The only way to overflow without
    // overflow_bits being set is to have an infinity. Basically for half
    // precision
    overflow = (xs.exp == fp_struct<FP>::EXP_INFNAN) || (overflow_bits != 0);
  } else {
    // Basically, this means that the exponent width of T will result in a shift
    // past the width of smantissa.
    overflow = xs.expv() >= (signed)(fp_struct<FP>::SIG_BITS + 8 * sizeof(T)) ||
               (overflow_bits != 0);
  }

  ap_uint<8 * sizeof(T)> minval = 0;
  ap_uint<8 * sizeof(T)> maxval = -1;
  if ((overflow || xs.__signbit()) && detect_overflow)
    return xs.__signbit() ? minval : maxval;

#ifdef DEBUG
  std::cout << std::hex << "overflow_bits = " << overflow_bits.to_string(16)
            << std::dec << "\n";
  std::cout << std::hex << "mantissa = " << mantissa.to_string(16) << std::dec
            << "\n";
  std::cout << std::hex << "smantissa = " << smantissa.to_string(16) << std::dec
            << "\n";
  std::cout << std::hex << "val = " << val.to_string(16) << std::dec << "\n";
#endif

  ap_fixed<8 * sizeof(T), 8 * sizeof(T)> result = val;
  // std::cout << std::hex << "result = " << result.to_string(16) << std::dec <<
  // "\n";
  return result;
}

template <typename T, typename FP>
T cast_IEEE754(FP x, bool detect_overflow = false) {
  return cast_IEEE754<T, AP_TRN_ZERO>(x, detect_overflow);
}

// lrint* functions use the current rounding mode.
// Currently HLS provides no way to modify the rounding mode.
static long int lrint(double x) {
  // Note: long int may be 32 or 64 bits.
  return cast_IEEE754<long int, AP_RND_CONV>(x, true);
}

static long long int llrint(double x) {
  return cast_IEEE754<long long int, AP_RND_CONV>(x, true);
}

static long int lrintf(float x) {
  // Note: long int may be 32 or 64 bits.
  return cast_IEEE754<long int, AP_RND_CONV>(x, true);
}

static long long int llrintf(float x) {
  return cast_IEEE754<long long int, AP_RND_CONV>(x, true);
}

static long int half_lrint(half x) {
  // Note: long int may be 32 or 64 bits.
  return cast_IEEE754<long int, AP_RND_CONV>(x, true);
}

static long long int half_llrint(half x) {
  return cast_IEEE754<long long int, AP_RND_CONV>(x, true);
}

static long int lround(double x) {
  // Note: long int may be 32 or 64 bits.
  return cast_IEEE754<long int, AP_RND_INF>(x, true);
}

static long long int llround(double x) {
  return cast_IEEE754<long long int, AP_RND_INF>(x, true);
}

static long int lroundf(float x) {
  // Note: long int may be 32 or 64 bits.
  return cast_IEEE754<long int, AP_RND_INF>(x, true);
}

static long long int llroundf(float x) {
  return cast_IEEE754<long long int, AP_RND_INF>(x, true);
}

static long int half_lround(half x) {
  // Note: long int may be 32 or 64 bits.
  return cast_IEEE754<long int, AP_RND_INF>(x, true);
}

static long long int half_llround(half x) {
  return cast_IEEE754<long long int, AP_RND_INF>(x, true);
}

template <typename T> T generic_modf(T x, T *intpart) {
  typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS];
  init_mask_table<T>(mask_table);
  fp_struct<T> xs(x);

  if (xs.exp < fp_struct<T>::EXP_BIAS) {
    *intpart = ::hls::copysign((T)0.0, x);
    return x;
  } else if ((xs.exp > fp_struct<T>::EXP_BIAS + fp_struct<T>::SIG_BITS)) {
    // In this special case, we know there are no fractional bits.
    *intpart = x;
    if (::hls::__isnan(x)) {
      fp_struct<T> nan;
      nan.sig = -1; // all ones
      nan.exp = -1; // all ones
      nan.sign = 0;
      return nan.to_ieee();
    } else {
      return ::hls::copysign((T)0.0, x);
    }
  } else {
    typename fp_struct<T>::inttype mask;
    // Find the mask.  These are 1's where there are fractional bits.
    // Note that the other bits should be zero, because of the special cases
    // above.
    ap_uint<UnsignedBitWidth<fp_struct<T>::BITS>::Value> index =
        xs.exp(UnsignedBitWidth<fp_struct<T>::BITS>::Value - 1, 0);
    mask = mask_table[index];

    // normalize the fractional part
    fp_struct<T> xf;
    xf.sig = xs.sig & mask;

    int zeros = xf.sig.countLeadingZeros();
    // If the significand is exactly zero, then we need to return a floating
    // point zero Otherwise compute the new exponent from the old one.
    xf.exp = (xf.sig == 0) ? 0 : (unsigned short)(xs.exp - zeros - 1);
    // Normalize the significand, shifting off the zeros and the leading 1.
    xf.sig = (xf.sig << 1) << zeros;
    xf.sign = xs.sign;

    // mask the integer part
    xs.sig = xs.sig & (~mask);
    *intpart = xs.to_ieee();

    return xf.to_ieee();
  }
}

static double modf(double x, double *intpart) {
  return generic_modf<double>(x, intpart);
}

static float modff(float x, float *intpart) {
  return generic_modf<float>(x, intpart);
}

static half half_modf(half x, half *intpart) {
  return generic_modf<half>(x, intpart);
}

#ifdef __cplusplus
static float modf(float x, float *intpart) { return modff(x, intpart); }
static half modf(half x, half *intpart) {
  return generic_modf<half>(x, intpart);
}
#endif

template <typename T> T generic_fract(T x, T *intpart) {
  typename fp_struct<T>::inttype mask_table[fp_struct<T>::BITS];
  init_mask_table<T>(mask_table);
  fp_struct<T> xs(x);

  // The number that is slightly less than 1. (e.g. for float: 0x1.fffffep-1f);
  fp_struct<T> special_case;
  special_case.sign = 0;
  special_case.exp = fp_struct<T>::EXP_BIAS - 1;
  special_case.sig = -1;

  if (xs.exp <
      fp_struct<T>::EXP_BIAS) { // fp_struct<float>::EXP_BIAS is biased zero
    if (xs.__signbit() &&
        !(xs.sig == 0 && xs.exp == 0)) { // negative and not zero
      *intpart = (T)-1.0f;
      T f = (T)1.0 + x;
      fp_struct<T> xf(f);
      bool is_one = (xf.exp == fp_struct<T>::EXP_BIAS) && xf.sig == 0;
      if (is_one) {
        return special_case.to_ieee();
      } else {
        return xf.to_ieee();
      }
    } else {
      *intpart = ::hls::copysign((T)0.0, x);
      return x;
    }
  } else if (xs.exp == fp_struct<T>::EXP_INFNAN) { // Infinity or nan
    *intpart = x;
    if (xs.sig == 0)
      return ::hls::copysign((T)0.0, x);
    return x;
  } else if ((xs.exp > fp_struct<T>::EXP_BIAS + fp_struct<T>::SIG_BITS)) {
    // In this special case, we know there are no fractional bits.
    *intpart = x;
    return ::hls::copysign((T)0.0, x);
  } else {
    typename fp_struct<T>::inttype mask;
    // Find the mask.  These are 1's where there are fractional bits.
    // Note that the other bits should be zero, because of the special cases
    // above.
    ap_uint<UnsignedBitWidth<fp_struct<T>::BITS>::Value> index =
        xs.exp(UnsignedBitWidth<fp_struct<T>::BITS>::Value - 1, 0);
    mask = mask_table[index];

    // normalize the fractional part
    fp_struct<T> xf;
    if (xs.__signbit())
      xf.sig = -xs.sig;
    else
      xf.sig = xs.sig;
    xf.sig = xf.sig & mask; // essentially 1.0f - xs.to_ieee()

    int zeros = xf.sig.countLeadingZeros();
    ;
    // If the significand is exactly zero, then we need to return a floating
    // point zero Otherwise compute the new exponent from the old one.
    xf.exp = (xf.sig == 0) ? 0 : (unsigned short)(xs.exp - zeros - 1);
    // Normalize the significand, shifting off the zeros and the leading 1.
    xf.sig = (xf.sig << 1) << zeros;
    xf.sign = 0;

    // Deal with negative numbers
    if (xs.__signbit()) {
      xs = typename fp_struct<T>::data_type(xs.data() + mask);
    }

    // mask the integer part
    xs.sig = xs.sig & (~mask);
    *intpart = xs.to_ieee();

    return xf.to_ieee();
  }
}
static double fract(double x, double *intpart) {
  return generic_fract<double>(x, intpart);
}

static float fractf(float x, float *intpart) {
  return generic_fract<float>(x, intpart);
}
static half half_fract(half x, half *intpart) {
  return generic_fract<half>(x, intpart);
}

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
