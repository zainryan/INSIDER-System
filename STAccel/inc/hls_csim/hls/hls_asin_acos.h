/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * @file hls_asin_acos.h
 * This file contains arcsine and arccos functions, mainly based on ieee754/
 * implementation except some optimization for fpga. This implementation will
 * occupy huge logic when doesn't share double operations (multi, add, div),
 * this need further optimize or adopt new approximation method suite to fpga,
 * like hotbm
 */

#ifndef X_HLS_MATH_H
#error hls_erf.h is not intended to be included directly.  Please include hls_math.h instead.
#endif

#ifndef X_HLS_ASIN_ACOS_H
#define X_HLS_ASIN_ACOS_H

namespace asin_acos_approx {
static float asin(float x) {
  static const float one = 1.0000000000e+00, /* 0x3F800000 */
      huge = 1.000e+30,

                     pio2_hi = 1.57079637050628662109375f,
                     pio2_lo = -4.37113900018624283e-8f,
                     pio4_hi = 0.785398185253143310546875f,

                     /* asin x = x + x^3 p(x^2)
                        -0.5 <= x <= 0.5;
                        Peak relative error 4.8e-9 */
      p0 = 1.666675248e-1f, p1 = 7.495297643e-2f, p2 = 4.547037598e-2f,
                     p3 = 2.417951451e-2f, p4 = 4.216630880e-2f;

  // flush subnormal
  if (fpclassify(x) == FP_SUBNORMAL)
    x = 0.0f;

  float t, w, p, q, c, r, s;
  int32_t hx, ix;
  // GET_FLOAT_WORD(hx,x);
  fp_struct<float> fp_x(x);
  ap_uint<32> tmp = fp_x.data()(31, 0);
  hx = tmp.to_int();
  ix = hx & 0x7fffffff;
  if (ix == 0x3f800000) {
    /* asin(1)=+-pi/2 with inexact */
    return x * pio2_hi + x * pio2_lo;
  } else if (ix > 0x3f800000) { /* |x|>= 1 */
    return (x - x) / (x - x);   /* asin(|x|>1) is NaN */
  } else if (ix < 0x3f000000) { /* |x|<0.5 */
    if (ix < 0x32000000) {      /* if |x| < 2**-27 */
      if (huge + x > one)
        return x; /* return x with inexact if x!=0*/
    } else {
      t = x * x;
      w = t * (p0 + t * (p1 + t * (p2 + t * (p3 + t * p4))));
      return x + x * w;
    }
  }
  /* 1> |x|>= 0.5 */
  w = one - fabsf(x);
  t = w * 0.5f;
  p = t * (p0 + t * (p1 + t * (p2 + t * (p3 + t * p4))));
  // s = __ieee754_sqrtf(t);
  s = sqrtf(t);
  if (ix >= 0x3F79999A) { /* if |x| > 0.975 */
    t = pio2_hi - (2.0f * (s + s * p) - pio2_lo);
  } else {
    // int32_t iw;
    w = s;
    // GET_FLOAT_WORD(iw,w);
    // SET_FLOAT_WORD(w,iw&0xfffff000);
    fp_struct<float> fp_w(w);
    ap_uint<Type_BitWidth<float>::Value> data = fp_w.data() & 0xfffff000;
    fp_struct<float> fp_w_new(data);
    w = fp_w_new.to_ieee();
    c = (t - w * w) / (s + w);
    r = p;
    p = 2.0f * s * r - (pio2_lo - 2.0f * c);
    q = pio4_hi - 2.0f * w;
    t = pio4_hi - (p - q);
  }
  if (hx > 0)
    return t;
  else
    return -t;
}

static float acos(float x) {
  static const float one = 1.0000000000e+00, /* 0x3F800000 */
      pi = 3.1415925026e+00,                 /* 0x40490fda */
      pio2_hi = 1.5707962513e+00,            /* 0x3fc90fda */
      pio2_lo = 7.5497894159e-08,            /* 0x33a22168 */
      pS0 = 1.6666667163e-01,                /* 0x3e2aaaab */
      pS1 = -3.2556581497e-01,               /* 0xbea6b090 */
      pS2 = 2.0121252537e-01,                /* 0x3e4e0aa8 */
      pS3 = -4.0055535734e-02,               /* 0xbd241146 */
      pS4 = 7.9153501429e-04,                /* 0x3a4f7f04 */
      pS5 = 3.4793309169e-05,                /* 0x3811ef08 */
      qS1 = -2.4033949375e+00,               /* 0xc019d139 */
      qS2 = 2.0209457874e+00,                /* 0x4001572d */
      qS3 = -6.8828397989e-01,               /* 0xbf303361 */
      qS4 = 7.7038154006e-02;                /* 0x3d9dc62e */

  float z, p, q, r, w, s, c, df;
  int32_t hx, ix;
  // GET_FLOAT_WORD(hx,x);
  fp_struct<float> fp_x(x);
  ap_uint<32> tmp = fp_x.data()(31, 0);
  hx = tmp.to_int();
  ix = hx & 0x7fffffff;
  if (ix == 0x3f800000) { /* |x|==1 */
    if (hx > 0)
      return 0.0; /* acos(1) = 0  */
    else
      return pi + (float)2.0 * pio2_lo; /* acos(-1)= pi */
  } else if (ix > 0x3f800000) {         /* |x| >= 1 */
    return (x - x) / (x - x);           /* acos(|x|>1) is NaN */
  }
  if (ix < 0x3f000000) { /* |x| < 0.5 */
    if (ix <= 0x23000000)
      return pio2_hi + pio2_lo; /*if|x|<2**-57*/
    z = x * x;
    p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    r = p / q;
    return pio2_hi - (x - (pio2_lo - x * r));
  } else if (hx < 0) { /* x < -0.5 */
    z = (one + x) * (float)0.5;
    p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    s = sqrtf(z);
    r = p / q;
    w = r * s - pio2_lo;
    return pi - (float)2.0 * (s + w);
  } else { /* x > 0.5 */
    // int32_t idf;
    z = (one - x) * (float)0.5;
    s = sqrtf(z);
    df = s;
    // GET_FLOAT_WORD(idf,df);
    // SET_FLOAT_WORD(df,idf&0xfffff000);
    fp_struct<float> fp_df(df);
    ap_uint<Type_BitWidth<float>::Value> data = fp_df.data() & 0xfffff000;
    fp_struct<float> fp_df_new(data);
    df = fp_df_new.to_ieee();
    c = (z - df * df) / (s + df);
    p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    r = p / q;
    w = r * s + c;
    return (float)2.0 * (df + w);
  }
}

// asin(double) and acos(double) based Apple opensoure libm
// https://opensource.apple.com/source/Libm/Libm-2026/
static volatile const double Infinity = INFINITY;
static volatile const double Tiny = 0x1p-1022;
// double2 represents a number equal to d0 + d1, with |d1| <= 1/2 ULP(d0).
typedef struct {
  double d0, d1;
} double2;

// Return a * b, given |a|, |b| < 2**970.
static inline double2 Mul112(double a, double b) {
  static const double c = 0x1p27 + 1;

  double ap = a * c, bp = b * c, a0 = (a - ap) + ap, b0 = (b - bp) + bp,
         a1 = a - a0, b1 = b - b0, d0 = a * b,
         d1 = a0 * b0 - d0 + a0 * b1 + a1 * b0 + a1 * b1;
  return (double2){d0, d1};
}

// Return a + b with relative error below 2**-103 given |b| < |a|.
static inline double2 Add212RightSmaller(double2 a, double b) {
  double c0 = a.d0 + b, c1 = a.d0 - c0 + b + a.d1, d0 = c0 + c1,
         d1 = c0 - d0 + c1;
  return (double2){d0, d1};
}

/*	Return a + b with relative error below 2**-103 and then rounded to
        double given |b| < |a|.
*/
static inline double Add221RightSmaller(double2 a, double2 b) {
  double c0 = a.d0 + b.d0, c1 = a.d0 - c0 + b.d0 + b.d1 + a.d1, d0 = c0 + c1;
  return d0;
}

/*	Return approximately a * b - 1 given |a|, |b| < 2**970 and a * b is
        very near 1.
*/
static inline double Mul121Special(double a, double2 b) {
  static const double c = 0x1p27 + 1;

  double ap = a * c, bp = b.d0 * c, a0 = (a - ap) + ap, b0 = (b.d0 - bp) + bp,
         a1 = a - a0, b1 = b.d0 - b0,
         m1 = a0 * b0 - 1 + a0 * b1 + a1 * b0 + a1 * b1 + a * b.d1;
  return m1;
}

// Return a * b with relative error below 2**-102 given |a|, |b| < 2**970.
static inline double2 Mul222(double2 a, double2 b) {
  static const double c = 0x1p27 + 1;

  double ap = a.d0 * c, bp = b.d0 * c, a0 = (a.d0 - ap) + ap,
         b0 = (b.d0 - bp) + bp, a1 = a.d0 - a0, b1 = b.d0 - b0,
         m0 = a.d0 * b.d0,
         m1 = a0 * b0 - m0 + a0 * b1 + a1 * b0 + a1 * b1 +
              (a.d0 * b.d1 + a.d1 * b.d0),
         d0 = m0 + m1, d1 = m0 - d0 + m1;
  return (double2){d0, d1};
}

// for acos
// Return a + b given |b| < |a|.
static inline double2 Add112RightSmaller(double a, double b) {
  double d0 = a + b, d1 = b - (d0 - a);
  return (double2){d0, d1};
}

/*	Return a - b with relative error below 2**-103 and then rounded to a
        double given |b| < |a|.
*/
static inline double Sub211RightSmaller(double2 a, double b) {
  double c0 = a.d0 - b, c1 = a.d0 - c0 - b + a.d1, d0 = c0 + c1;
  return d0;
}

/*	Return a - b with relative error below 2**-103 and then rounded to
        double given |b| < |a|.
*/
static inline double Sub221RightSmaller(double2 a, double2 b) {
  double c0 = a.d0 - b.d0, c1 = a.d0 - c0 - b.d0 - b.d1 + a.d1, d0 = c0 + c1;
  return d0;
}

// Return approximately a * b given |a|, |b| < 2**970.
static inline double Mul221(double2 a, double2 b) {
  static const double c = 0x1p27 + 1;

  double ap = a.d0 * c, bp = b.d0 * c, a0 = (a.d0 - ap) + ap,
         b0 = (b.d0 - bp) + bp, a1 = a.d0 - a0, b1 = b.d0 - b0,
         m0 = a.d0 * b.d0,
         m1 = a0 * b0 - m0 + a0 * b1 + a1 * b0 + a1 * b1 +
              (a.d0 * b.d1 + a.d1 * b.d0),
         d0 = m0 + m1;
  return d0;
}
// for acos end

// Return arcsine(x) given |x| <= .5, with the same properties as asin.
static double Center(double x) {
  if (-0x1.7137449123ef5p-26 <= x && x <= +0x1.7137449123ef5p-26)
    return -0x1p-1022 < x && x < +0x1p-1022
               // Generate underflow and inexact and return x.
               ? x - x * x
               // Generate inexact and return x.
               : x * (Tiny + 1);

  static const double p03 = 0.1666666666666558995379880;
  static const double p05 = 0.0750000000029696112392353;
  static const double p07 = 0.0446428568582815922683933;
  static const double p09 = 0.0303819580081956423799529;
  static const double p11 = 0.0223717830666671020710108;
  static const double p13 = 0.0173593516996479249428647;
  static const double p15 = 0.0138885410156894774969889;
  static const double p17 = 0.0121483892822292648695383;
  static const double p19 = 0.0066153165197009078340075;
  static const double p21 = 0.0192942786775238654913582;
  static const double p23 = -0.0158620440988475212803145;
  static const double p25 = 0.0316658385792867081040808;

  // Square x.
  double x2 = x * x;

  return ((((((((((((+p25) * x2 + p23) * x2 + p21) * x2 + p19) * x2 + p17) *
                    x2 +
                p15) *
                   x2 +
               p13) *
                  x2 +
              p11) *
                 x2 +
             p09) *
                x2 +
            p07) *
               x2 +
           p05) *
              x2 +
          p03) *
             x2 * x +
         x;
}

// Return arcsine(x) given .5 < x, with the same properties as asin.
static double Tail(double x) {
  if (1 <= x)
    return 1 == x
               // If x is 1, generate inexact and return Pi/2 rounded down.
               ? 0x3.243f6a8885a308d313198a2e03707344ap-1 + Tiny
               // If x is outside the domain, generate invalid and return NaN.
               : Infinity - Infinity;

  static const double p01 = 0.2145993335526539017488949;
  static const double p02 = -0.0890259194305537131666744;
  static const double p03 = 0.0506659694457588602631748;
  static const double p04 = -0.0331919619444009606270380;
  static const double p05 = 0.0229883479552557203133368;
  static const double p06 = -0.0156746038587246716524035;
  static const double p07 = 0.0097868293573384001221447;
  static const double p08 = -0.0052049731575223952626203;
  static const double p09 = 0.0021912255981979442677477;
  static const double p10 = -0.0006702485124770180942917;
  static const double p11 = 0.0001307564187657962919394;
  static const double p12 = -0.0000121189820098929624806;

  double polynomial =
      ((((((((((((+p12) * x + p11) * x + p10) * x + p09) * x + p08) * x + p07) *
                x +
            p06) *
               x +
           p05) *
              x +
          p04) *
             x +
         p03) *
            x +
        p02) *
           x +
       p01) *
      x;

  static const double2 HalfPi = {0x1.921fb54442d18p+0, 0x1.1a62633145c07p-54},
                       p00 = {-0x1.921fb31e97d96p0, +0x1.eab77149ad27cp-54};

  // Estimate 1 / sqrt(1-x).
  double e = 1 / sqrt(1 - x);

  double2 ex = Mul112(e, 1 - x);     // e * x.
  double e2x = Mul121Special(e, ex); // e**2 * x - 1.

  // Set t0 to an improved approximation of sqrt(1-x) with Newton-Raphson.
  double2 t0 = Add212RightSmaller(ex, ex.d0 * -.5 * e2x);

  // Calculate pi/2 + sqrt(1-x) * p(x).
  return Add221RightSmaller(HalfPi,
                            Mul222(t0, Add212RightSmaller(p00, polynomial)));
}

// Return arccosine(x) given |x| <= .4, with the same properties as acos.
static double Center_acos(double x) {
  static const double HalfPi = 0x3.243f6a8885a308d313198a2e03707344ap-1;

  /*	If x is small, generate inexact and return pi/2.  We must do this
          for very small x to avoid underflow when x is squared.
  */
  if (-0x1.8d313198a2e03p-53 <= x && x <= +0x1.8d313198a2e03p-53)
    return HalfPi + Tiny;

  static const double p03 = +.1666666666666251331848183;
  static const double p05 = +.7500000000967090522908427e-1;
  static const double p07 = +.4464285630020156622713320e-1;
  static const double p09 = +.3038198238851575770651788e-1;
  static const double p11 = +.2237115216935265224962544e-1;
  static const double p13 = +.1736953298172084894468665e-1;
  static const double p15 = +.1378527665685754961528021e-1;
  static const double p17 = +.1277870997666947910124296e-1;
  static const double p19 = +.4673473145155259234911049e-2;
  static const double p21 = +.1951350766744288383625404e-1;

  // Square x.
  double x2 = x * x;

  return HalfPi -
         (((((((((((+p21) * x2 + p19) * x2 + p17) * x2 + p15) * x2 + p13) * x2 +
               p11) *
                  x2 +
              p09) *
                 x2 +
             p07) *
                x2 +
            p05) *
               x2 +
           p03) *
              x2 * x +
          x);
}

// Return arccosine(x) given .4 <= |x| <= .6, with the same properties as acos.
static double Gap_acos(double x) {
  static const double p03 = +.1666666544260252354339083;
  static const double p05 = +.7500058936188719422797382e-1;
  static const double p07 = +.4462999611462664666589096e-1;
  static const double p09 = +.3054999576148835435598555e-1;
  static const double p11 = +.2090953485621966528477317e-1;
  static const double p13 = +.2626916834046217573905021e-1;
  static const double p15 = -.2496419961469848084029243e-1;
  static const double p17 = +.1336320190979444903198404;
  static const double p19 = -.2609082745402891409913617;
  static const double p21 = +.4154485118940996442799104;
  static const double p23 = -.3718481677216955169129325;
  static const double p25 = +.1791132167840254903934055;

  // Square x.
  double x2 = x * x;

  double poly =
      ((((((((((((+p25) * x2 + p23) * x2 + p21) * x2 + p19) * x2 + p17) * x2 +
             p15) *
                x2 +
            p13) *
               x2 +
           p11) *
              x2 +
          p09) *
             x2 +
         p07) *
            x2 +
        p05) *
           x2 +
       p03) *
      x2 * x;

  static const double2 HalfPi = {0x1.921fb54442d18p+0, 0x1.1a62633145c07p-54};
  return Sub221RightSmaller(HalfPi, Add112RightSmaller(x, poly));
}

// Return arccosine(x) given +.6 < x, with the same properties as acos.
static double pTail_acos(double x) {
  if (1 <= x)
    return 1 == x
               // If x is 1, return zero.
               ? 0
               // If x is outside the domain, generate invalid and return NaN.
               : Infinity - Infinity;

  static const double p01 = -.2145900291823555067724496;
  static const double p02 = +.8895931658903454714161991e-1;
  static const double p03 = -.5037781062999805015401690e-1;
  static const double p04 = +.3235271184788013959507217e-1;
  static const double p05 = -.2125492340970560944012545e-1;
  static const double p06 = +.1307107321829037349021838e-1;
  static const double p07 = -.6921689208385164161272068e-2;
  static const double p08 = +.2912114685670939037614086e-2;
  static const double p09 = -.8899459104279910976564839e-3;
  static const double p10 = +.1730883544880830573920551e-3;
  static const double p11 = -.1594866672026418356538789e-4;

  double t0 =
      (((((((((((+p11) * x + p10) * x + p09) * x + p08) * x + p07) * x + p06) *
               x +
           p05) *
              x +
          p04) *
             x +
         p03) *
            x +
        p02) *
           x +
       p01) *
      x;

  static const double2 p00 = {0x1.921fa926d2f24p0, +0x1.b4a23d0ecbb40p-59};
  /*	p00.d1 might not be needed.  However, omitting it brings the
          sampled error to .872 ULP.  We would need to prove this is okay.
  */

  // Estimate square root to double precision.
  double e = 1 / sqrt(1 - x);

  // Refine estimate using Newton-Raphson.
  double2 ex = Mul112(e, 1 - x);
  double e2x = Mul121Special(e, ex);
  double2 t1 = Add212RightSmaller(ex, ex.d0 * -.5 * e2x);

  // Return sqrt(1-x) * (t0 + p00).
  return Mul221(t1, Add212RightSmaller(p00, t0));
}

// Return arccosine(x) given x < -.6, with the same properties as acos.
static double nTail_acos(double x) {
  if (x <= -1)
    return -1 == x
               // If x is -1, generate inexact and return pi rounded toward
               // zero.
               ? 0x3.243f6a8885a308d313198a2e03707344ap0 + Tiny
               // If x is outside the domain, generate invalid and return NaN.
               : Infinity - Infinity;

  static const double p00 = +1.5707956513160834076561054;
  static const double p01 = +.2145907003920708442108238;
  static const double p02 = +.8896369437915166409934895e-1;
  static const double p03 = +.5039488847935731213671556e-1;
  static const double p04 = +.3239698582040400391437898e-1;
  static const double p05 = +.2133501549935443220662813e-1;
  static const double p06 = +.1317423797769298396461497e-1;
  static const double p07 = +.7016307696008088925432394e-2;
  static const double p08 = +.2972670140131377611481662e-2;
  static const double p09 = +.9157019394367251664320071e-3;
  static const double p10 = +.1796407754831532447333023e-3;
  static const double p11 = +.1670402962434266380655447e-4;

  double poly =
      sqrt(1 + x) *
      ((((((((((((+p11) * x + p10) * x + p09) * x + p08) * x + p07) * x + p06) *
                x +
            p05) *
               x +
           p04) *
              x +
          p03) *
             x +
         p02) *
            x +
        p01) *
           x +
       p00);

  static const double2 Pi = {0x1.921fb54442d18p+1, 0x1.1a62633145c07p-53};
  return Sub211RightSmaller(Pi, poly);
}

static double asin(double x) {
  // flush subnormalnormal
  if (fpclassify(x) == FP_SUBNORMAL)
    x = 0.0;

  if ((x <= 0.5) && (x >= -0.5)) {
    return Center(x);
  } else {
    double x1;
    if (x < 0)
      x1 = -x;
    else
      x1 = x;
    double r = Tail(x1);
    if (x < 0)
      r = -r;

    return r;
  }
}

// See documentation above.
static double acos(double x) {
  if (x < -.4)
    if (x < -.6)
      return nTail_acos(x);
    else
      return Gap_acos(x);
  else if (x <= +.4)
    return Center_acos(x);
  else if (x <= +.6)
    return Gap_acos(x);
  else
    return pTail_acos(x);
}

static half asin(half x) { return asin((float)x); }

static half acos(half x) { return acos((float)x); }

static float asinf(float x) { return asin(x); }

static float acosf(float x) { return acos(x); }

} // namespace asin_acos_approx
#endif //
