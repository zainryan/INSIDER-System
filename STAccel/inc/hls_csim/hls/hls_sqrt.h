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
 *     (c) Copyright 2013 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/**
 * @file hls_sqrt.h
 */

#define Wg 1
template <int W, int I, ap_q_mode _AP_Q, ap_o_mode _AP_O>
ap_ufixed<W, I> sqrt(ap_ufixed<W, I, _AP_Q, _AP_O> x) {
  assert(I >= 0 &&
         "Number of integer bits for sqrt() must be greater than zero");
  assert(W >= I && "Number of integer bits for sqrt() must be less than or "
                   "equal to total width");
  ap_ufixed<W + Wg, I> factor = 0;
  int offset;
  // Since input bits are handled in pairs, the
  // start condition for even and odd integer widths
  // are handled slightly differently.
  if (I % 2 == 0) {
    offset = 1;
  } else {
    offset = 0;
  }
  factor[W + Wg - 1 - offset] = 1;
  ap_ufixed<W + Wg + 1, I + 1> result = 0;
  ap_ufixed<W + Wg + 2, I + 2> x2 = x;
  for (int i = W + Wg - offset; i > (I - 1) / 2; i -= 1) {

    ap_ufixed<W + 2 + Wg, I + 2> t = (result << 1) + factor;
    //        std::cout << x2.to_string(2) << " " << t.to_string(2) << " " <<
    //        result.to_string(2) << " " << factor.to_string(2) << "\n";
    ap_ufixed<W + Wg, I> thisfactor = 0;
    if (x2 >= t) {
      x2 -= t;
      thisfactor = factor;
    }
    result = result + thisfactor;
    factor >>= 1;
    x2 <<= 1;
  }
  //    std::cout << W << " " << I << " " << offset << ((I-1) >> 1) << " " <<
  //    x2.to_string(2) << " " << result.to_string(2) << " " <<
  //    factor.to_string(2) << "\n";
  return result >> ((I - 1) >> 1);
  // I <= 0 ? (I-2)/2 : (I-1)/2);
}

template <int W, int I, ap_q_mode _AP_Q, ap_o_mode _AP_O>
ap_fixed<W, I> sqrt(ap_fixed<W, I, _AP_Q, _AP_O> x) {
  ap_ufixed<W - 1, I - 1> x2 = x;
  if (x < 0)
    return 0;
  else
    return ::hls::sqrt(x2);
}

template <typename T> class cbrt_traits {};

template <> class cbrt_traits<half> {
public:
  static ap_uint<2> exp_cbrt(ap_uint<5> &exp) { // we = 5 for half
    // ceil((x-15)/3)+15 = ceil(x/3)+10

    // each bit of exp has a remainder of { 1, 2, 1, 2, ... }
    ap_uint<1> r_0;
    { r_0 = (exp[0]) ? 1 : 0; }
    ap_uint<2> r_1;
    { r_1 = (exp[1]) ? 2 : 0; }
    ap_uint<1> r_2;
    { r_2 = (exp[2]) ? 1 : 0; }
    ap_uint<2> r_3;
    { r_3 = (exp[3]) ? 2 : 0; }
    ap_uint<1> r_4;
    { r_4 = (exp[4]) ? 1 : 0; }
    ap_uint<3> r_l =
        r_0 + r_1 + r_2 + r_3 + r_4; // sum the remainders of all bits of exp

    // calculate
    // exp_r = r_l / 3
    // r = r_l % 3
    ap_uint<2> exp_r;
    ap_uint<2> r;
    exp_r[1] = (r_l[2] & r_l[1]);                      // 11x
    exp_r[0] = (r_l[1] & r_l[0]) | (r_l[2] & ~r_l[1]); // x11, 10x
    r[1] = (~r_l[2] & ~r_l[0]) | (r_l[2] & r_l[0]);    // 0x0, 1x1
    r[0] = (~r_l[2] & ~r_l[1]) | (~r_l[1] & ~r_l[0]);  // 00x, x00
    assert(r < 3);
    assert(r_l = exp_r * 3 + r);

    // each bit of exp / 3 = { 0, 0, 1, 2, 5 }
    ap_uint<1> exp_2;
    { exp_2 = (exp[2]) ? 1 : 0; }
    ap_uint<2> exp_3;
    { exp_3 = (exp[3]) ? 2 : 0; }
    ap_uint<3> exp_4;
    { exp_4 = (exp[4]) ? 5 : 0; }

    // sum to return
    exp = exp_r + exp_2 + exp_3 + exp_4 + 10;

    return r;
  }
};

template <> class cbrt_traits<float> {
public:
  static ap_uint<2> exp_cbrt(ap_uint<8> &exp) { // we = 8 for float
    // ceil((x-127)/3)+127 = ceil((x-126-1)/3)+127 = ceil(x-1/3)+85

    // each bit of exp has a remainder of { 1, 2, 1, 2, ... }
    ap_uint<1> r_0;
    { r_0 = (exp[0]) ? 1 : 0; }
    ap_uint<2> r_1;
    { r_1 = (exp[1]) ? 2 : 0; }
    ap_uint<1> r_2;
    { r_2 = (exp[2]) ? 1 : 0; }
    ap_uint<2> r_3;
    { r_3 = (exp[3]) ? 2 : 0; }
    ap_uint<1> r_4;
    { r_4 = (exp[4]) ? 1 : 0; }
    ap_uint<2> r_5;
    { r_5 = (exp[5]) ? 2 : 0; }
    ap_uint<1> r_6;
    { r_6 = (exp[6]) ? 1 : 0; }
    ap_uint<2> r_7;
    { r_7 = (exp[7]) ? 2 : 0; }
    ap_uint<4> r_l = r_0 + r_1 + r_2 + r_3 + r_4 + r_5 + r_6 +
                     r_7; // sum the remainders of all bits of exp

    // calculate
    // exp_r = (r_l-1) / 3
    // r = (r_l-1) % 3
    ap_uint<2> exp_r;
    ap_uint<2> r;
    exp_r[1] = (r_l[2] & r_l[1] & r_l[0]) | (r_l[3]); // x111, 1xxx
    exp_r[0] = (r_l[2] & ~r_l[1]) | (r_l[2] & r_l[1] & ~r_l[0]) |
               (r_l[3] & r_l[1]); // x10x, x110, 1x1x
    r[1] = (~r_l[3] & ~r_l[2] & r_l[1] & r_l[0]) | (r_l[2] & r_l[1] & ~r_l[0]) |
           (r_l[3] & ~r_l[1] & r_l[0]); // 0011, x110, 1x01
    r[0] = (~r_l[3] & ~r_l[2] & ~r_l[0]) | (r_l[2] & ~r_l[1] & r_l[0]) |
           (~r_l[2] & ~r_l[1] & ~r_l[0]) |
           (r_l[3] & r_l[1] & r_l[0]); // 00x0, x101, x000, 1x11
    assert(r < 3);
    assert(r_l = exp_r * 3 + r + 1);

    // each bit of exp / 3 = { 0, 0, 1, 2, 5, 10, 21, 42 }
    ap_uint<1> exp_2;
    { exp_2 = (exp[2]) ? 1 : 0; }
    ap_uint<2> exp_3;
    { exp_3 = (exp[3]) ? 2 : 0; }
    ap_uint<3> exp_4;
    { exp_4 = (exp[4]) ? 5 : 0; }
    ap_uint<4> exp_5;
    { exp_5 = (exp[5]) ? 10 : 0; }
    ap_uint<5> exp_6;
    { exp_6 = (exp[6]) ? 21 : 0; }
    ap_uint<6> exp_7;
    { exp_7 = (exp[7]) ? 42 : 0; }

    // sum to return
    exp = exp_r + exp_2 + exp_3 + exp_4 + exp_5 + exp_6 + exp_7 + 85;

    return r;
  }
};

template <> class cbrt_traits<double> {
public:
  static ap_uint<2> exp_cbrt(ap_uint<11> &exp) { // we = 11 for double
    // ceil((x-1023)/3)+1023 = ceil(x/3)+682

    // each bit of exp has a remainder of { 1, 2, 1, 2, ... }
    ap_uint<1> r_0;
    { r_0 = (exp[0]) ? 1 : 0; }
    ap_uint<2> r_1;
    { r_1 = (exp[1]) ? 2 : 0; }
    ap_uint<1> r_2;
    { r_2 = (exp[2]) ? 1 : 0; }
    ap_uint<2> r_3;
    { r_3 = (exp[3]) ? 2 : 0; }
    ap_uint<1> r_4;
    { r_4 = (exp[4]) ? 1 : 0; }
    ap_uint<2> r_5;
    { r_5 = (exp[5]) ? 2 : 0; }
    ap_uint<1> r_6;
    { r_6 = (exp[6]) ? 1 : 0; }
    ap_uint<2> r_7;
    { r_7 = (exp[7]) ? 2 : 0; }
    ap_uint<1> r_8;
    { r_8 = (exp[8]) ? 1 : 0; }
    ap_uint<2> r_9;
    { r_9 = (exp[9]) ? 2 : 0; }
    ap_uint<1> r_10;
    { r_10 = (exp[10]) ? 1 : 0; }
    ap_uint<4> r_l = r_0 + r_1 + r_2 + r_3 + r_4 + r_5 + r_6 + r_7 + r_8 + r_9 +
                     r_10; // sum the remainders of all bits of exp

    // calculate
    // exp_r = r_l / 3
    // r = r_l-1 % 3
    ap_uint<3> exp_r;
    ap_uint<2> r;
    exp_r[2] = (r_l[3] & r_l[2]);                                // 11xx
    exp_r[1] = (~r_l[3] & r_l[2] & r_l[1]) | (r_l[3] & ~r_l[2]); // 011x, 10xx
    exp_r[0] =
        (~r_l[3] & ~r_l[2] & r_l[1] & r_l[0]) | (~r_l[3] & r_l[2] & ~r_l[1]) |
        (r_l[3] & ~r_l[2] & ~r_l[1] & r_l[0]) | (r_l[3] & ~r_l[2] & r_l[1]) |
        (r_l[3] & r_l[2] & r_l[1] & r_l[0]); // 0011, 010x, 1001, 101x, 1111
    r[1] =
        (~r_l[3] & ~r_l[2] & ~r_l[0]) | (~r_l[3] & r_l[2] & ~r_l[1] & r_l[0]) |
        (~r_l[2] & ~r_l[1] & ~r_l[0]) | (r_l[3] & ~r_l[2] & r_l[1] & r_l[0]) |
        (r_l[3] & r_l[2] & r_l[1] & ~r_l[0]); // 00x0, 0101, x000, 1011, 1110
    r[0] = (~r_l[3] & ~r_l[2] & ~r_l[1]) | (~r_l[3] & ~r_l[1] & ~r_l[0]) |
           (~r_l[3] & r_l[2] & r_l[1] & r_l[0]) |
           (r_l[3] & ~r_l[2] & r_l[1] & ~r_l[0]) |
           (r_l[3] & r_l[2] & ~r_l[1] & r_l[0]); // 000x, 0x00, 0111, 1010, 1101
    assert(r < 3);
    assert(r_l = exp_r * 3 + r);

    // each bit of exp / 3 = { 0, 0, 1, 2, 5, 10, 21, 42 }
    ap_uint<1> exp_2;
    { exp_2 = (exp[2]) ? 1 : 0; }
    ap_uint<2> exp_3;
    { exp_3 = (exp[3]) ? 2 : 0; }
    ap_uint<3> exp_4;
    { exp_4 = (exp[4]) ? 5 : 0; }
    ap_uint<4> exp_5;
    { exp_5 = (exp[5]) ? 10 : 0; }
    ap_uint<5> exp_6;
    { exp_6 = (exp[6]) ? 21 : 0; }
    ap_uint<6> exp_7;
    { exp_7 = (exp[7]) ? 42 : 0; }
    ap_uint<7> exp_8;
    { exp_8 = (exp[8]) ? 85 : 0; }
    ap_uint<8> exp_9;
    { exp_9 = (exp[9]) ? 170 : 0; }
    ap_uint<9> exp_10;
    { exp_10 = (exp[10]) ? 341 : 0; }

    // sum to return
    exp = exp_r + exp_2 + exp_3 + exp_4 + exp_5 + exp_6 + exp_7 + exp_8 +
          exp_9 + exp_10 + 682;

    return r;
  }
};

template <typename T> T cbrt_generic(T x) {
#pragma HLS pipeline

  fp_struct<T> xs(x);

  //	x	|	0	|	inf	|	NaN	|	normal
  // =============================================================================
  // 	cbrt(x)	|	0	|	inf	|	NaN
  //
  if (xs.exp == 0)
    xs.sig = 0;
  if ((xs.exp == 0) || (xs.exp == fp_struct<T>::EXP_INFNAN))
    return xs.to_ieee();

  const static int we = fp_struct<T>::EXP_BITS;
  const static int wf = fp_struct<T>::SIG_BITS;

  fp_struct<T> out;
  out.sign[0] = xs.sign[0];

  ap_uint<we> x_exp = xs.exp; // bias will be removed by exp_cbrt()
  ap_uint<2> r = cbrt_traits<T>::exp_cbrt(x_exp);
  const static int prcs = (wf + 1) * 3;   // max precision
  ap_ufixed<4 + prcs, 4> x_frac = 0;      // = sigificant << r
  x_frac[x_frac.wl() - x_frac.iwl()] = 1; // The implicit '1' in IEEE format.
  x_frac(x_frac.wl() - x_frac.iwl() - 1, x_frac.wl() - x_frac.iwl() - wf) =
      xs.sig(wf - 1, 0);
  if (r[1] | r[0])
    x_frac <<= 1;
  if (r[1])
    x_frac <<= 1;

  ap_ufixed<3, 3> x_frac_i;
  x_frac_i(2, 0) = x_frac(x_frac.wl() - 2, x_frac.wl() - x_frac.iwl());
  x_frac_i -= 1;
  x_frac(x_frac.wl() - 2, x_frac.wl() - x_frac.iwl()) = x_frac_i(2, 0);

  ap_ufixed<1 + prcs, 1> res = 1;  // result to return
  ap_ufixed<2 + prcs, 2> resq = 1; // sqare of result
#pragma unroll
  // each iteration calculate 1 bit of result, starting from MSB
  // bit is used to store the current bit under calculation
  // if accepted bit will be added to result
  for (int pos = -1; pos >= -wf - 1; pos--) {
    ap_ufixed<2 + prcs, 2> mul1a = 0; // 2 * resq * bit
    ap_ufixed<1 + prcs, 1> mul1b = 0; // 1 * resq * bit
    assert(resq(pos * 2 + 1 + prcs, 0) == 0);
    mul1a(pos + 2 + prcs, pos * 3 + 3 + prcs) =
        resq(1 + prcs, pos * 2 + 2 + prcs);
    mul1b(pos + 1 + prcs, pos * 3 + 2 + prcs) =
        resq(1 + prcs, pos * 2 + 2 + prcs);

    ap_ufixed<0 + prcs, 0> mul2a = 0;   // 2 * res * bit^2
    ap_ufixed<-1 + prcs, -1> mul2b = 0; // 1 * res * bit^2
    assert(res(pos + prcs, 0) == 0);
    mul2a(pos * 2 + 1 + prcs, pos * 3 + 2 + prcs) =
        res(0 + prcs, pos + 1 + prcs);
    mul2b(pos * 2 + prcs, pos * 3 + 1 + prcs) = res(0 + prcs, pos + 1 + prcs);

    // ap_ufixed<3 + prcs, 3> mul = 0;
    /*mul ( pos+3 + prcs , pos*3+1 + prcs ) = mul1a ( pos+2   + prcs , pos*3+1 +
       prcs ) + mul1b ( pos+1 + prcs , pos*3+1 + prcs ) + \ mul2a ( pos*2+1 +
       prcs , pos*3+1 + prcs ) + mul2b ( pos*2 + prcs , pos*3+1 + prcs ); //
       must align the last bit*/
    // assert ( mul == mul1a + mul1b + mul2a + mul2b );
    // mul [ pos*3 + prcs ] = 1;

    ap_ufixed<3 + prcs, 3> mulH = 0;
    ap_ufixed<3 + prcs, 3> mulL = 0;
    mulL(pos * 2 + 3 + prcs, pos * 3 + 1 + prcs) =
        mul1a(pos * 2 + 1 + prcs, pos * 3 + 1 + prcs) +
        mul1b(pos * 2 + 1 + prcs, pos * 3 + 1 + prcs) +
        mul2a(pos * 2 + 1 + prcs, pos * 3 + 1 + prcs) +
        mul2b(pos * 2 + prcs, pos * 3 + 1 + prcs); // must align the last bit
    mulH(pos + 3 + prcs, pos * 2 + 2 + prcs) =
        mul1a(pos + 2 + prcs, pos * 2 + 2 + prcs) +
        mul1b(pos + 1 + prcs, pos * 2 + 2 + prcs) +
        mulL(pos * 2 + 3 + prcs, pos * 2 + 2 + prcs); // must align the last bit
    // mulL [ pos*2+3 + prcs ] = 0; // just for assert
    // mulL [ pos*2+2 + prcs ] = 0; // just for assert
    // assert ( mulH + mulL == mul1a + mul1b + mul2a + mul2b );
    mulL[pos * 3 + prcs] = 1;

    assert((pos >= -2) || (x_frac(2 + prcs, pos + 5 + prcs) == 0));
    bool cond1 = (x_frac(pos + 4 + prcs, pos * 2 + 2 + prcs) >
                  mulH(pos + 3 + prcs, pos * 2 + 2 + prcs));
    bool cond2 = (x_frac(pos + 4 + prcs, pos * 2 + 2 + prcs) ==
                  mulH(pos + 3 + prcs, pos * 2 + 2 + prcs));
    bool cond3 = (x_frac(pos * 2 + 1 + prcs, pos * 3 + prcs) >=
                  mulL(pos * 2 + 1 + prcs, pos * 3 + prcs));
    // if ( x_frac ( pos+4 + prcs , pos*3 + prcs ) >= mul ( pos+3 + prcs, pos*3
    // + prcs ) ) {
    if (cond1 | (cond2 & cond3)) {
      // assert ( x_frac >= mulH + mulL );
      // ap_ufixed<4 + prcs, 4> x_frac_ = x_frac; // just for assert
      // x_frac   ( pos  +4 + prcs , pos*3   + prcs ) =   ( x_frac ( pos+4 +
      // prcs , pos*3 + prcs ) - mul ( pos+3 + prcs , pos*3 + prcs ) ); assert (
      // x_frac == x_frac_ - mul );
      ap_ufixed<4 + prcs, 4> x_frac_L = 0;
      x_frac_L(pos * 2 + 1 + prcs, pos * 3 + prcs) =
          x_frac(pos * 2 + 1 + prcs, pos * 3 + prcs);
      x_frac_L[pos * 2 + 2 + prcs] = cond3 ? 0 : 1;
      x_frac(pos + 4 + prcs, pos * 2 + 2 + prcs) =
          x_frac(pos + 4 + prcs, pos * 2 + 2 + prcs) -
          mulH(pos + 3 + prcs, pos * 2 + 2 + prcs) -
          x_frac_L(pos * 2 + 2 + prcs, pos * 2 + 2 + prcs);
      x_frac(pos * 2 + 1 + prcs, pos * 3 + prcs) =
          x_frac_L(pos * 2 + 2 + prcs, pos * 3 + prcs) -
          mulL(pos * 2 + 1 + prcs, pos * 3 + prcs);
      // assert ( x_frac == x_frac_ - mulH - mulL );

      ap_ufixed<1 + prcs, 1> mul3 = 0; // 2 * res * bit
      assert(res(pos + prcs, 0) == 0);
      assert(resq(pos * 2 + 1 + prcs, 0) == 0);
      mul3(pos + 1 + prcs, pos * 2 + 2 + prcs) = res(0 + prcs, pos + 1 + prcs);
      // resq ( 1 + prcs , pos*2+2 + prcs ) = resq ( 1 + prcs , pos*2+2 + prcs )
      // + mul3 ( pos+1 + prcs , pos*2+2 + prcs );
      ap_ufixed<2 + prcs, 2> resq_L = 0;
      resq_L(pos + 2 + prcs, pos * 2 + 2 + prcs) =
          resq(pos + 1 + prcs, pos * 2 + 2 + prcs) +
          mul3(pos + 1 + prcs, pos * 2 + 2 + prcs);
      resq(pos + 1 + prcs, pos * 2 + 2 + prcs) =
          resq_L(pos + 1 + prcs, pos * 2 + 2 + prcs);
      resq(1 + prcs, pos + 2 + prcs) = resq(1 + prcs, pos + 2 + prcs) +
                                       resq_L(pos + 2 + prcs, pos + 2 + prcs);
      resq[pos * 2 + prcs] = 1;
      res[pos + prcs] = 1;
    }
  }

  ap_ufixed<1, -wf> delta;
  delta[0] = 1;
  ap_ufixed<2 + wf + 1, 2> res_s = res;
  res_s += delta;

  if (res_s[res_s.wl() - 1] == 1) {
    x_exp++;
    res_s >>= 1;
  }

  out.exp = x_exp;
  out.sig(wf - 1, 0) =
      res_s(res_s.wl() - res_s.iwl() - 1, res_s.wl() - res_s.iwl() - wf);
  return out.to_ieee();
}

static double hypot(double x, double y) {
  if (::hls::__isinf(x))
    return fabs(x);
  if (::hls::__isinf(y))
    return fabs(y);
  return ::HLS_FPO_SQRT(x * x + y * y);
}
static float hypotf(float x, float y) {
  if (::hls::__isinf(x))
    return fabs(x);
  if (::hls::__isinf(y))
    return fabs(y);
  double xd = x;
  double yd = y;
  return ::HLS_FPO_SQRT(xd * xd + yd * yd);
}
static half half_hypot(half x, half y) {
  if (::hls::__isinf(x))
    return fabs(x);
  if (::hls::__isinf(y))
    return fabs(y);
  float xf = x;
  float yf = y;
  return ::HLS_FPO_SQRTF(xf * xf + yf * yf);
}

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
