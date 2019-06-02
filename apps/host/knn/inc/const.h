#ifndef CONST_H
#define CONST_H

#define NUM_TRAIN_CASES (14680064) // should be the multiples of READ_BUF_SIZE
#define NUM_PREDICTING_CASES (8)
#define FEATURE_DIM (4096)
#define NUM_RESULTS (32)
#define MAX_FEATURE_WEIGHT (10)
#define RESULT_SIZE (64)
#define WEIGHT_SIZE (1)
#define DATA_GEN_NUM_THREADS (16)
#define COMPUTATION_NUM_THREADS (8)
#define BUS_WIDTH (64)
#define READ_BUF_SIZE (1024 * 1024 * 2)
#define PARAM_K (10)

#endif
