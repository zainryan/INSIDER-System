#pragma once

#include <ap_int.h>

#define NUM_OF_PAIRS (32)

struct APP_Expand_Data {
  unsigned short prefix_sum[NUM_OF_PAIRS + 1];
  char alpha_array[NUM_OF_PAIRS];
  bool eop;
  unsigned char len; // mark valid number of pairs
};
