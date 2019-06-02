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
 * @file hls_gamma.h
 *
 * pure software implementations
 */

static ap_ufixed<0 + 52, 0> lgamma_product(ap_ufixed<0 + 52, 0> t,
                                           ap_ufixed<5 + 52, 5> x,
                                           /*double x_eps, */ ap_uint<2> n) {
#pragma HLS pipeline

  // fp_struct<double> ts(t);
  // fp_struct<double> xs(x);
  // fp_struct<double> x_epss(x_eps);
  ap_ufixed<0 + 52, 0> ret[4] = {0};
// ap_ufixed<0 + 52, 0> ret_eps[4] = {0};
// double ret[4] = {0};
// double ret_eps[4] = {0};
#pragma unroll
  for (ap_uint<3> i = 1; i < 4; i++) {
    // int x_exp = xs.expv();
    // ap_ufixed<64 + 64, 64> x_fix = 0;
    // x_fix[x_fix.wl()-x_fix.iwl()] = 1; // The implicit '1' in IEEE format.
    // x_fix(x_fix.wl()-x_fix.iwl()-1,x_fix.wl()-x_fix.iwl()-52) = xs.sig(51,0);
    // if (x_exp>0)	x_fix <<= x_exp;
    // else		x_fix >>= (-x_exp);
    // x_fix += i;

    ap_ufixed<5 + 52, 5> xi = x + i;
    // double xi = x+i;
    ap_ufixed<0 + 52, 0> quot = t / xi;
    // double quot = t/xi;
    // double m = quot*xi;
    // double quot_lo = quot*x_eps/xi;//(t-m)/xi-t*x_eps/(xi*xi);
    ap_ufixed<0 + 52, 0> r = ret[i - 1] * quot;
    ap_ufixed<0 + 52, 0> rpq = ret[i - 1] + quot;
    // double r = ret[i-1]*quot;
    // double rpq = ret[i-1]+quot;
    // double rpq_eps = (ret[i-1]-rpq)+quot;
    ret[i] = rpq + r;
    // ret_eps[i] = (rpq-ret[i])+r;
    // ret_eps[i] =
    // ret_eps[i-1]+(rpq_eps+ret_eps[i]+ret_eps[i-1]*quot+quot_lo+quot_lo*(ret[i-1]+ret_eps[i-1]));
  }
  return ret[n] /*+ret_eps[n]*/;
}

static double lg_sinpi(ap_fixed<1 - 1 + 52, 1 - 1> x) {
#pragma HLS pipeline

  double xd1 = x;
  ap_fixed<1 - 1 + 52, 1 - 1> x2 = (ap_fixed<0 + 1, 0>)0.5 - x;
  double xd2 = x2;
  if (x[x.wl() - 1] | ~x[x.wl() - 2])
    return sin(M_PI * xd1);
  else
    return cos(M_PI * xd2);
}

static double lg_cospi(ap_fixed<1 - 1 + 52, 1 - 1> x) {
#pragma HLS pipeline

  double xd1 = x;
  ap_fixed<1 - 1 + 52, 1 - 1> x2 = (ap_ufixed<0 + 1, 0>)0.5 - x;
  double xd2 = x2;
  if (x[x.wl() - 1] | ~x[x.wl() - 2])
    return cos(double(M_PI) * xd1);
  else
    return sin(double(M_PI) * xd2);
}

static double lg_cotpi(ap_fixed<1 - 1 + 52, 1 - 1> x) {
#pragma HLS pipeline

  return lg_cospi(x) / lg_sinpi(x);
}

static double lgamma_neg(double x, int *signgamp) {
#pragma HLS pipeline

  const ap_ufixed<5 + 52, 5> lgamma_zeros[52] = {
      /*{ -*/ 0x2.74ff92c01f0d8p+0, // -0x2.abec9f315f1ap-56 },
      /*{ -*/ 0x2.bf6821437b202p+0, // 0x6.866a5b4b9be14p-56 },
      /*{ -*/ 0x3.24c1b793cb35ep+0, // -0xf.b8be699ad3d98p-56 },
      /*{ -*/ 0x3.f48e2a8f85fcap+0, // -0x1.70d4561291237p-56 },
      /*{ -*/ 0x4.0a139e1665604p+0, // 0xf.3c60f4f21e7fp-56 },
      /*{ -*/ 0x4.fdd5de9bbabf4p+0, // 0xa.ef2f55bf89678p-56 },
      /*{ -*/ 0x5.021a95fc2db64p+0, // -0x3.2a4c56e595394p-56 },
      /*{ -*/ 0x5.ffa4bd647d034p+0, // -0x1.7dd4ed62cbd32p-52 },
      /*{ -*/ 0x6.005ac9625f234p+0, // 0x4.9f83d2692e9c8p-56 },
      /*{ -*/ 0x6.fff2fddae1bcp+0,  // 0xc.29d949a3dc03p-60 },
      /*{ -*/ 0x7.000cff7b7f87cp+0, // 0x1.20bb7d2324678p-52 },
      /*{ -*/ 0x7.fffe5fe05673cp+0, // -0x3.ca9e82b522b0cp-56 },
      /*{ -*/ 0x8.0001a01459fc8p+0, // -0x1.f60cb3cec1cedp-52 },
      /*{ -*/ 0x8.ffffd1c425e8p+0,  // -0xf.fc864e9574928p-56 },
      /*{ -*/ 0x9.00002e3bb47d8p+0, // -0x6.d6d843fedc35p-56 },
      /*{ -*/ 0x9.fffffb606bep+0,   // 0x2.32f9d51885afap-52 },
      /*{ -*/ 0xa.0000049f93bb8p+0, // -0x1.927b45d95e154p-52 },
      /*{ -*/ 0xa.ffffff9466eap+0,  // 0xe.4c92532d5243p-56 },
      /*{ -*/ 0xb.0000006b9915p+0,  // -0x3.15d965a6ffea4p-52 },
      /*{ -*/ 0xb.fffffff708938p+0, // -0x7.387de41acc3d4p-56 },
      /*{ -*/ 0xc.00000008f76c8p+0, // 0x8.cea983f0fdafp-56 },
      /*{ -*/ 0xc.ffffffff4f6ep+0,  // 0x3.09e80685a0038p-52 },
      /*{ -*/ 0xd.00000000b092p+0,  // -0x3.09c06683dd1bap-52 },
      /*{ -*/ 0xd.fffffffff3638p+0, // 0x3.a5461e7b5c1f6p-52 },
      /*{ -*/ 0xe.000000000c9c8p+0, // -0x3.a545e94e75ec6p-52 },
      /*{ -*/ 0xe.ffffffffff29p+0,  // 0x3.f9f399fb10cfcp-52 },
      /*{ -*/ 0xf.0000000000d7p+0,  // -0x3.f9f399bd0e42p-52 },
      /*{ -*/ 0xf.fffffffffff28p+0, // -0xc.060c6621f513p-56 },
      /*{ -*/ 0x1.000000000000dp+4, // -0x7.3f9f399da1424p-52 },
      /*{ -*/ 0x1.0ffffffffffffp+4, // -0x3.569c47e7a93e2p-52 },
      /*{ -*/ 0x1.1000000000001p+4, // 0x3.569c47e7a9778p-52 },
      /*{ -*/ 0x1.2p+4,             // 0xb.413c31dcbecdp-56 },
      /*{ -*/ 0x1.2p+4,             // -0xb.413c31dcbeca8p-56 },
      /*{ -*/ 0x1.3p+4,             // 0x9.7a4da340a0ab8p-60 },
      /*{ -*/ 0x1.3p+4,             // -0x9.7a4da340a0ab8p-60 },
      /*{ -*/ 0x1.4p+4,             // 0x7.950ae90080894p-64 },
      /*{ -*/ 0x1.4p+4,             // -0x7.950ae90080894p-64 },
      /*{ -*/ 0x1.5p+4,             // 0x5.c6e3bdb73d5c8p-68 },
      /*{ -*/ 0x1.5p+4,             // -0x5.c6e3bdb73d5c8p-68 },
      /*{ -*/ 0x1.6p+4,             // 0x4.338e5b6dfe14cp-72 },
      /*{ -*/ 0x1.6p+4,             // -0x4.338e5b6dfe14cp-72 },
      /*{ -*/ 0x1.7p+4,             // 0x2.ec368262c7034p-76 },
      /*{ -*/ 0x1.7p+4,             // -0x2.ec368262c7034p-76 },
      /*{ -*/ 0x1.8p+4,             // 0x1.f2cf01972f578p-80 },
      /*{ -*/ 0x1.8p+4,             // -0x1.f2cf01972f578p-80 },
      /*{ -*/ 0x1.9p+4,             // 0x1.3f3ccdd165fa9p-84 },
      /*{ -*/ 0x1.9p+4,             // -0x1.3f3ccdd165fa9p-84 },
      /*{ -*/ 0x1.ap+4,             // 0xc.4742fe35272dp-92 },
      /*{ -*/ 0x1.ap+4,             // -0xc.4742fe35272dp-92 },
      /*{ -*/ 0x1.bp+4,             // 0x7.46ac70b733a8cp-96 },
      /*{ -*/ 0x1.bp+4,             // -0x7.46ac70b733a8cp-96 },
      /*{ -*/ 0x1.cp+4,             // 0x4.2862898d42174p-100 },
  };

  fp_struct<double> xs(x);
  fp_struct<double> out;
  out.sign[0] = 0;
  out.sig = -1;
  out.exp = fp_struct<double>::EXP_INFNAN;

  ap_ufixed<5 + 51, 5> x_fix = 0;
  x_fix[x_fix.wl() - x_fix.iwl() + 1] = 1;
  x_fix(51, 0) = xs.sig(51, 0);
  if (xs.exp[1])
    x_fix <<= 2;
  if (xs.exp[0])
    x_fix <<= 1;

  ap_uint<6> i;
  i(5, 0) = x_fix(x_fix.wl() - 1, x_fix.wl() - x_fix.iwl() - 1);
  // int i = floor((-x)<<1);
  // ap_int<3> i2 = i;
  if (x_fix(x_fix.wl() - x_fix.iwl() - 1, 0) == 0)
    // if ( (i2[0]==0) && (i==((-x)<<1)) )
    return out.to_ieee();
  ap_uint<5> xn;
  xn(4, 0) = i(5, 1) + i[0];
  // double xn = (i2[0]==0) ? -i/2 : (-i-1)/2;
  i -= 4;
  *signgamp = (i[1] == 0) ? -1 : 1;
  //*signgamp = (i2[1]==0) ? -1 : 1;
  ap_ufixed<5 + 52, 5> x0 = lgamma_zeros[i];
  // double x0_hi = lgamma_zeros[i][0];
  // double x0_lo = lgamma_zeros[i][1];
  ap_fixed<1 + 0 + 52, 1 + 0> xdiff = x0 - x_fix;
  // double xdiff = x-x0_hi-x0_lo;
  if (i < 2) {
    const ap_uint<2> poly_deg[8] = {
        0, 1, 2, 3, 3, 2, 1, 1,
    };
    // const ap_uint<4> poly_deg[8] = { 10, 11, 12, 13, 13, 12, 11, 11, };
    const ap_uint<7> poly_end[8] = {
        10, 22, 35, 49, 63, 76, 88, 100,
    };
    const ap_fixed<1 + 12 + 52, 1 + 12> poly_coeff[112] = {
        /* Interval [-2.125, -2] (polynomial degree 10).  */
        -0x1.0b71c5c54d42fp+0,
        -0xc.73a1dc05f3758p-4,
        -0x1.ec84140851911p-4,
        -0xe.37c9da23847e8p-4,
        -0x1.03cd87cdc0ac6p-4,
        -0xe.ae9aedce12eep-4,
        0x9.b11a1780cfd48p-8,
        -0xe.f25fc460bdebp-4,
        0x2.6e984c61ca912p-4,
        -0xf.83fea1c6d35p-4,
        0x4.760c8c8909758p-4,
        /* Interval [-2.25, -2.125] (polynomial degree 11).  */
        -0xf.2930890d7d678p-4,
        -0xc.a5cfde054eaa8p-4,
        0x3.9c9e0fdebd99cp-4,
        -0x1.02a5ad35619d9p+0,
        0x9.6e9b1167c164p-4,
        -0x1.4d8332eba090ap+0,
        0x1.1c0c94b1b2b6p+0,
        -0x1.c9a70d138c74ep+0,
        0x1.d7d9cf1d4c196p+0,
        -0x2.91fbf4cd6abacp+0,
        0x2.f6751f74b8ff8p+0,
        -0x3.e1bb7b09e3e76p+0,
        /* Interval [-2.375, -2.25] (polynomial degree 12).  */
        -0xd.7d28d505d618p-4,
        -0xe.69649a3040958p-4,
        0xb.0d74a2827cd6p-4,
        -0x1.924b09228a86ep+0,
        0x1.d49b12bcf6175p+0,
        -0x3.0898bb530d314p+0,
        0x4.207a6be8fda4cp+0,
        -0x6.39eef56d4e9p+0,
        0x8.e2e42acbccec8p+0,
        -0xd.0d91c1e596a68p+0,
        0x1.2e20d7099c585p+4,
        -0x1.c4eb6691b4ca9p+4,
        0x2.96a1a11fd85fep+4,
        /* Interval [-2.5, -2.375] (polynomial degree 13).  */
        -0xb.74ea1bcfff948p-4,
        -0x1.2a82bd590c376p+0,
        0x1.88020f828b81p+0,
        -0x3.32279f040d7aep+0,
        0x5.57ac8252ce868p+0,
        -0x9.c2aedd093125p+0,
        0x1.12c132716e94cp+4,
        -0x1.ea94dfa5c0a6dp+4,
        0x3.66b61abfe858cp+4,
        -0x6.0cfceb62a26e4p+4,
        0xa.beeba09403bd8p+4,
        -0x1.3188d9b1b288cp+8,
        0x2.37f774dd14c44p+8,
        -0x3.fdf0a64cd7136p+8,
        /* Interval [-2.625, -2.5] (polynomial degree 13).  */
        -0x3.d10108c27ebbp-4,
        0x1.cd557caff7d2fp+0,
        0x3.819b4856d36cep+0,
        0x6.8505cbacfc42p+0,
        0xb.c1b2e6567a4dp+0,
        0x1.50a53a3ce6c73p+4,
        0x2.57adffbb1ec0cp+4,
        0x4.2b15549cf400cp+4,
        0x7.698cfd82b3e18p+4,
        0xd.2decde217755p+4,
        0x1.7699a624d07b9p+8,
        0x2.98ecf617abbfcp+8,
        0x4.d5244d44d60b4p+8,
        0x8.e962bf7395988p+8,
        /* Interval [-2.75, -2.625] (polynomial degree 12).  */
        -0x6.b5d252a56e8a8p-4,
        0x1.28d60383da3a6p+0,
        0x1.db6513ada89bep+0,
        0x2.e217118fa8c02p+0,
        0x4.450112c651348p+0,
        0x6.4af990f589b8cp+0,
        0x9.2db5963d7a238p+0,
        0xd.62c03647da19p+0,
        0x1.379f81f6416afp+4,
        0x1.c5618b4fdb96p+4,
        0x2.9342d0af2ac4ep+4,
        0x3.d9cdf56d2b186p+4,
        0x5.ab9f91d5a27a4p+4,
        /* Interval [-2.875, -2.75] (polynomial degree 11).  */
        -0x8.a41b1e4f36ff8p-4,
        0xc.da87d3b69dbe8p-4,
        0x1.1474ad5c36709p+0,
        0x1.761ecb90c8c5cp+0,
        0x1.d279bff588826p+0,
        0x2.4e5d003fb36a8p+0,
        0x2.d575575566842p+0,
        0x3.85152b0d17756p+0,
        0x4.5213d921ca13p+0,
        0x5.55da7dfcf69c4p+0,
        0x6.acef729b9404p+0,
        0x8.483cc21dd0668p+0,
        /* Interval [-3, -2.875] (polynomial degree 11).  */
        -0xa.046d667e468f8p-4,
        0x9.70b88dcc006cp-4,
        0xa.a8a39421c94dp-4,
        0xd.2f4d1363f98ep-4,
        0xd.ca9aa19975b7p-4,
        0xf.cf09c2f54404p-4,
        0x1.04b1365a9adfcp+0,
        0x1.22b54ef213798p+0,
        0x1.2c52c25206bf5p+0,
        0x1.4aa3d798aace4p+0,
        0x1.5c3f278b504e3p+0,
        0x1.7e08292cc347bp+0,
    };
    ap_uint<3> j;
    j(2, 0) = x_fix(50, 48);
    // int j = floor(-8*x)-16;
    ap_ufixed<-3 + 4, -3> xm = 0.0625;
    // double xm = (-33-2*j)*0.0625;
    ap_ufixed<-3 + 51, -3> x_fix_s = x_fix;
    ap_fixed<1 - 3 + 51, 1 - 3> x_adj = xm - x_fix_s;
    // double x_adj = x-xm;
    ap_uint<2> deg = poly_deg[j];
    ap_uint<7> end = poly_end[j];
    ap_fixed<1 + 12 + 52, 1 + 12> g = poly_coeff[end];
#pragma unroll
    for (ap_uint<4> j = 1; j <= 10; j++)
      g = g * x_adj + poly_coeff[end - j];
    if (deg[1] | deg[0])
      g = g * x_adj + poly_coeff[end - 11];
    if (deg[1])
      g = g * x_adj + poly_coeff[end - 12];
    if (deg[1] & deg[0])
      g = g * x_adj + poly_coeff[end - 13];
    ap_fixed<1 + 5 + 51, 1 + 5> x_fix_sign = x_fix;
    x_fix_sign = -x_fix_sign;
    return log1p(double(g * xdiff / (x_fix_sign - xn)));
  }
  ap_fixed<1 + 5 + 52, 1 + 5> pos = x_fix - xn;
  ap_fixed<1 + 5 + 52, 1 + 5> neg = xn - x_fix;
  ap_ufixed<-1 + 52, -1> x_idiff;
  if (~pos[57])
    x_idiff(50, 0) = pos(50, 0);
  else
    x_idiff(50, 0) = neg(50, 0);
  // double x_idiff = fabs(xn-x);
  pos = x0 - xn;
  neg = xn - x0;
  ap_ufixed<-1 + 52, -1> x0_idiff;
  if (~pos[57])
    x0_idiff(50, 0) = pos(50, 0);
  else
    x0_idiff(50, 0) = neg(50, 0);
  // double x0_idiff = fabs(xn-x0_hi-x0_lo);
  double log_sinpi_ratio;
  if (x0_idiff < (x_idiff >> 1)) {
    // if ( x0_idiff<x_idiff*0.5 ) {
    log_sinpi_ratio = log(lg_sinpi(x0_idiff) / lg_sinpi(x_idiff));
  } else {
    ap_fixed<1 - 1 + 52, 1 - 1> x0diff2;
    if (~i[0])
      x0diff2 = xdiff >> 1;
    else
      x0diff2 = (-xdiff) >> 1;
    // double x0diff2 = ((i2[0]==0) ? xdiff : -xdiff)*0.5;
    double sx0d2 = lg_sinpi(x0diff2);
    double cx0d2 = lg_cospi(x0diff2);
    log_sinpi_ratio = log1p(2 * sx0d2 * (-sx0d2 + cx0d2 * lg_cotpi(x_idiff)));
  }
  double log_gamma_ratio;
  ap_ufixed<5 + 52, 5> y0 = x0 - 1;
  // double y0 = 1-x0_hi;
  // double y0_eps = -x0_hi+(1-y0)-x0_lo;
  ap_ufixed<5 + 52, 5> y = x_fix + 1;
  // double y = 1-x;
  // double y_eps = -x+(1-y);
  double log_gamma_adj = 0;
  if (i < 6) {
    ap_uint<2> n_up = (7 - i) / 2;
    y0 += n_up;
    // int n_up = (7-i)/2;
    // double ny0 = y0+n_up;
    // double ny0_eps = y0-(ny0-n_up)+y0_eps;
    // y0 = ny0;
    // y0_eps = ny0_eps;
    // double ny = y+n_up;
    // double ny_eps = y-(ny-n_up)+y_eps;
    // y = ny;
    // y_eps = ny_eps;
    ap_ufixed<0 + 52, 0> prodm1 = lgamma_product(xdiff, y, n_up);
    // double prodm1 = lgamma_product(xdiff,y-n_up,y_eps,n_up);
    y += n_up;
    log_gamma_adj = -log1p(double(prodm1));
  }
  const ap_ufixed<2 + 52, 2> e = 0x2.b7e151628aed2p+0;
  // const double e_hi = 0x2.b7e151628aed2p+0;
  // const double e_lo = 0xa.6abf7158809dp-56;
  // const ap_fixed<1 + 8 + 52, 1 + 8> lgamma_coeff[12] = {
  // 0x1.5555555555555p-4,    -0xb.60b60b60b60b8p-12,    0x3.4034034034034p-12,
  // -0x2.7027027027028p-12, 				    0x3.72a3c5631fe46p-12,    -0x7.daac36664f1f4p-12,
  //0x1.a41a41a41a41ap-8,    -0x7.90a1b2c3d4e6p-8, 				    0x2.dfd2c703c0dp-4,
  //-0x1.6476701181f3ap+0,    0xd.672219167003p+0,    -0x9.cd9292e6660d8p+4, };
  double log_gamma_high =
      ((double)xdiff) * log1p(double((y0 - e) / e)) +
      (double(y - (ap_ufixed<0 + 1, 0>)0.5) * log1p(double(xdiff / y))) +
      log_gamma_adj;
  // double log_gamma_high =
  // (xdiff*log1p((y0-e_hi-e_lo+y0_eps)/e_hi)+(y-0.5+y_eps)*log1p(xdiff/y)+log_gamma_adj);
  /*ap_ufixed<0 + 52, 0> y0r = 1/y0;
  ap_ufixed<0 + 52, 0> yr = 1/y;
  ap_ufixed<0 + 52, 0> y0r2 = y0r*y0r;
  ap_ufixed<0 + 52, 0> yr2 = yr*yr;
  ap_fixed<1 + 0 + 52, 1 + 0> rdiff = -xdiff*yr*y0r;
  //double y0r = 1/y0;
  //double yr = 1/y;
  //double y0r2 = y0r*y0r;
  //double yr2 = yr*yr;
  //double rdiff = -xdiff/(y*y0);
  ap_fixed<1 + 8 + 52, 1 + 8> bterm[12];
  ap_fixed<1 + 0 + 52, 1 + 0> dlast = rdiff;
  ap_fixed<1 + 0 + 52, 1 + 0> elast = rdiff*yr*(yr+y0r);
  //double bterm[12];
  //double dlast = rdiff;
  //double elast = rdiff*yr*(yr+y0r);
  bterm[0] = dlast*lgamma_coeff[0];
  #pragma unroll
  for ( int j = 1; j < 12; j++ )
  {
          ap_fixed<1 + 0 + 52, 1 + 0> dnext = dlast*y0r2+elast;
          ap_fixed<1 + 0 + 52, 1 + 0> enext = elast*yr2;
          //double dnext = dlast*y0r2+elast;
          //double enext = elast*yr2;
          bterm[j] = dnext*lgamma_coeff[j];
          dlast = dnext;
          elast = enext;
  }
  ap_fixed<1 + 8 + 52, 1 + 8>  log_gamma_low = 0;
  #pragma unroll
  for ( int j = 0; j < 12; j++ )
          log_gamma_low += bterm[12-1-j];*/
  log_gamma_ratio = log_gamma_high /*+log_gamma_low*/;
  return log_sinpi_ratio + log_gamma_ratio;
}

/*static double sin_pi (double x)
{
#pragma HLS pipeline

        const double pi = 3.14159265358979311600e+00;
        const double two52=  4.50359962737049600000e+15; // 0x43300000,
0x00000000 fp_struct<double> xs(x); double ix = fabs(x); if ( xs.exp<1021 )//if
( ix<0.25 ) return sin(pi*x); ap_ufixed<1 + 52, 1> x_frac = 0; x_frac[51] = 1;
        x_frac(51,0) = xs.sig(51,0);
        int x_exp = xs.expv();
        if ( x_exp>0 )	x_frac <<= x_exp;
        else		x_frac >>= -x_exp;

        ap_ufixed<1 + 52, 1> y_frac;
        y_frac(51,0) = x_frac(51,0);
        ap_uint<3> n;
        n(2,0) = x_frac(52,50);
        //double y = -x;
        //double z = floor(y);
        //if ( z!=y ) {
        //	y *= 0.5;
        //	y = 2*(y-floor(y));
        //	n = (int)(y*4);
        //} else {
        //	if ( xs.exp>=1076 ) {
        //		y = 0;
        //		n = 0;
        //	} else {
        //		if ( xs.exp<1075 )
        //			z = y+two52;
        //		ap_int<2> n2 = z;
        //		ap_uint<1> n1;
        //		n1[0] = n2[0];
        //		n = n1;
        //		y = n;
        //		n <<= 2;
        //	}
        //}
        switch(n) {
            case 0:   y =  sin(pi*y); break;
            case 1:
            case 2:   y =  cos(pi*(0.5-y)); break;
            case 3:
            case 4:   y =  sin(pi*(1-y)); break;
            case 5:
            case 6:   y = -cos(pi*(y-1.5)); break;
            default:  y =  sin(pi*(y-2.0)); break;
        }
        return -y;
}
*/
static double lgamma_r(double x, int *signgamp) {
#pragma HLS pipeline

  fp_struct<double> xs(x);
  fp_struct<double> out;
  out.sign[0] = 0;
  out.sig = 0;
  out.exp = fp_struct<double>::EXP_INFNAN;

  *signgamp = 1;
  // double ix = fabs(x);
  if (::hls::__isinf(x))
    return out.to_ieee();
  if (::hls::__isnan(x)) {
    out.sig = -1;
    return out.to_ieee();
  }
  if (xs.exp == 0) {
    if (xs.sign[0])
      *signgamp = -1;
    out.sig = -1;
    return out.to_ieee();
  }
  if (xs.exp < 953) {
    if (xs.sign[0])
      *signgamp = -1;
    return -log(fabs(x));
  }
  double nadj = 0;
  if (xs.sign[0]) {
    if (xs.exp >= 1075) {
      out.sig = -1;
      return out.to_ieee();
    }
    if ((xs.exp > 1 && xs.exp < 4) || (xs.exp == 1 && (xs.sig != 0)) ||
        (xs.exp == 4 && ~(xs.sig[51] & xs.sig[50]))) // if ( (x<-2)&&(x>-28) )
      return lgamma_neg(x, signgamp);
    const double pi = 3.14159265358979311600e+00;
    double t = sin(pi * x);
    fp_struct<double> ts(t);
    if (ts.exp == 0) {
      out.sig = -1;
      return out.to_ieee();
    }
    nadj = log(pi / fabs(t * x));
    if (ts.sign[0])
      *signgamp = -1;
    x = -x;
    xs.sign[0] = 0;
  }
  double r;
  if ((xs.sig == 0) && ((xs.exp == 1023) || (xs.exp == 1024))) {
    r = 0;
  } else if (xs.exp < 1024) { // x<2
    double y, z, p, p1, p2, p3;
    int i;
    const double tc = 1.46163214496836224576e+00;
    const double tc_m1 = 0.46163214496836224576e+00;
    ap_uint<20> x_sig_hi = (xs.sig >> 32);
    if ((xs.exp < 1022) || ((xs.exp == 1022) && (x_sig_hi <= 838860))) {
      r = -log(x);
      if ((xs.exp > 1022) || ((xs.exp == 1022) && (x_sig_hi >= 485700))) {
        y = 1 - x;
        i = 0;
      } else if ((xs.exp > 1020) ||
                 ((xs.exp == 1020) && (x_sig_hi >= 894561))) {
        y = x - tc_m1;
        i = 1;
      } else {
        y = x;
        i = 2;
      }
    } else {
      r = 0;
      if ((xs.exp > 1023) || ((xs.exp == 1023) && (x_sig_hi >= 767171))) {
        y = 2 - x;
        i = 0;
      } else if ((xs.exp > 1023) ||
                 ((xs.exp == 1023) && (x_sig_hi >= 242884))) {
        y = x - tc;
        i = 1;
      } else {
        y = x - 1;
        i = 2;
      }
    }
    switch (i) {

    case 0: {
      const double a0 = 7.72156649015328655494e-02; /* 0x3FB3C467, 0xE37DB0C8 */
      const double a1 = 3.22467033424113591611e-01; /* 0x3FD4A34C, 0xC4A60FAD */
      const double a2 = 6.73523010531292681824e-02; /* 0x3FB13E00, 0x1A5562A7 */
      const double a3 = 2.05808084325167332806e-02; /* 0x3F951322, 0xAC92547B */
      const double a4 = 7.38555086081402883957e-03; /* 0x3F7E404F, 0xB68FEFE8 */
      const double a5 = 2.89051383673415629091e-03; /* 0x3F67ADD8, 0xCCB7926B */
      const double a6 = 1.19270763183362067845e-03; /* 0x3F538A94, 0x116F3F5D */
      const double a7 = 5.10069792153511336608e-04; /* 0x3F40B6C6, 0x89B99C00 */
      const double a8 = 2.20862790713908385557e-04; /* 0x3F2CF2EC, 0xED10E54D */
      const double a9 = 1.08011567247583939954e-04; /* 0x3F1C5088, 0x987DFB07 */
      const double a10 =
          2.52144565451257326939e-05; /* 0x3EFA7074, 0x428CFA52 */
      const double a11 =
          4.48640949618915160150e-05; /* 0x3F07858E, 0x90A45837 */

      z = y * y;
      p1 = a0 + z * (a2 + z * (a4 + z * (a6 + z * (a8 + z * a10))));
      p2 = z * (a1 + z * (a3 + z * (a5 + z * (a7 + z * (a9 + z * a11)))));
      p = y * p1 + p2;
      r += (p - 0.5 * y);
      break;
    }
    case 1: {
      const double tf =
          -1.21486290535849611461e-01; /* 0xBFBF19B9, 0xBCC38A42 */
      const double tt =
          -3.63867699703950536541e-18;              /* 0xBC50C7CA, 0xA48A971F */
      const double t0 = 4.83836122723810047042e-01; /* 0x3FDEF72B, 0xC8EE38A2 */
      const double t1 =
          -1.47587722994593911752e-01;              /* 0xBFC2E427, 0x8DC6C509 */
      const double t2 = 6.46249402391333854778e-02; /* 0x3FB08B42, 0x94D5419B */
      const double t3 =
          -3.27885410759859649565e-02;              /* 0xBFA0C9A8, 0xDF35B713 */
      const double t4 = 1.79706750811820387126e-02; /* 0x3F9266E7, 0x970AF9EC */
      const double t5 =
          -1.03142241298341437450e-02;              /* 0xBF851F9F, 0xBA91EC6A */
      const double t6 = 6.10053870246291332635e-03; /* 0x3F78FCE0, 0xE370E344 */
      const double t7 =
          -3.68452016781138256760e-03;              /* 0xBF6E2EFF, 0xB3E914D7 */
      const double t8 = 2.25964780900612472250e-03; /* 0x3F6282D3, 0x2E15C915 */
      const double t9 =
          -1.40346469989232843813e-03; /* 0xBF56FE8E, 0xBF2D1AF1 */
      const double t10 =
          8.81081882437654011382e-04; /* 0x3F4CDF0C, 0xEF61A8E9 */
      const double t11 =
          -5.38595305356740546715e-04; /* 0xBF41A610, 0x9C73E0EC */
      const double t12 =
          3.15632070903625950361e-04; /* 0x3F34AF6D, 0x6C0EBBF7 */
      const double t13 =
          -3.12754168375120860518e-04; /* 0xBF347F24, 0xECC38C38 */
      const double t14 =
          3.35529192635519073543e-04; /* 0x3F35FD3E, 0xE8C2D3F4 */

      z = y * y;
      double w = z * y;
      p1 = t0 + w * (t3 + w * (t6 + w * (t9 + w * t12)));
      p2 = t1 + w * (t4 + w * (t7 + w * (t10 + w * t13)));
      p3 = t2 + w * (t5 + w * (t8 + w * (t11 + w * t14)));
      p = z * p1 - (tt - w * (p2 + y * p3));
      r += (tf + p);
      break;
    }
    case 2: {
      const double u0 =
          -7.72156649015328655494e-02;              /* 0xBFB3C467, 0xE37DB0C8 */
      const double u1 = 6.32827064025093366517e-01; /* 0x3FE4401E, 0x8B005DFF */
      const double u2 = 1.45492250137234768737e+00; /* 0x3FF7475C, 0xD119BD6F */
      const double u3 = 9.77717527963372745603e-01; /* 0x3FEF4976, 0x44EA8450 */
      const double u4 = 2.28963728064692451092e-01; /* 0x3FCD4EAE, 0xF6010924 */
      const double u5 = 1.33810918536787660377e-02; /* 0x3F8B678B, 0xBF2BAB09 */
      const double v1 = 2.45597793713041134822e+00; /* 0x4003A5D7, 0xC2BD619C */
      const double v2 = 2.12848976379893395361e+00; /* 0x40010725, 0xA42B18F5 */
      const double v3 = 7.69285150456672783825e-01; /* 0x3FE89DFB, 0xE45050AF */
      const double v4 = 1.04222645593369134254e-01; /* 0x3FBAAE55, 0xD6537C88 */
      const double v5 = 3.21709242282423911810e-03; /* 0x3F6A5ABB, 0x57D0CF61 */

      p1 = y * (u0 + y * (u1 + y * (u2 + y * (u3 + y * (u4 + y * u5)))));
      p2 = 1 + y * (v1 + y * (v2 + y * (v3 + y * (v4 + y * v5))));
      r += (-0.5 * y + p1 / p2);
    }
    }
  } else if (xs.exp < 1026) { // x<8

    const double s0 = -7.72156649015328655494e-02; /* 0xBFB3C467, 0xE37DB0C8 */
    const double s1 = 2.14982415960608852501e-01;  /* 0x3FCB848B, 0x36E20878 */
    const double s2 = 3.25778796408930981787e-01;  /* 0x3FD4D98F, 0x4F139F59 */
    const double s3 = 1.46350472652464452805e-01;  /* 0x3FC2BB9C, 0xBEE5F2F7 */
    const double s4 = 2.66422703033638609560e-02;  /* 0x3F9B481C, 0x7E939961 */
    const double s5 = 1.84028451407337715652e-03;  /* 0x3F5E26B6, 0x7368F239 */
    const double s6 = 3.19475326584100867617e-05;  /* 0x3F00BFEC, 0xDD17E945 */
    const double r1 = 1.39200533467621045958e+00;  /* 0x3FF645A7, 0x62C4AB74 */
    const double r2 = 7.21935547567138069525e-01;  /* 0x3FE71A18, 0x93D3DCDC */
    const double r3 = 1.71933865632803078993e-01;  /* 0x3FC601ED, 0xCCFBDF27 */
    const double r4 = 1.86459191715652901344e-02;  /* 0x3F9317EA, 0x742ED475 */
    const double r5 = 7.77942496381893596434e-04;  /* 0x3F497DDA, 0xCA41A95B */
    const double r6 = 7.32668430744625636189e-06;  /* 0x3EDEBAF7, 0xA5B38140 */

    ap_uint<3> i;
    ap_uint<51> x_frac = 0;
    if (~xs.exp[0]) {
      i[2] = 0;
      i[1] = 1;
      i[0] = xs.sig[51];
      x_frac(50, 0) = xs.sig(50, 0);
    } else {
      i[2] = 1;
      i[1] = xs.sig[51];
      i[0] = xs.sig[50];
      x_frac(50, 1) = xs.sig(49, 0);
    }
    fp_struct<double> ys(double(0));
#pragma unroll
    for (ap_uint<6> pos = 50; pos >= 0; pos--) {
      if (x_frac[pos]) {
        ys.exp = pos - 51;
        ys.sig(51, 51 - pos) = x_frac(pos, 0);
        break;
      }
    }
    double y = ys.to_ieee();
    // int i = (int)x;
    // double t = 0;
    // double y = x-(double)i;
    double p =
        y *
        (s0 + y * (s1 + y * (s2 + y * (s3 + y * (s4 + y * (s5 + y * s6))))));
    double q =
        1 + y * (r1 + y * (r2 + y * (r3 + y * (r4 + y * (r5 + y * r6)))));
    r = 0.5 * y + p / q;
    double z = 1;
    switch (i) {
    case 7:
      z *= (y + 6.0); /* FALLTHRU */
    case 6:
      z *= (y + 5.0); /* FALLTHRU */
    case 5:
      z *= (y + 4.0); /* FALLTHRU */
    case 4:
      z *= (y + 3.0); /* FALLTHRU */
    case 3:
      z *= (y + 2.0); /* FALLTHRU */
      r += log(z);
      break;
    }

  } else if (xs.exp < 1081) {

    const double w0 = 4.18938533204672725052e-01;  /* 0x3FDACFE3, 0x90C97D69 */
    const double w1 = 8.33333333333329678849e-02;  /* 0x3FB55555, 0x5555553B */
    const double w2 = -2.77777777728775536470e-03; /* 0xBF66C16C, 0x16B02E5C */
    const double w3 = 7.93650558643019558500e-04;  /* 0x3F4A019F, 0x98CF38B6 */
    const double w4 = -5.95187557450339963135e-04; /* 0xBF4380CB, 0x8C0FE741 */
    const double w5 = 8.36339918996282139126e-04;  /* 0x3F4B67BA, 0x4CDAD5D1 */
    const double w6 = -1.63092934096575273989e-03; /* 0xBF5AB89D, 0x0B9E43E4 */

    double t = log(x);
    double z = 1 / x;
    double y = z * z;
    double w =
        w0 + z * (w1 + y * (w2 + y * (w3 + y * (w4 + y * (w5 + y * w6)))));
    r = (x - 0.5) * (t - 1) + w;

  } else
    r = x * (log(x) - 1);
  if (x < 0)
    r = nadj - r;
  return r;
}

static ap_fixed<20 + 52, 20>
gamma_product(ap_ufixed<4 + 52, 4> x /*, double x_eps*/,
              ap_uint<3> n /*, double *eps*/) {
#pragma HLS pipeline

  ap_fixed<20 + 52, 20> ret[6] = {0};
  ret[0] = x;
// double eps_[6] = {0};
// eps_[0] = x_eps/x;
#pragma unroll
  for (ap_uint<3> i = 1; i < 6; i++) {
    // eps_[i] = eps[i-1]+x_eps/(x+i);
    ret[i] = ret[i - 1] * (x + i);
  }
  //*eps = eps_[n];
  return ret[n];
}

static double gamma_positive(double x, int *exp2_adj) {
#pragma HLS pipeline

  fp_struct<double> xs(x);
  ap_ufixed<8 + 52, 8> x_fix = 1;
  x_fix(51, 0) = xs.sig(51, 0);
  ap_uint<3> shl;
  shl(2, 0) = xs.exp(2, 0);
  shl++;
  x_fix <<= shl;

  if (xs.exp < 126) {
    *exp2_adj = 0;
    return exp(lgamma(x + 1)) / x;
  } else if ((xs.exp < 127) || ((xs.exp == 127) && (xs.sig[51] == 0))) {
    *exp2_adj = 0;
    return exp(lgamma(x));
  } else if ((xs.exp < 129) || ((xs.exp == 129) && (xs.sig(51, 49) < 5))) {
    *exp2_adj = 0;
    ap_uint<4> nl;
    nl(3, 0) = x_fix(54, 51);
    nl -= 3;
    ap_uint<3> n;
    n(2, 0) = nl(3, 1);
    ap_ufixed<2 + 52, 2> x_adj;
    x_adj(51, 0) = x_fix(51, 0);
    x_adj[52] = x_fix[51];
    x_adj[53] = ~x_fix[51];
    // double n = ceil(x-1.5);
    // double x_adj = x-n;
    // double eps;
    double prod = gamma_product(x_adj /*,0*/, n /*,&eps*/);
    return (exp(lgamma(double(x_adj))) * prod /**(1+eps)*/);
  } else {
    ap_ufixed<8 + 52, 8> x_adj = x_fix;
    // double eps = 0;
    // double x_eps = 0;
    // double x_adj = x;
    double prod = 1;
    if ((xs.exp < 130) || ((xs.exp == 130) && (xs.sig(51, 50) < 3))) {
      ap_uint<4> xi = x_fix(55, 52);
      ap_uint<3> n = 11 - xi;
      x_adj[52] = 1;
      x_adj[53] = 1;
      x_adj[54] = 0;
      x_adj[55] = 1;
      if (x_fix(51, 0) == 0) {
        n++;
        x_adj[52] = 0;
        x_adj[53] = 0;
        x_adj[54] = 1;
      }
      // double n = ceil(12-x);
      // x_adj = x+n;
      // x_eps = (x-(x_adj-n));
      prod = gamma_product(x_fix, n);
      // prod = gamma_product(x_adj-n,x_eps,n,&eps);
    }
    ap_uint<8> x_adj_int;
    x_adj_int(7, 0) = x_adj(59, 52) + x_adj[51];
    ap_fixed<1 - 1 + 52, 1 - 1> x_adj_frac;
    x_adj_frac(51, 0) = x_adj(51, 0);
    // double exp_adj = -eps;
    // double x_adj_int = round(x_adj);
    // double x_adj_frac = x_adj-x_adj_int;
    ap_uint<3> x_adj_log2;
    ap_ufixed<1 + 52, 1> x_adj_mant;
#pragma unroll
    for (ap_uint<3> pos = 6; pos >= 2; pos--) {
      ap_uint<9> x_adj_hi;
      x_adj_hi(8, 0) = x_adj(53 + pos, 45 + pos);
      if (x_adj_hi > 181) {
        x_adj_log2 = pos;
        x_adj_mant(52, 0) = x_adj(53 + pos, 1 + pos);
      }
    }
    // int x_adj_log2;
    // double x_adj_mant = frexp(x_adj,&x_adj_log2);
    // if ( x_adj_mant<M_SQRT1_2 )
    //{
    //	x_adj_log2--;
    //	x_adj_mant *= 2;
    //}
    *exp2_adj = x_adj_log2 * x_adj_int;
    double ret =
        (pow(double(x_adj_mant), double(x_adj)) *
         exp2(double(x_adj_log2 * x_adj_frac)) * exp(double(-x_adj)) *
         sqrt(double((ap_fixed<3 + 52, 3>(M_PI) << 1) / x_adj)) / prod);
    // exp_adj += x_eps*log(x_adj);
    const ap_fixed<1 - 3 + 52, 1 - 3> gamma_coeff[6] = {
        0x1.5555555555555p-4,   -0xb.60b60b60b60b8p-12, 0x3.4034034034034p-12,
        -0x2.7027027027028p-12, 0x3.72a3c5631fe46p-12,  -0x7.daac36664f1f4p-12,
    };
    ap_fixed<1 - 3 + 52, 1 - 3> bsum = gamma_coeff[5];
    ap_fixed<16 + 52, 16> x_adj2 = x_adj * x_adj;
    for (ap_uint<3> i = 1; i <= 5; i++)
      bsum = bsum / x_adj2 + gamma_coeff[5 - i];
    ap_fixed<1 - 3 + 52, 1 - 3> exp_adj = bsum / x_adj;
    return ret + ret * expm1(double(exp_adj));
  }
}

static double tgamma(double x) {
#pragma HLS pipeline

  fp_struct<double> xs(x);
  fp_struct<double> out;
  out.sign[0] = 0;
  out.sig = 0;
  out.exp = fp_struct<double>::EXP_INFNAN;

  ap_ufixed<1 + 52, 1> x_sig = 1;
  x_sig(51, 0) = xs.sig(51, 0);
  int x_exp = xs.expv();
  if (x_exp > 0)
    x_sig <<= x_exp;

  if ((xs.exp == 0) || ((xs.sign[0]) & (::hls::__isinf(x))) ||
      (::hls::__isnan(x)) ||
      ((xs.sign[0] == 1) && (x_exp >= 0) && (x_sig == 0))) {
    out.sig = -1;
    return out.to_ieee();
  }
  if ((::hls::__isinf(x)) ||
      ((xs.sign[0] == 0) &&
       ((xs.exp > 7) || ((xs.exp == 7) && (xs.sig(51, 47) >= 11))))) {
    return out.to_ieee();
  } else {
    double ret;
    if (~xs.sign[0]) {
      int exp2_adj;
      double tret = gamma_positive(x, &exp2_adj);
      ret = scalbn(tret, exp2_adj);
    }
    //		else if ( x>=-DBL_EPSILON/4 )
    //		{
    //			out.sig = -1;
    //			return out.to_ieee();
    //		}
    else {
      fp_struct<double> xs(x);
      ap_ufixed<8 + 52, 8> x_fix = 1;
      x_fix(51, 0) = xs.sig(51, 0);
      ap_uint<3> shl;
      shl(2, 0) = xs.exp(2, 0);
      shl++;
      x_fix <<= shl;

      // double tx = trunc(x);
      if ((xs.sign[0] == 1) &&
          ((xs.exp > 7) || ((xs.exp == 7) && (xs.sig(51, 48) >= 7))))
        ret = 0;
      else {
        ap_ufixed<0 + 52, 0> frac;
        frac(51, 0) = x_fix(51, 0);
        // double frac = tx-x;
        if (frac[51])
          frac = 1 - frac;
        double sinpix = ((~frac[51]) & (~frac[50]))
                            ? sin(double((ap_fixed<2 + 52, 2>)M_PI * frac))
                            : cos(double((ap_fixed<2 + 52, 2>)M_PI *
                                         ((ap_ufixed<0 + 52, 0>)0.5 - frac)));
        int exp2_adj;
        double tret = M_PI / (-x * sinpix * gamma_positive(-x, &exp2_adj));
        ret = scalbn(tret, -exp2_adj);
      }
    }
    return ret;
  }
}
