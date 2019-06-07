#include "../../inc/const.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <queue>
#include <unistd.h>

#include <insider_runtime.hpp>
#include <stdio.h>
#include <time.h>
#define c2i(c) (c - '0')
#define max(a, b) ((a) > (b) ? (a) : (b))
#define BUF_LEN (2 * 1024 * 1024)
//#define NDEBUG

using namespace std;

class Timer {
public:
  Timer() : m_beg(clock_::now()) {}
  void reset() { m_beg = clock_::now(); }

  double elapsed() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(clock_::now() -
                                                                 m_beg)
        .count();
  }

private:
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1>> second_;
  std::chrono::time_point<clock_> m_beg;
};

unsigned char predicting_vec[FEATURE_DIM];

struct DisPair {
  unsigned int distance;
  unsigned int result;
  DisPair() {}
  DisPair(unsigned int _distance, unsigned int _result)
      : distance(_distance), result(_result) {}
  bool operator<(const DisPair &w) const { return distance < w.distance; }
};
priority_queue<DisPair> max_heap;
int fd;
void gen_predicting_data(void) {
  const char *virt_path = reg_virt_file("/mnt/knn_data.txt");
  fd = vopen(virt_path, O_RDONLY);
  for (int i = 0; i < FEATURE_DIM; i++) {
    send_input_params(rand() % MAX_FEATURE_WEIGHT);
  }
}

void prediction(void) {

  int cur = 0;
  int *buf = (int *)malloc(BUF_LEN);
  while (cur = vread(fd, buf, BUF_LEN)) {
    int pair_num = cur / sizeof(int) / 2;
    for (int i = 0; i < pair_num; i++) {
      DisPair disPair;
      disPair.distance = buf[2 * i];
      disPair.result = buf[2 * i + 1];
      if (max_heap.size() < PARAM_K) {
        max_heap.push(disPair);
      } else {
        if (disPair.distance < max_heap.top().distance) {
          max_heap.pop();
          max_heap.push(disPair);
        }
      }
    }
  }

  map<unsigned int, unsigned int> freq_map;
  while (!max_heap.empty()) {
    DisPair disPair = max_heap.top();
    max_heap.pop();
    freq_map[disPair.result]++;
  }
  unsigned int max_freq = 0;
  unsigned int prediction_result;
  for (auto mit = freq_map.begin(); mit != freq_map.end(); mit++) {
    if (mit->second > max_freq) {
      max_freq = mit->second;
      prediction_result = mit->first;
    }
  }
  cout << "result = " << prediction_result << endl;
  free(buf);

  vclose(fd);
}

int main(int argc, char **argv) {
  puts("gen_predicting_data().....");
  Timer timer;
  gen_predicting_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("prediction().....");
  timer.reset();
  prediction();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
