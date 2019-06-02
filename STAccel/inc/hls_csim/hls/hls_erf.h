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
 * @file hls_erf.h
 * This file contains error functions, mainly based on ieee754/ implementation
 * except some optimization for fpga. This implementation will occupy huge logic
 * when doesn't share double operations (multi, add, div), this need further
 * optimize or adopt new approximation method suite to fpga
 */

#ifndef X_HLS_MATH_H
#error hls_erf.h is not intended to be included directly.  Please include hls_math.h instead.
#endif

#ifndef X_HLS_ERF_H
#define X_HLS_ERF_H

/* double erf(double x)
 * double erfc(double x)
 *			     x
 *		      2      |\
 *     erf(x)  =  ---------  | exp(-t*t)dt
 *	 	   sqrt(pi) \|
 *			     0
 *
 *     erfc(x) =  1-erf(x)
 *  Note that
 *		erf(-x) = -erf(x)
 *		erfc(-x) = 2 - erfc(x)
 *
 * Method:
 *	1. For |x| in [0, 0.84375]
 *	    erf(x)  = x + x*R(x^2)
 *          erfc(x) = 1 - erf(x)           if x in [-.84375,0.25]
 *                  = 0.5 + ((0.5-x)-x*R)  if x in [0.25,0.84375]
 *	   where R = P/Q where P is an odd poly of degree 8 and
 *	   Q is an odd poly of degree 10.
 *						 -57.90
 *			| R - (erf(x)-x)/x | <= 2
 *
 *
 *	   Remark. The formula is derived by noting
 *          erf(x) = (2/sqrt(pi))*(x - x^3/3 + x^5/10 - x^7/42 + ....)
 *	   and that
 *          2/sqrt(pi) = 1.128379167095512573896158903121545171688
 *	   is close to one. The interval is chosen because the fix
 *	   point of erf(x) is near 0.6174 (i.e., erf(x)=x when x is
 *	   near 0.6174), and by some experiment, 0.84375 is chosen to
 * 	   guarantee the error is less than one ulp for erf.
 *
 *      2. For |x| in [0.84375,1.25], let s = |x| - 1, and
 *         c = 0.84506291151 rounded to single (24 bits)
 *         	erf(x)  = sign(x) * (c  + P1(s)/Q1(s))
 *         	erfc(x) = (1-c)  - P1(s)/Q1(s) if x > 0
 *			  1+(c+P1(s)/Q1(s))    if x < 0
 *         	|P1/Q1 - (erf(|x|)-c)| <= 2**-59.06
 *	   Remark: here we use the taylor series expansion at x=1.
 *		erf(1+s) = erf(1) + s*Poly(s)
 *			 = 0.845.. + P1(s)/Q1(s)
 *	   That is, we use rational approximation to approximate
 *			erf(1+s) - (c = (single)0.84506291151)
 *	   Note that |P1/Q1|< 0.078 for x in [0.84375,1.25]
 *	   where
 *		P1(s) = degree 6 poly in s
 *		Q1(s) = degree 6 poly in s
 *
 *      3. For x in [1.25,1/0.35(~2.857143)],
 *         	erfc(x) = (1/x)*exp(-x*x-0.5625+R1/S1)
 *         	erf(x)  = 1 - erfc(x)
 *	   where
 *		R1(z) = degree 7 poly in z, (z=1/x^2)
 *		S1(z) = degree 8 poly in z
 *
 *      4. For x in [1/0.35,28]
 *         	erfc(x) = (1/x)*exp(-x*x-0.5625+R2/S2) if x > 0
 *			= 2.0 - (1/x)*exp(-x*x-0.5625+R2/S2) if -6<x<0
 *			= 2.0 - tiny		(if x <= -6)
 *         	erf(x)  = sign(x)*(1.0 - erfc(x)) if x < 6, else
 *         	erf(x)  = sign(x)*(1.0 - tiny)
 *	   where
 *		R2(z) = degree 6 poly in z, (z=1/x^2)
 *		S2(z) = degree 7 poly in z
 *
 *      Note1:
 *	   To compute exp(-x*x-0.5625+R/S), let s be a single
 *	   precision number and s := x; then
 *		-x*x = -s*s + (s-x)*(s+x)
 *	        exp(-x*x-0.5626+R/S) =
 *			exp(-s*s-0.5625)*exp((s-x)*(s+x)+R/S);
 *      Note2:
 *	   Here 4 and 5 make use of the asymptotic series
 *			  exp(-x*x)
 *		erfc(x) ~ ---------- * ( 1 + Poly(1/x^2) )
 *			  x*sqrt(pi)
 *	   We use rational approximation to approximate
 *      	g(s)=f(1/x^2) = log(erfc(x)*x) - x*x + 0.5625
 *	   Here is the error bound for R1/S1 and R2/S2
 *      	|R1/S1 - f(x)|  < 2**(-62.57)
 *      	|R2/S2 - f(x)|  < 2**(-61.52)
 *
 *      5. For inf > x >= 28
 *         	erf(x)  = sign(x) *(1 - tiny)  (raise inexact)
 *         	erfc(x) = tiny*tiny (raise underflow) if x > 0
 *			= 2 - tiny if x<0
 *
 *      7. Special case:
 *         	erf(0)  = 0, erf(inf)  = 1, erf(-inf) = -1,
 *         	erfc(0) = 1, erfc(inf) = 0, erfc(-inf) = 2,
 *	   	erfc/erf(NaN) is NaN
 */

#include <stdint.h>

namespace erf_approx {

namespace {
template <typename T> class coeff {};
template <> class coeff<double> {
public:
  static const double tiny;
  static const double one_over_two;
  static const double one;
  static const double two;
  static const double erx;
  static const double efx;
  static const double efx8;
  static const double pp[5];
  static const double qq[6];
  static const double pa[7];
  static const double qa[7];
  static const double ra[8];
  static const double sa[9];
  static const double rb[7];
  static const double sb[8];
};
const double coeff<double>::tiny = 1e-300;
const double coeff<double>::one_over_two =
    5.00000000000000000000e-01; /* 0x3FE00000, 0x00000000 */
const double coeff<double>::one =
    1.00000000000000000000e+00; /* 0x3FF00000, 0x00000000 */
const double coeff<double>::two =
    2.00000000000000000000e+00; /* 0x40000000, 0x00000000 */
/* c = (float)0.84506291151 */
const double coeff<double>::erx =
    8.45062911510467529297e-01; /* 0x3FEB0AC1, 0x60000000 */
/*
 * Coefficients for approximation to  erf on [0,0.84375]
 */
const double coeff<double>::efx =
    1.28379167095512586316e-01; /* 0x3FC06EBA, 0x8214DB69 */
const double coeff<double>::efx8 =
    1.02703333676410069053e+00; /* 0x3FF06EBA, 0x8214DB69 */
const double coeff<double>::pp[] = {
    1.28379167095512558561e-01,   /* 0x3FC06EBA, 0x8214DB68 */
    -3.25042107247001499370e-01,  /* 0xBFD4CD7D, 0x691CB913 */
    -2.84817495755985104766e-02,  /* 0xBF9D2A51, 0xDBD7194F */
    -5.77027029648944159157e-03,  /* 0xBF77A291, 0x236668E4 */
    -2.37630166566501626084e-05}; /* 0xBEF8EAD6, 0x120016AC */
const double coeff<double>::qq[] = {
    0.0,
    3.97917223959155352819e-01,   /* 0x3FD97779, 0xCDDADC09 */
    6.50222499887672944485e-02,   /* 0x3FB0A54C, 0x5536CEBA */
    5.08130628187576562776e-03,   /* 0x3F74D022, 0xC4D36B0F */
    1.32494738004321644526e-04,   /* 0x3F215DC9, 0x221C1A10 */
    -3.96022827877536812320e-06}; /* 0xBED09C43, 0x42A26120 */
/*
 * Coefficients for approximation to  erf  in [0.84375,1.25]
 */
const double coeff<double>::pa[] = {
    -2.36211856075265944077e-03,  /* 0xBF6359B8, 0xBEF77538 */
    4.14856118683748331666e-01,   /* 0x3FDA8D00, 0xAD92B34D */
    -3.72207876035701323847e-01,  /* 0xBFD7D240, 0xFBB8C3F1 */
    3.18346619901161753674e-01,   /* 0x3FD45FCA, 0x805120E4 */
    -1.10894694282396677476e-01,  /* 0xBFBC6398, 0x3D3E28EC */
    3.54783043256182359371e-02,   /* 0x3FA22A36, 0x599795EB */
    -2.16637559486879084300e-03}; /* 0xBF61BF38, 0x0A96073F */
const double coeff<double>::qa[] = {
    0.0,
    1.06420880400844228286e-01,  /* 0x3FBB3E66, 0x18EEE323 */
    5.40397917702171048937e-01,  /* 0x3FE14AF0, 0x92EB6F33 */
    7.18286544141962662868e-02,  /* 0x3FB2635C, 0xD99FE9A7 */
    1.26171219808761642112e-01,  /* 0x3FC02660, 0xE763351F */
    1.36370839120290507362e-02,  /* 0x3F8BEDC2, 0x6B51DD1C */
    1.19844998467991074170e-02}; /* 0x3F888B54, 0x5735151D */
/*
 * Coefficients for approximation to  erfc in [1.25,1/0.35]
 */
const double coeff<double>::ra[] = {
    -9.86494403484714822705e-03,  /* 0xBF843412, 0x600D6435 */
    -6.93858572707181764372e-01,  /* 0xBFE63416, 0xE4BA7360 */
    -1.05586262253232909814e+01,  /* 0xC0251E04, 0x41B0E726 */
    -6.23753324503260060396e+01,  /* 0xC04F300A, 0xE4CBA38D */
    -1.62396669462573470355e+02,  /* 0xC0644CB1, 0x84282266 */
    -1.84605092906711035994e+02,  /* 0xC067135C, 0xEBCCABB2 */
    -8.12874355063065934246e+01,  /* 0xC0545265, 0x57E4D2F2 */
    -9.81432934416914548592e+00}; /* 0xC023A0EF, 0xC69AC25C */
const double coeff<double>::sa[] = {
    0.0,
    1.96512716674392571292e+01,   /* 0x4033A6B9, 0xBD707687 */
    1.37657754143519042600e+02,   /* 0x4061350C, 0x526AE721 */
    4.34565877475229228821e+02,   /* 0x407B290D, 0xD58A1A71 */
    6.45387271733267880336e+02,   /* 0x40842B19, 0x21EC2868 */
    4.29008140027567833386e+02,   /* 0x407AD021, 0x57700314 */
    1.08635005541779435134e+02,   /* 0x405B28A3, 0xEE48AE2C */
    6.57024977031928170135e+00,   /* 0x401A47EF, 0x8E484A93 */
    -6.04244152148580987438e-02}; /* 0xBFAEEFF2, 0xEE749A62 */
/*
 * Coefficients for approximation to  erfc in [1/.35,28]
 */
const double coeff<double>::rb[] = {
    -9.86494292470009928597e-03,  /* 0xBF843412, 0x39E86F4A */
    -7.99283237680523006574e-01,  /* 0xBFE993BA, 0x70C285DE */
    -1.77579549177547519889e+01,  /* 0xC031C209, 0x555F995A */
    -1.60636384855821916062e+02,  /* 0xC064145D, 0x43C5ED98 */
    -6.37566443368389627722e+02,  /* 0xC083EC88, 0x1375F228 */
    -1.02509513161107724954e+03,  /* 0xC0900461, 0x6A2E5992 */
    -4.83519191608651397019e+02}; /* 0xC07E384E, 0x9BDC383F */
const double coeff<double>::sb[] = {
    0.0,
    3.03380607434824582924e+01,   /* 0x403E568B, 0x261D5190 */
    3.25792512996573918826e+02,   /* 0x40745CAE, 0x221B9F0A */
    1.53672958608443695994e+03,   /* 0x409802EB, 0x189D5118 */
    3.19985821950859553908e+03,   /* 0x40A8FFB7, 0x688C246A */
    2.55305040643316442583e+03,   /* 0x40A3F219, 0xCEDF3BE6 */
    4.74528541206955367215e+02,   /* 0x407DA874, 0xE79FE763 */
    -2.24409524465858183362e+01}; /* 0xC03670E2, 0x42712D62 */

template <> class coeff<float> {
public:
  static const float tiny;
  static const float one_over_two;
  static const float one;
  static const float two;
  static const float erx;
  static const float efx;
  static const float efx8;
  static const float pp[5];
  static const float qq[6];
  static const float pa[7];
  static const float qa[7];
  static const float ra[8];
  static const float sa[9];
  static const float rb[7];
  static const float sb[8];
};
const float coeff<float>::tiny = 1e-30;
const float coeff<float>::one_over_two = 5.0000000000e-01; /* 0x3F000000 */
const float coeff<float>::one = 1.0000000000e+00;          /* 0x3F800000 */
const float coeff<float>::two = 2.0000000000e+00;          /* 0x40000000 */
/* c = (float)0.84506291151 */
const float coeff<float>::erx = 8.4506291151e-01; /* 0x3f58560b */
/*
 * Coefficients for approximation to  erf on [0,0.84375]
 */
const float coeff<float>::efx = 1.2837916613e-01;     /* 0x3e0375d4 */
const float coeff<float>::efx8 = 1.0270333290e+00;    /* 0x3f8375d4 */
const float coeff<float>::pp[] = {1.2837916613e-01,   /* 0x3e0375d4 */
                                  -3.2504209876e-01,  /* 0xbea66beb */
                                  -2.8481749818e-02,  /* 0xbce9528f */
                                  -5.7702702470e-03,  /* 0xbbbd1489 */
                                  -2.3763017452e-05}; /* 0xb7c756b1 */
const float coeff<float>::qq[] = {0.0,
                                  3.9791721106e-01,   /* 0x3ecbbbce */
                                  6.5022252500e-02,   /* 0x3d852a63 */
                                  5.0813062117e-03,   /* 0x3ba68116 */
                                  1.3249473704e-04,   /* 0x390aee49 */
                                  -3.9602282413e-06}; /* 0xb684e21a */
/*
 * Coefficients for approximation to  erf  in [0.84375,1.25]
 */
const float coeff<float>::pa[] = {-2.3621185683e-03,  /* 0xbb1acdc6 */
                                  4.1485610604e-01,   /* 0x3ed46805 */
                                  -3.7220788002e-01,  /* 0xbebe9208 */
                                  3.1834661961e-01,   /* 0x3ea2fe54 */
                                  -1.1089469492e-01,  /* 0xbde31cc2 */
                                  3.5478305072e-02,   /* 0x3d1151b3 */
                                  -2.1663755178e-03}; /* 0xbb0df9c0 */
const float coeff<float>::qa[] = {0.0,
                                  1.0642088205e-01,  /* 0x3dd9f331 */
                                  5.4039794207e-01,  /* 0x3f0a5785 */
                                  7.1828655899e-02,  /* 0x3d931ae7 */
                                  1.2617121637e-01,  /* 0x3e013307 */
                                  1.3637083583e-02,  /* 0x3c5f6e13 */
                                  1.1984500103e-02}; /* 0x3c445aa3 */
/*
 * Coefficients for approximation to  erfc in [1.25,1/0.35]
 */
const float coeff<float>::ra[] = {-9.8649440333e-03,  /* 0xbc21a093 */
                                  -6.9385856390e-01,  /* 0xbf31a0b7 */
                                  -1.0558626175e+01,  /* 0xc128f022 */
                                  -6.2375331879e+01,  /* 0xc2798057 */
                                  -1.6239666748e+02,  /* 0xc322658c */
                                  -1.8460508728e+02,  /* 0xc3389ae7 */
                                  -8.1287437439e+01,  /* 0xc2a2932b */
                                  -9.8143291473e+00}; /* 0xc11d077e */
const float coeff<float>::sa[] = {0.0,
                                  1.9651271820e+01,   /* 0x419d35ce */
                                  1.3765776062e+02,   /* 0x4309a863 */
                                  4.3456588745e+02,   /* 0x43d9486f */
                                  6.4538726807e+02,   /* 0x442158c9 */
                                  4.2900814819e+02,   /* 0x43d6810b */
                                  1.0863500214e+02,   /* 0x42d9451f */
                                  6.5702495575e+00,   /* 0x40d23f7c */
                                  -6.0424413532e-02}; /* 0xbd777f97 */
/*
 * Coefficients for approximation to  erfc in [1/.35,28]
 */
const float coeff<float>::rb[] = {-9.8649431020e-03,  /* 0xbc21a092 */
                                  -7.9928326607e-01,  /* 0xbf4c9dd4 */
                                  -1.7757955551e+01,  /* 0xc18e104b */
                                  -1.6063638306e+02,  /* 0xc320a2ea */
                                  -6.3756646729e+02,  /* 0xc41f6441 */
                                  -1.0250950928e+03,  /* 0xc480230b */
                                  -4.8351919556e+02}; /* 0xc3f1c275 */
const float coeff<float>::sb[] = {0.0,
                                  3.0338060379e+01,   /* 0x41f2b459 */
                                  3.2579251099e+02,   /* 0x43a2e571 */
                                  1.5367296143e+03,   /* 0x44c01759 */
                                  3.1998581543e+03,   /* 0x4547fdbb */
                                  2.5530502930e+03,   /* 0x451f90ce */
                                  4.7452853394e+02,   /* 0x43ed43a7 */
                                  -2.2440952301e+01}; /* 0xc1b38712 */

template <typename T> class erf_traits {};
template <> class erf_traits<float> {
public:
  static const uint32_t mask;
  static const uint32_t segment[10];
};
const uint32_t erf_traits<float>::mask = 0xfffff000;
const uint32_t erf_traits<float>::segment[10] = {
    0x7fffffff, 0x7f800000, 0x3f580000, 0x31800000, 0x04000000,
    0x3fa00000, 0x40c00000, 0x4036DB6E, 31,         0};
template <> class erf_traits<double> {
public:
  static const uint64_t mask;
  static const uint32_t segment[10];
};
const uint64_t erf_traits<double>::mask = 0xffffffff00000000;
const uint32_t erf_traits<double>::segment[10] = {
    0x7fffffff, 0x7ff00000, 0x3feb0000, 0x3e300000, 0x00800000,
    0x3ff40000, 0x40180000, 0x4006DB6E, 63,         32};

template <typename T> class erfc_traits {};
template <> class erfc_traits<float> {
public:
  static const uint32_t segment[11];
};
const uint32_t erfc_traits<float>::segment[11] = {
    0x7fffffff, 0x7f800000, 0x3f580000, 0x23800000, 0x3e800000, 0x3fa00000,
    0x41e00000, 0x4036DB6D, 0x40c00000, 31,         0};
template <> class erfc_traits<double> {
public:
  static const uint32_t segment[11];
};
const uint32_t erfc_traits<double>::segment[11] = {
    0x7fffffff, 0x7ff00000, 0x3feb0000, 0x3c700000, 0x3fd00000, 0x3ff40000,
    0x403c0000, 0x4006DB6D, 0x40180000, 63,         32};
} // anonymous namespace

#define tiny (coeff<T>::tiny)
#define one_over_two (coeff<T>::one_over_two)
#define one (coeff<T>::one)
#define two (coeff<T>::two)
#define erx (coeff<T>::erx)
#define efx (coeff<T>::efx)
#define efx8 (coeff<T>::efx8)
#define pp coeff<T>::pp
#define qq coeff<T>::qq
#define pa coeff<T>::pa
#define qa coeff<T>::qa
#define ra coeff<T>::ra
#define sa coeff<T>::sa
#define rb coeff<T>::rb
#define sb coeff<T>::sb
#define seg erf_traits<T>::segment
#define segc erfc_traits<T>::segment
#define mask erf_traits<T>::mask

template <typename T> T generic_erf(T x) {
  int32_t hx;
  uint32_t ix;
  T R, S, P, Q, s, y, z, r;
  // GET_HIGH_WORD(hx,x);
  fp_struct<T> fp_x(x);
  ap_uint<32> tmp = fp_x.data()(seg[8], seg[9]);
  hx = tmp.to_int();
  ix = hx & seg[0];

  if (FP_SUBNORMAL == ::hls::fpclassify(x)) {
    return 0;
  }

  if (ix >= seg[1]) { /* erf(nan)=nan */
                      // i = ((uint32_t)hx>>31)<<1;
    if (hx > 0)
      return (T)1.0 + one / x; /* erf(+-inf)=+-1 */
    else
      return one / x - (T)1.0;
  }

  if (ix < seg[2]) { /* |x|<0.84375 */
    T r1, r2, s1, s2, s3, z2, z4;
    if (ix < seg[3]) { /* |x|<2**-28 */
      if (ix < seg[2])
        return ((T)0.125) * ((T)8.0 * x + efx8 * x); /*avoid underflow */
      return x + efx * x;
    }
    z = x * x;
#ifdef DO_NOT_USE_THIS
    r = pp0 + z * (pp1 + z * (pp2 + z * (pp3 + z * pp4)));
    s = one + z * (qq1 + z * (qq2 + z * (qq3 + z * (qq4 + z * qq5))));
#else
    r1 = pp[0] + z * pp[1];
    z2 = z * z;
    r2 = pp[2] + z * pp[3];
    z4 = z2 * z2;
    s1 = one + z * qq[1];
    s2 = qq[2] + z * qq[3];
    s3 = qq[4] + z * qq[5];
    r = r1 + z2 * r2 + z4 * pp[4];
    s = s1 + z2 * s2 + z4 * s3;
#endif
    y = r / s;
    return x + x * y;
  }
  if (ix < seg[5]) { /* 0.84375 <= |x| < 1.25 */
    T s2, s4, s6, P1, P2, P3, P4, Q1, Q2, Q3, Q4;
    s = fabs(x) - one;
#ifdef DO_NOT_USE_THIS
    P = pa0 +
        s * (pa1 + s * (pa2 + s * (pa3 + s * (pa4 + s * (pa5 + s * pa6)))));
    Q = one +
        s * (qa1 + s * (qa2 + s * (qa3 + s * (qa4 + s * (qa5 + s * qa6)))));
#else
    P1 = pa[0] + s * pa[1];
    s2 = s * s;
    Q1 = one + s * qa[1];
    s4 = s2 * s2;
    P2 = pa[2] + s * pa[3];
    s6 = s4 * s2;
    Q2 = qa[2] + s * qa[3];
    P3 = pa[4] + s * pa[5];
    Q3 = qa[4] + s * qa[5];
    P4 = pa[6];
    Q4 = qa[6];
    P = P1 + s2 * P2 + s4 * P3 + s6 * P4;
    Q = Q1 + s2 * Q2 + s4 * Q3 + s6 * Q4;
#endif
    if (hx >= 0)
      return erx + P / Q;
    else
      return -erx - P / Q;
  }
  if (ix >= seg[6]) { /* inf>|x|>=6 */
    if (hx >= 0)
      return one - tiny;
    else
      return tiny - one;
  }
  x = fabs(x);
  s = one / (x * x);
  if (ix < seg[7]) { /* |x| < 1/0.35 */
#ifdef DO_NOT_USE_THIS
    R = ra0 +
        s * (ra1 +
             s * (ra2 +
                  s * (ra3 + s * (ra4 + s * (ra5 + s * (ra6 + s * ra7))))));
    S = one +
        s * (sa1 +
             s * (sa2 +
                  s * (sa3 +
                       s * (sa4 +
                            s * (sa5 + s * (sa6 + s * (sa7 + s * sa8)))))));
#else
    T R1, R2, R3, R4, S1, S2, S3, S4, s2, s4, s6, s8;
    R1 = ra[0] + s * ra[1];
    s2 = s * s;
    S1 = one + s * sa[1];
    s4 = s2 * s2;
    R2 = ra[2] + s * ra[3];
    s6 = s4 * s2;
    S2 = sa[2] + s * sa[3];
    s8 = s4 * s4;
    R3 = ra[4] + s * ra[5];
    S3 = sa[4] + s * sa[5];
    R4 = ra[6] + s * ra[7];
    S4 = sa[6] + s * sa[7];
    R = R1 + s2 * R2 + s4 * R3 + s6 * R4;
    S = S1 + s2 * S2 + s4 * S3 + s6 * S4 + s8 * sa[8];
#endif
  } else { /* |x| >= 1/0.35 */
#ifdef DO_NOT_USE_THIS
    R = rb0 +
        s * (rb1 + s * (rb2 + s * (rb3 + s * (rb4 + s * (rb5 + s * rb6)))));
    S = one +
        s * (sb1 +
             s * (sb2 +
                  s * (sb3 + s * (sb4 + s * (sb5 + s * (sb6 + s * sb7))))));
#else
    T R1, R2, R3, S1, S2, S3, S4, s2, s4, s6;
    R1 = rb[0] + s * rb[1];
    s2 = s * s;
    S1 = one + s * sb[1];
    s4 = s2 * s2;
    R2 = rb[2] + s * rb[3];
    s6 = s4 * s2;
    S2 = sb[2] + s * sb[3];
    R3 = rb[4] + s * rb[5];
    S3 = sb[4] + s * sb[5];
    S4 = sb[6] + s * sb[7];
    R = R1 + s2 * R2 + s4 * R3 + s6 * rb[6];
    S = S1 + s2 * S2 + s4 * S3 + s6 * S4;
#endif
  }
  z = x;
  // SET_LOW_WORD(z,0);
  fp_struct<T> fp_z(z);
  ap_uint<Type_BitWidth<T>::Value> data = fp_z.data() & mask;
  fp_struct<T> fp_z_new(data);
  z = fp_z_new.to_ieee();
  // r  =  __ieee754_exp(-z*z-0.5625)*__ieee754_exp((z-x)*(z+x)+R/S);
  r = ::hls::exp(-z * z - (T)0.5625) * ::hls::exp((z - x) * (z + x) + R / S);
  // r  =  ::hls::exp(-x*x-(T)0.5625+R/S); // also work
  if (hx >= 0)
    return one - r / x;
  else
    return r / x - one;
}

template <typename T> T generic_erfc(T x) {
  int32_t hx;
  uint32_t ix;
  T R, S, P, Q, s, y, z, r;
  // GET_HIGH_WORD(hx,x);
  fp_struct<T> fp_x(x);
  ap_uint<32> tmp = fp_x.data()(segc[9], segc[10]);
  hx = tmp.to_int();
  ix = hx & segc[0];
  if (ix >= segc[1]) { /* erfc(nan)=nan */
                       /* erfc(+-inf)=0,2 */
    if (hx > 0)
      return one / x;
    else
      return two + one / x;
    // return (T)(((uint32_t)hx>>31)<<1)+one/x;
  }

  if (ix < segc[2]) { /* |x|<0.84375 */
    T r1, r2, s1, s2, s3, z2, z4;
    if (ix < segc[3]) /* |x|<2**-56 */
      return one - x;
    z = x * x;
#ifdef DO_NOT_USE_THIS
    r = pp0 + z * (pp1 + z * (pp2 + z * (pp3 + z * pp4)));
    s = one + z * (qq1 + z * (qq2 + z * (qq3 + z * (qq4 + z * qq5))));
#else
    r1 = pp[0] + z * pp[1];
    z2 = z * z;
    r2 = pp[2] + z * pp[3];
    z4 = z2 * z2;
    s1 = one + z * qq[1];
    s2 = qq[2] + z * qq[3];
    s3 = qq[4] + z * qq[5];
    r = r1 + z2 * r2 + z4 * pp[4];
    s = s1 + z2 * s2 + z4 * s3;
#endif
    y = r / s;
    if (ix < segc[4]) { /* x<1/4 */
      return one - (x + x * y);
    } else {
      r = x * y;
      r += (x - one_over_two);
      return one_over_two - r;
    }
  }
  if (ix < segc[5]) { /* 0.84375 <= |x| < 1.25 */
    T s2, s4, s6, P1, P2, P3, P4, Q1, Q2, Q3, Q4;
    s = fabs(x) - one;
#ifdef DO_NOT_USE_THIS
    P = pa0 +
        s * (pa1 + s * (pa2 + s * (pa3 + s * (pa4 + s * (pa5 + s * pa6)))));
    Q = one +
        s * (qa1 + s * (qa2 + s * (qa3 + s * (qa4 + s * (qa5 + s * qa6)))));
#else
    P1 = pa[0] + s * pa[1];
    s2 = s * s;
    Q1 = one + s * qa[1];
    s4 = s2 * s2;
    P2 = pa[2] + s * pa[3];
    s6 = s4 * s2;
    Q2 = qa[2] + s * qa[3];
    P3 = pa[4] + s * pa[5];
    Q3 = qa[4] + s * qa[5];
    P4 = pa[6];
    Q4 = qa[6];
    P = P1 + s2 * P2 + s4 * P3 + s6 * P4;
    Q = Q1 + s2 * Q2 + s4 * Q3 + s6 * Q4;
#endif
    if (hx >= 0) {
      z = one - erx;
      return z - P / Q;
    } else {
      z = erx + P / Q;
      return one + z;
    }
  }
  if (ix < segc[6]) { /* |x|<28 */
    x = fabs(x);
    s = one / (x * x);
    if (ix < segc[7]) { /* |x| < 1/.35 ~ 2.857143*/
#ifdef DO_NOT_USE_THIS
      R = ra0 +
          s * (ra1 +
               s * (ra2 +
                    s * (ra3 + s * (ra4 + s * (ra5 + s * (ra6 + s * ra7))))));
      S = one +
          s * (sa1 +
               s * (sa2 +
                    s * (sa3 +
                         s * (sa4 +
                              s * (sa5 + s * (sa6 + s * (sa7 + s * sa8)))))));
#else
      T R1, R2, R3, R4, S1, S2, S3, S4, s2, s4, s6, s8;
      R1 = ra[0] + s * ra[1];
      s2 = s * s;
      S1 = one + s * sa[1];
      s4 = s2 * s2;
      R2 = ra[2] + s * ra[3];
      s6 = s4 * s2;
      S2 = sa[2] + s * sa[3];
      s8 = s4 * s4;
      R3 = ra[4] + s * ra[5];
      S3 = sa[4] + s * sa[5];
      R4 = ra[6] + s * ra[7];
      S4 = sa[6] + s * sa[7];
      R = R1 + s2 * R2 + s4 * R3 + s6 * R4;
      S = S1 + s2 * S2 + s4 * S3 + s6 * S4 + s8 * sa[8];
#endif
    } else { /* |x| >= 1/.35 ~ 2.857143 */
      T R1, R2, R3, S1, S2, S3, S4, s2, s4, s6;
      if (hx < 0 && ix >= segc[8])
        return two - tiny; /* x < -6 */
#ifdef DO_NOT_USE_THIS
      R = rb0 +
          s * (rb1 + s * (rb2 + s * (rb3 + s * (rb4 + s * (rb5 + s * rb6)))));
      S = one +
          s * (sb1 +
               s * (sb2 +
                    s * (sb3 + s * (sb4 + s * (sb5 + s * (sb6 + s * sb7))))));
#else
      R1 = rb[0] + s * rb[1];
      s2 = s * s;
      S1 = one + s * sb[1];
      s4 = s2 * s2;
      R2 = rb[2] + s * rb[3];
      s6 = s4 * s2;
      S2 = sb[2] + s * sb[3];
      R3 = rb[4] + s * rb[5];
      S3 = sb[4] + s * sb[5];
      S4 = sb[6] + s * sb[7];
      R = R1 + s2 * R2 + s4 * R3 + s6 * rb[6];
      S = S1 + s2 * S2 + s4 * S3 + s6 * S4;
#endif
    }
    z = x;
    // SET_LOW_WORD(z,0);
    fp_struct<T> fp_z(z);
    ap_uint<Type_BitWidth<T>::Value> data = fp_z.data() & mask;
    fp_struct<T> fp_z_new(data);
    z = fp_z_new.to_ieee();
    // r  =  __ieee754_exp(-z*z-0.5625)*
    //	__ieee754_exp((z-x)*(z+x)+R/S);
    r = ::hls::exp(-z * z - (T)0.5625) * ::hls::exp((z - x) * (z + x) + R / S);
    T r_x = r / x;
    if (hx > 0) {
      if (FP_SUBNORMAL == ::hls::fpclassify(r_x)) {
        return 0;
      }
      return r_x;
    } else
      return two - r_x;
  } else {
    if (hx > 0)
      return tiny * tiny;
    else
      return two - tiny;
  }
}

#undef tiny
#undef one_over_two
#undef one
#undef two
#undef erx
#undef efx
#undef efx8
#undef pp
#undef qq
#undef pa
#undef qa
#undef ra
#undef sa
#undef rb
#undef sb
#undef seg
#undef segc
#undef mask

static double erf(double x) { return generic_erf(x); }

static float erf(float x) { return generic_erf(x); }

// TODO
// will give a half implementation later
static half erf(half x) { return generic_erf((float)x); }

static float erff(float x) { return generic_erf(x); }

static double erfc(double x) { return generic_erfc(x); }

static float erfc(float x) { return generic_erfc(x); }

// TODO
// will give a half implementation later
static half erfc(half x) { return generic_erfc((float)x); }

static float erfcf(float x) { return generic_erfc(x); }

} // namespace erf_approx
#endif //
