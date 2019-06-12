#include "../inc/const.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

int main() {
  ios::sync_with_stdio(false);
  freopen("/mnt/centos/statistical_data.txt", "w", stdout);

  for (int i = 0; i < ROW_NUM; i++) {
    for (int j = 0; j < COL_NUM / sizeof(unsigned int); j++) {
      unsigned int number = rand();
      char *number_in_bytes = (char *)&number;
      for (int k = 0; k < sizeof(unsigned int); k++) {
        cout << hex << number_in_bytes[k];
      }
    }
  }

  return 0;
}
