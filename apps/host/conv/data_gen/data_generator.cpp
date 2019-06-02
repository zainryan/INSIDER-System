// the data generated based on following assumptions:
// kernel size: 8 * 8 * 4 (double)
// kernel number: 96
// conv stride: 8
// image size: 256 * 256 * 4 (unsigned char)
// no padding
// for simplicity use long long instead of double in kernels

#include "../inc/const.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

int main() {
  ios::sync_with_stdio(false);

  freopen("/data_new_apps/conv_kernel.txt", "w", stdout);
  // generate kernels
  for (int i = 0; i < NUM_OF_KERNELS; i++) {
    for (int j = 0; j < KERNEL_SIZE; j++) {
      char number  = (char) rand();
      cout << hex << number;
    }
  }
  cout << flush;

  // generate images
  freopen("/data_new_apps/conv_data.txt", "w", stdout);
  for (int n = 0; n < NUM_OF_TOTAL_ROWS; n++) {
    for (int i = 0; i < KERNEL_SIZE; i++) {
      unsigned char number = (unsigned char) rand();
      cout << hex << number;
    }
  } 
  cout << flush;
  return 0;
}
