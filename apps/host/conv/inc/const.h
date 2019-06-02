#ifndef _CONST_H
#define _CONST_H

//#define NUM_OF_TOTAL_ROWS (524288L) // 32M
//#define NUM_OF_TOTAL_ROWS (33554432L) // 2G
#define NUM_OF_TOTAL_ROWS (1006632960L) // 60G
#define NUM_OF_KERNELS (32)
#define KERNEL_SIZE (64) // `char` each 
#define READ_BUF_SIZE (2 * 1024 * 1024)
#define KERNEL_BATCH_SIZE (2)
#endif
