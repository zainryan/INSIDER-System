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
 *     (c) Copyright 2012 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

#ifndef X_HLS_BIG_MULT_H
#define X_HLS_BIG_MULT_H

template <int W1, int W2>
void big_mult_v1(ap_uint<W1> a, ap_uint<W2> b, ap_uint<W1 + W2> &c) {
  c = a * b;
}

template <int M>
void aca(ap_uint<M> a0, ap_uint<M> a1, ap_uint<M> b0, ap_uint<M> b1,
         ap_uint<2 * M> c, ap_uint<1> cin, ap_uint<2 * M> &p, ap_uint<2> &out) {
  ap_uint<2 * M> p0;
  ap_uint<2 * M> p1;
  ap_uint<(2 * M) + 2> pp0;
  ap_uint<(2 * M) + 2> pp1;

  p0 = a0 * b0;
  pp0 = p0 + c + cin;
  p1 = a1 * b1;
  pp1 = pp0 + p1;
  p = pp1((2 * M) - 1, 0);
  out = pp1(((2 * M) + 1), (2 * M));
}

template <int M> void acb(ap_uint<2> x, ap_uint<M> y, ap_uint<M> &r) {
  r = x + y;
}

template <int M> void cr(ap_uint<M> din, ap_uint<M> &dout) { dout = din; }

template <int W, int M>
void big_mult_v2(ap_uint<W> a, ap_uint<W> b, ap_uint<2 * W> &m) {
  const int N = (W / M) - 1;

  // 1st dim: pipeline stage (including before 1st stage)
  // 2nd dim: W/M breakdown
  ap_uint<2 * M> z[N + 1][N + 1];
  // ap_uint<2*W> z[N+1];

  // 1st dim: pipeline stage
  // 2nd dim: row per stage
  ap_uint<2> ci[N][N + 1]; // TODO check this

  for (int i = 0; i < N + 1; i++) {
    z[0][i] = a(M * (i + 1) - 1, M * i) * b(M * (i + 1) - 1, M * i);
    // z[0](2*M*(i+1)-1,2*M*i) = a(M*(i+1)-1,M*i) * b(M*(i+1)-1,M*i);
#ifdef DEBUG
    printf("z[%d][%d] = a(%d,%d) * b(%d,%d) = %s\n", 0, i, M * (i + 1) - 1,
           M * i, M * (i + 1) - 1, M * i, z[0][i].to_string(16).c_str());
//		printf("z[%d](%d,%d) = a(%d,%d) * b(%d,%d) = %s\n",
//			0,2*M*(i+1)-1,2*M*i,
//			M*(i+1)-1, M*i,
//			M*(i+1)-1, M*i,
//			//z[0](2*M*(i+1)-1,2*M*i).to_string(16).c_str());
//			z[0][i].to_string(16).c_str());
#endif
  }

  //	m(M-1,0) = z[0][0]; // TODO check this
  for (int i = 0; i < N; i++) {
    ci[i][0] = 0;

    ap_uint<M> a0, a1, b0, b1;
    ap_uint<2 * M> c, p;
    ap_uint<1> cin;
    ap_uint<2> out;

    for (int j = 0; j < N; j++) {
      if (j >= i) {
        a0 = a(M * (j - i + 1) - 1, M * (j - i));
        b0 = b(M * (j + 2) - 1, M * (j + 1));
        a1 = a(M * (j + 2) - 1, M * (j + 1));
        b1 = b(M * (j - i + 1) - 1, M * (j - i));
        c = (ap_uint<W>(z[i][j - i + 1](M - 1, 0)),
             ap_uint<M>(z[i][j - i]((2 * M) - 1, M)));
        // c     = z[i]((2*M*(j-i+1))+M-1,M*(2*(j-i)+1));
        cin = ci[i][j - i][0]; // TODO check this
        aca<M>(a0, a1, b0, b1, c, cin, p, out);
        ci[i][j - i + 1] = out;
        z[i + 1][j - i] = p;
        // z[i+1]((2*M*(j-i+1))-1,2*M*(j-i)) = p;
#ifdef DEBUG
        // printf("\ni: %d, j: %d\na0(%d,%d) = %s\nb0(%d,%d) = %s\na1(%d,%d) =
        // %s\nb1(%d,%d) = %s\nc(z[%d][%d]) = %s\ncin(ci[%d][%d]) =
        // %s\nout(ci[%d][%d]) = %s\np(z[%d][%d]) = %s\n",
        printf("\ni: %d, j: %d\na0(%d,%d) = %s\nb0(%d,%d) = %s\na1(%d,%d) = "
               "%s\nb1(%d,%d) = %s\nc(z[%d][%d]) = %s\ncin(ci[%d][%d]) = "
               "%s\nout(ci[%d][%d]) = %s\np(z[%d](%d,%d) = %s\n",
               i, j, M * (j - i + 1), M * (j - i), a0.to_string(16).c_str(),
               M * (j + 2) - 1, M * (j + 1), b0.to_string(16).c_str(),
               M * (j + 2) - 1, M * (j + 1), a1.to_string(16).c_str(),
               M * (j - i + 1), M * (j - i), b1.to_string(16).c_str(), i, j - i,
               c.to_string(16).c_str(), i, j - i, cin.to_string(16).c_str(), i,
               j - i + 1, out.to_string(16).c_str(),
               // i+1,j-i,p.to_string(16).c_str()
               i, (2 * M * (j - i + 1)) - 1, 2 * M * (j - i),
               p.to_string(16).c_str());
#endif
      }
    }
    // if((i == N-1) && (j == N-1)) {

    if (i == N - 1) {
      // m((M*((2*N)+2-i))-1,M*((2*N)+1-i)) = z[N][0]// Store middle 34-bit
      // result m(M*(N+2)-1,M*(N)) = z[N][0];// Store middle 34-bit result
      m(M * (N + 2) - 1, M * (N)) = p; // Store middle 34-bit result
                                       // printf("\nm(%d,%d) = z[%d][0] =
      // %s\n",M*(N+2)-1,M*N,N,z[N][0].to_string(16).c_str()); printf("\naca:
      // m(%d,%d) = z[%d][0] = %s\n",M*(N+2)-1,M*N,N,p.to_string(16).c_str());
#ifdef DEBUG
      printf("\naca: m(%d,%d) = p = %s\n", M * (N + 2) - 1, M * N,
             p.to_string(16).c_str());
#endif
      // m(,) = p[N+1]// Store middle 34-bit result
    }

    // ACB blocks
    ap_uint<2> x;
    ap_uint<M> y, r;
    x = ci[i][N - i]; // TODO check this, should be good
    y = z[i][N - i]((2 * M) - 1, M);
    ////y = z[i](M*((2*(j-i))+2)-1,M*((2*N)-i+1));
    ////y = z[i](M*((2*N)-i+2)-1,M*((2*N)-i+1));
    ////y = z[i](M*((2*N)-i+2)-1,M*((2*N)-i+1));
    // y = z[i](M*((2*(N-i))+2)-1,M*((2*(N-i))+1));
    acb<M>(x, y, r);
    m((M * ((2 * N) + 2 - i)) - 1, M * ((2 * N) + 1 - i)) = r; // looks right
#ifdef DEBUG
    printf("x(ci[%d][%d] = %s, y(z[%d][(%d,%d) = %s\n", i, N - i,
           x.to_string(16).c_str(), i, M * ((2 * N) - i + 2) - 1,
           M * ((2 * N) - i + 1), y.to_string(16).c_str());
    printf("\nacb: m(%d,%d) = %s\n", M * ((2 * N) + 2 - i) - 1,
           M * ((2 * N) + 1 - i), r.to_string(16).c_str());
#endif

    // CR blocks
    ap_uint<M> din, dout;
    din = z[i][0]; // TODO check this
    // din = z[i](M-1,0); // TODO check this
    cr<M>(din, dout);
    // m((M*(i+2))-1,M*(i+1)) = dout;
    m((M * (i + 1)) - 1, M * i) = dout; // looks right
#ifdef DEBUG
    printf("\ncr: m(%d,%d) = %s\n", M * (i + 1) - 1, M * i,
           dout.to_string(16).c_str());
#endif
  }
}

template <int W, int M>
void big_mult_v3(ap_uint<W> a, ap_uint<W> b, ap_uint<2 * W> &m) {
  const int N = (W + M - 1) / M; // Take the ceiling of W/M
  // Partial products
  ap_uint<2 * M> pp[N][N];
  int i, j;
  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j) {
      unsigned Ui = (i == N - 1) ? W - 1 : M * (i + 1) - 1;
      unsigned Uj = (j == N - 1) ? W - 1 : M * (j + 1) - 1;
      pp[i][j] = a(Ui, M * i) * b(Uj, M * j);
    }

  // Partial product sums
  ap_uint<48> pps[2 * N];
  pps[0] = pp[0][0];
  // Add partial products diagonally
  for (i = 1; i < 2 * N; ++i) {
    pps[i] = pps[i - 1] >> M;
    for (j = 0; j < N; ++j)
      if ((j <= i) && (i - j < N)) {
        pps[i] += pp[j][i - j];
      }
  }

  // Extract out the useful bits
  for (i = 0; i < 2 * N; ++i) {
    unsigned Li = M * i;
    if (Li < 2 * W) {
      unsigned Ui = M * (i + 1) - 1;
      if (Ui >= 2 * W) {
        Ui = 2 * W - 1;
      }
      m(Ui, Li) = pps[i](M - 1, 0);
    }
  }
}

// M is the size of multiplier to decompose into.
template <int W, int W2, int M>
void big_mult_v3small(ap_uint<W> a, ap_uint<W2> b, ap_uint<W + W2> &m) {
  const int N = (W + M - 1) / M; // Take the ceiling of W/M
  // Partial products
  ap_uint<M + W2> pp[N];
  int i;
  for (i = 0; i < N; ++i) {
    unsigned Ui = (i == N - 1) ? W - 1 : M * (i + 1) - 1;
    pp[i] = a(Ui, M * i) * b;
  }

  // Partial product sums
  ap_uint<M + W2> pps[N];
  pps[0] = pp[0];
  // Add partial products
  for (i = 1; i < N; ++i) {
    pps[i] = pps[i - 1] >> M;
    pps[i] += pp[i];
  }

  // Extract out the useful bits
  for (i = 0; i < N - 1; ++i) {
    unsigned Li = M * i;
    if (Li < W + W2) {
      unsigned Ui = M * (i + 1) - 1;
      if (Ui >= W + W2) {
        Ui = W + W2 - 1;
      }
      m(Ui, Li) = pps[i](M - 1, 0);
    }
  }
  unsigned Li = M * i;
  if (Li < W + W2) {
    // The highest term is larger than the others
    unsigned Ui = W + W2 - 1;
    m(Ui, Li) = pps[i]; //(M-1, 0);
  }
}

template <int SELECT, int W, int M>
void big_mult(ap_uint<W> a, ap_uint<W> b, ap_uint<2 * W> &m) {
  switch (SELECT) {
  case 1:
    big_mult_v1(a, b, m);
    break;
  case 2:
    big_mult_v2<W, M>(a, b, m);
    break;
  default:
    big_mult_v3<W, M>(a, b, m);
    break;
  };
}

template <int W, int M>
void big_mult(ap_uint<W> a, ap_uint<W> b, ap_uint<2 * W> &m) {
  big_mult<3, W, M>(a, b, m);
}

template <int W1, int W2>
void big_mult(ap_uint<W1> a, ap_uint<W2> b, ap_uint<W1 + W2> &m) {
  if (W2 <= 64) {
    // Decompose into 17x64 multipliers
    big_mult_v3small<W1, W2, 17>(a, b, m);
  } else {
    // const int W_MAX =(W1 > W2) ? W1 : W2;
//         const int tmp =(W1 > W2) ? W1 : W2;
//         const int mod = (tmp+ 16)/17;
//         const int W_MAX = 17*mod;
#define W_MAX (17 * (((W1 > W2) ? W1 : W2) + 16) / 17)
    ap_uint<W_MAX> a_m = a;
    ap_uint<W_MAX> b_m = b;
    ap_uint<2 * W_MAX> m_m;
    big_mult<W_MAX, 17>(a_m, b_m, m_m);
#undef W_MAX
    m = m_m;
  }
}

template <int W1, int I1, int W2, int I2>
void big_mult(ap_ufixed<W1, I1> a, ap_ufixed<W2, I2> b,
              ap_ufixed<W1 + W2, I1 + I2> &m) {
  ap_uint<W1> ai;
  ap_uint<W2> bi;
  ap_uint<W1 + W2> mi;
  ai(W1 - 1, 0) = a(W1 - 1, 0);
  bi(W2 - 1, 0) = b(W2 - 1, 0);
  big_mult(ai, bi, mi);
  m(W1 + W2 - 1, 0) = mi(W1 + W2 - 1, 0);
}

#endif

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
