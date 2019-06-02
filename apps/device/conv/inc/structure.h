#pragma once 

#include<ap_int.h>

typedef char Elem;
typedef short Result_Elem; 
//
// be careful to make all numbers blow a powe of 2
#define DBLOCK_SIZE_IN_BYTE (64)
#define DBLOCK_SIZE_IN_BIT (8 * DBLOCKS_SIZE_IN_BYTE)
#define NUM_OF_ENGINES (16)



#define KERNEL_SIZE (64)   // type of 'Elem'
#define DBLOCKS_PER_KERNEL (KERNEL_SIZE * sizeof(Elem) / DBLOCK_SIZE_IN_BYTE) // (1)
#define LOG_DBLOCKS_PER_KERNEL (0)  
#define DBLOCKS_PER_PARAM (DBLOCKS_PER_KERNEL)   
#define ELEMS_PER_DBLOCK (DBLOCK_SIZE_IN_BYTE / sizeof(Elem)) //(64)
#define KERNEL_BATCH_SIZE (2)
#define NUM_OF_KERNELS (NUM_OF_ENGINES * KERNEL_BATCH_SIZE)


#ifdef CSIM
#define NUM_OF_TOTAL_ROW (128) // NUM_OF_MATRIX * MATRIX_HEIGHT)
#endif


struct APP_Mult_Data {
 ap_uint<512> data;
 bool eop;
};

struct APP_Reduce_Data {
 // ap_uint<256> data;
  ap_uint<KERNEL_BATCH_SIZE * sizeof(Result_Elem) * 8> data; // 64
  unsigned char id;
  bool eop;
};

struct APP_Combine_L0_Data {
  ap_uint<2 * KERNEL_BATCH_SIZE * sizeof(Result_Elem) * 8> data; // 128
  bool eop;
};

struct APP_Combine_L1_Data {
  ap_uint<4 * KERNEL_BATCH_SIZE * sizeof(Result_Elem) * 8> data; //256
  bool eop;
};

struct APP_Combine_L2_Data {
  ap_uint<8 * KERNEL_BATCH_SIZE * sizeof(Result_Elem) * 8> data; // 512
  bool eop;
};

struct APP_Param {
  unsigned char idx_blk;
  unsigned char idx_dim_0;
  unsigned char idx_dim_1;
  unsigned char val;
};
