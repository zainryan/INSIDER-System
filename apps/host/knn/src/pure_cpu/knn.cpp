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

#define c2i(c) (c - '0')
#define max(a, b) ((a) > (b) ? (a) : (b))
//#define NDEBUG

using namespace std;

unsigned char result[NUM_TRAIN_CASES];

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

unsigned char feature_vec[NUM_TRAIN_CASES][FEATURE_DIM],
    predicting_vec[NUM_PREDICTING_CASES][FEATURE_DIM];

unsigned int distance_arr[NUM_PREDICTING_CASES][NUM_TRAIN_CASES];

struct DisPair {
  unsigned int distance;
  unsigned int result;
  DisPair(unsigned int _distance, unsigned int _result)
      : distance(_distance), result(_result) {}
  bool operator<(const DisPair &w) const { return distance < w.distance; }
};
priority_queue<DisPair> max_heap[NUM_PREDICTING_CASES];

void read_data(void) {
  int fd = open("/mnt/centos/knn_data.txt", O_RDWR);

  char buf[READ_BUF_SIZE];
  int state = 0;
  int data_idx = 0;
  int feature_idx = 0;
  int ten_exp_table[WEIGHT_SIZE];

  ten_exp_table[0] = 1;
  int i;
  for (i = 1; i < WEIGHT_SIZE; i++) {
    ten_exp_table[i] = ten_exp_table[i - 1] * 10;
  }

  while (1) {
    int read_len = 0;
    int tmp_len;
    while (read_len != READ_BUF_SIZE &&
           (tmp_len = read(fd, buf, READ_BUF_SIZE - read_len))) {
      read_len += tmp_len;
    }
    if (!tmp_len) {
      break;
    }
    int k;
    for (k = 0; k < READ_BUF_SIZE / BUS_WIDTH; k++) {
      char *buf_ptr = buf + k * BUS_WIDTH;
      if (state == 0) {
        // result
        result[data_idx] =
            c2i(buf_ptr[BUS_WIDTH - 2]) * 10 + c2i(buf_ptr[BUS_WIDTH - 1]);
        state++;
      } else {
        // feature
        for (i = 0; i < BUS_WIDTH / WEIGHT_SIZE; i++) {
          int buf_idx = i * WEIGHT_SIZE;
          unsigned char weight = 0;
          for (int j = 0; j < WEIGHT_SIZE; j++) {
            weight +=
                c2i(buf_ptr[buf_idx + j]) * ten_exp_table[WEIGHT_SIZE - 1 - j];
          }
          feature_vec[data_idx][feature_idx++] = weight;
        }
        if (state == FEATURE_DIM * WEIGHT_SIZE / BUS_WIDTH) {
          assert(feature_idx == FEATURE_DIM);
          state = 0;
          feature_idx = 0;
          data_idx++;
        } else {
          state++;
        }
      }
    }
  }
  assert(data_idx == NUM_TRAIN_CASES);

  close(fd);
}

void gen_predicting_data(void) {
  int i, j;
  for (i = 0; i < NUM_PREDICTING_CASES; i++) {
    for (j = 0; j < FEATURE_DIM; j++) {
      predicting_vec[i][j] = rand() % MAX_FEATURE_WEIGHT;
    }
  }
}

inline int sqr(int x) { return x * x; }

void calc_distance(void) {
  int i, j, k;
  omp_set_num_threads(COMPUTATION_NUM_THREADS);
#pragma omp parallel for schedule(dynamic, 8) collapse(2)
  for (k = 0; k < FEATURE_DIM; k++) {
    for (i = 0; i < NUM_TRAIN_CASES; i++) {
      for (j = 0; j < NUM_PREDICTING_CASES; j++) {
        distance_arr[j][i] += sqr(feature_vec[i][k] - predicting_vec[j][k]);
      }
    }
  }
}

void prediction(void) {
  int i, j;
  omp_set_num_threads(COMPUTATION_NUM_THREADS);
#pragma omp parallel for schedule(dynamic)
  for (i = 0; i < NUM_PREDICTING_CASES; i++) {
    for (j = 0; j < NUM_TRAIN_CASES; j++) {
      int cur_dis = distance_arr[i][j];
      if (max_heap[i].size() < PARAM_K) {
        max_heap[i].emplace(cur_dis, result[j]);
      } else {
        if (cur_dis < max_heap[i].top().distance) {
          max_heap[i].pop();
          max_heap[i].emplace(cur_dis, result[j]);
        }
      }
    }
    map<unsigned int, unsigned int> freq_map;
    while (!max_heap[i].empty()) {
      DisPair disPair = max_heap[i].top();
      max_heap[i].pop();
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
#pragma omp critical
    { cout << "result = " << prediction_result << " for i = " << i << endl; }
  }
}

int main(int argc, char **argv) {
  puts("read_data().....");
  Timer timer;
  read_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("gen_predicting_data().....");
  timer.reset();
  gen_predicting_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("calc_distance().....");
  timer.reset();
  calc_distance();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("prediction().....");
  timer.reset();
  prediction();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
