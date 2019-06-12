#include "../inc/const.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

int random(int upper, unsigned int *tid) { return rand_r(tid) % upper; }

string supply_leadings(string orig, int len) {
  assert(orig.size() <= len);
  while (orig.size() < len) {
    orig = "0" + orig;
  }
  return orig;
}

string random_result(unsigned int *tid) {
  string ret = to_string(random(NUM_RESULTS, tid));
  return supply_leadings(ret, RESULT_SIZE);
}

string random_weight(unsigned int *tid) {
  string ret = to_string(random(MAX_FEATURE_WEIGHT, tid));
  return supply_leadings(ret, WEIGHT_SIZE);
}

int main() {
  freopen("/mnt/centos/knn_data.txt", "w", stdout);
  ios::sync_with_stdio(false);

  omp_set_num_threads(DATA_GEN_NUM_THREADS);
#pragma omp parallel
  {
    unsigned int tid = omp_get_thread_num();
    for (int i = 0; i < NUM_TRAIN_CASES / DATA_GEN_NUM_THREADS; i++) {
      string result = random_result(&tid);
      vector<string> feature_vec;
      for (int j = 0; j < FEATURE_DIM; j++) {
        feature_vec.push_back(random_weight(&tid));
      }
#pragma omp critical
      {
        cout << result;
        for (auto &str : feature_vec) {
          cout << str;
        }
      }
    }
  }
  return 0;
}
