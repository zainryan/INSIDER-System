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
/* File: hls_rsr_tables.h
 *
 * Description:
 *     looking up tables for reciprocal, square root and reciprocal square root
 *functions the value of the key parameter k_bits is chosen to avoid TMD
 *accrding to the book "Elementary Functions Algorithms and Implementation" by
 *Jean-Michel Muller
 */

#pragma once

namespace {

template <typename T> class Trait {};

template <> class Trait<half> {
public:
  // k_bits is the key parameter in this algorithm
  // it controls the size of LUTs, size of multipliers, etc.
  // the accuracy bit is 4*k_bits-4 for reciprocal
  // 					4*k_bits-3 for both sqrt and rsqrt
  // for half precision k_bits=4 is enough for faithfull rounding
  // to avoid TMD, k_bits is chosen to 6 for correctness rounding
  const static int k_bits = 6;
  const static int n_bits = 4 * k_bits;

  typedef ap_ufixed<k_bits + 1, 0> LUT_RType;
  typedef ap_ufixed<n_bits + 1, 1> LUT_MType;
  const static int LUTsize = 1 << k_bits;
  const static ap_ufixed<1, -fp_struct<half>::SIG_BITS> RC;
};

const ap_ufixed<1, -fp_struct<half>::SIG_BITS> Trait<half>::RC = 0x0.002p0;

template <typename R, typename T, int size> class RTable {};

template <>
class RTable<typename Trait<half>::LUT_RType, half, Trait<half>::LUTsize> {
public:
  static const typename Trait<half>::LUT_RType arrayR[Trait<half>::LUTsize];
};
const Trait<half>::LUT_RType
    RTable<Trait<half>::LUT_RType, half,
           Trait<half>::LUTsize>::arrayR[Trait<half>::LUTsize] = {
        0x1,      0x0.fcp0, 0x0.f8p0, 0x0.f4p0, 0x0.f0p0, 0x0.ecp0, 0x0.eap0,
        0x0.e6p0, 0x0.e2p0, 0x0.e0p0, 0x0.dcp0, 0x0.dap0, 0x0.d6p0, 0x0.d4p0,
        0x0.d2p0, 0x0.cep0, 0x0.ccp0, 0x0.cap0, 0x0.c6p0, 0x0.c4p0, 0x0.c2p0,
        0x0.c0p0, 0x0.bep0, 0x0.bcp0, 0x0.bap0, 0x0.b8p0, 0x0.b6p0, 0x0.b4p0,
        0x0.b2p0, 0x0.b0p0, 0x0.aep0, 0x0.acp0, 0x0.aap0, 0x0.a8p0, 0x0.a6p0,
        0x0.a4p0, 0x0.a2p0, 0x0.a2p0, 0x0.a0p0, 0x0.9ep0, 0x0.9cp0, 0x0.9cp0,
        0x0.9ap0, 0x0.98p0, 0x0.96p0, 0x0.96p0, 0x0.94p0, 0x0.92p0, 0x0.92p0,
        0x0.90p0, 0x0.8ep0, 0x0.8ep0, 0x0.8cp0, 0x0.8cp0, 0x0.8ap0, 0x0.88p0,
        0x0.88p0, 0x0.86p0, 0x0.86p0, 0x0.84p0, 0x0.84p0, 0x0.82p0, 0x0.82p0,
        0x0.80p0};

template <typename M, typename T, int size> class RecipMTable {};
template <typename M, typename T, int size> class SqrtMTable {};
template <typename M, typename T, int size> class RsqrtMTable {};
// 	template<> class RecipMTable<typename Trait<half>::LUT_RType,
// 		half, Trait<half>::LUTsize> {
// 			public:
// 				static const typename Trait<half>::LUT_RType
// *arrayM;
// 		};
// 	const Trait<half>::LUT_RType *RecipMTable<Trait<half>::LUT_RType,
// 				half, Trait<half>::LUTsize>::arrayM =
// RTable<Trait<half>::LUT_RType, 				half, Trait<half>::LUTsize>::arrayR;
template <>
class RsqrtMTable<typename Trait<half>::LUT_MType, half, Trait<half>::LUTsize> {
public:
  static const typename Trait<half>::LUT_MType arrayM[Trait<half>::LUTsize];
};
const Trait<half>::LUT_MType
    RsqrtMTable<Trait<half>::LUT_MType, half,
                Trait<half>::LUTsize>::arrayM[Trait<half>::LUTsize] = {
        0x0,           0x0.fdfdfb8p0, 0x0.fbf7df0p0, 0x0.f9ed908p0,
        0x0.f7def58p0, 0x0.f5cbf20p0, 0x0.f4c0c00p0, 0x0.f2a6ea0p0,
        0x0.f088618p0, 0x0.ef77508p0, 0x0.ed517f0p0, 0x0.ec3cb70p0,
        0x0.ea0f500p0, 0x0.e8f6a90p0, 0x0.e7dcad8p0, 0x0.e5a4a88p0,
        0x0.e486948p0, 0x0.e367190p0, 0x0.e123d40p0, 0x0.e000000p0,
        0x0.dedaad8p0, 0x0.ddb3d70p0, 0x0.dc8b768p0, 0x0.db61858p0,
        0x0.da35fe0p0, 0x0.d908d88p0, 0x0.d7da0f8p0, 0x0.d6a99b0p0,
        0x0.d577748p0, 0x0.d443948p0, 0x0.d30df30p0, 0x0.d1d6890p0,
        0x0.d09d4e0p0, 0x0.cf623a0p0, 0x0.ce25448p0, 0x0.cce6648p0,
        0x0.cba5918p0, 0x0.cba5918p0, 0x0.ca62c18p0, 0x0.c91deb8p0,
        0x0.c7d7060p0, 0x0.c7d7060p0, 0x0.c68e058p0, 0x0.c542e10p0,
        0x0.c3f58c8p0, 0x0.c3f58c8p0, 0x0.c2a5fd8p0, 0x0.c154280p0,
        0x0.c154280p0, 0x0.c000000p0, 0x0.bea9790p0, 0x0.bea9790p0,
        0x0.bd50868p0, 0x0.bd50868p0, 0x0.bbf51a8p0, 0x0.ba97280p0,
        0x0.ba97280p0, 0x0.b936a08p0, 0x0.b936a08p0, 0x0.b7d3750p0,
        0x0.b7d3750p0, 0x0.b66d958p0, 0x0.b66d958p0, 0x0.b504f30p0};
template <>
class SqrtMTable<typename Trait<half>::LUT_MType, half, Trait<half>::LUTsize> {
public:
  static const typename Trait<half>::LUT_MType arrayM[Trait<half>::LUTsize];
};
const Trait<half>::LUT_MType
    SqrtMTable<Trait<half>::LUT_MType, half,
               Trait<half>::LUTsize>::arrayM[Trait<half>::LUTsize] = {
        0x0,          0x1.020614p0, 0x1.0418a4p0, 0x1.063833p0, 0x1.08654ap0,
        0x1.0aa07bp0, 0x1.0bc38ep0, 0x1.0e150dp0, 0x1.10763cp0, 0x1.11aceep0,
        0x1.1426fap0, 0x1.156a87p0, 0x1.17ff2ep0, 0x1.19507ep0, 0x1.1aa69ep0,
        0x1.1d61c0p0, 0x1.1ec701p0, 0x1.20318cp0, 0x1.23170dp0, 0x1.249249p0,
        0x1.26135ep0, 0x1.279a74p0, 0x1.2927b2p0, 0x1.2abb43p0, 0x1.2c5552p0,
        0x1.2df60cp0, 0x1.2f9d9fp0, 0x1.314c3dp0, 0x1.330218p0, 0x1.34bf63p0,
        0x1.368457p0, 0x1.38512bp0, 0x1.3a261bp0, 0x1.3c0365p0, 0x1.3de948p0,
        0x1.3fd807p0, 0x1.41cfe9p0, 0x1.41cfe9p0, 0x1.43d136p0, 0x1.45dc3ap0,
        0x1.47f144p0, 0x1.47f144p0, 0x1.4a10a9p0, 0x1.4c3abep0, 0x1.4e6fdfp0,
        0x1.4e6fdfp0, 0x1.50b06ap0, 0x1.52fcc4p0, 0x1.52fcc4p0, 0x1.555555p0,
        0x1.57ba8bp0, 0x1.57ba8bp0, 0x1.5a2cd8p0, 0x1.5a2cd8p0, 0x1.5cacb7p0,
        0x1.5f3aa6p0, 0x1.5f3aa6p0, 0x1.61d72bp0, 0x1.61d72bp0, 0x1.6482d3p0,
        0x1.6482d3p0, 0x1.673e32p0, 0x1.673e32p0, 0x1.6a09e6p0,
};

} // namespace
