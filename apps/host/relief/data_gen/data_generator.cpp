#include "../inc/const.h"
#include <cassert>
#include <climits>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <omp.h>
#include <unistd.h>
#include <vector>

using namespace std;

int rand_num() { return rand() % 10; }

int features[APP_HIT_DATA_NUM + APP_MISS_DATA_NUM][APP_FEATURE_DIM];
int query[APP_QUERY_NUM][APP_FEATURE_DIM];

int main() {
  freopen("/mnt/centos/relief_data.txt", "w", stdout);
  ios::sync_with_stdio(false);

  for (long long i = 0; i < APP_HIT_DATA_NUM + APP_MISS_DATA_NUM; i++) {
    for (long long j = 0; j < APP_FEATURE_DIM; j++) {
      features[i][j] = rand_num();
    }
  }

  char *feature_ptr_in_byte = (char *)features;
  for (long long i = 0; i < sizeof(features); i++) {
    cout << *feature_ptr_in_byte++;
  }

  feature_ptr_in_byte = (char *)features;
  for (long long i = 0; i < sizeof(features); i++) {
    cout << *feature_ptr_in_byte++;
  }

  return 0;
}
