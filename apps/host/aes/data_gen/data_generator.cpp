#include "../inc/const.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

int main() {
  ios::sync_with_stdio(false);
  freopen("/mnt/centos/aes_data.txt", "w", stdout);

  for (unsigned long long i = 0; i < DATA_SIZE; i++) {
    unsigned char number = (unsigned char) rand();
    cout << hex << number;
  } 
  return 0;
}
