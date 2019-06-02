#include "../../inc/const.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <queue>
#include <unistd.h>

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

const long long TRAIN_SET_SIZE =
    (APP_HIT_DATA_NUM + APP_MISS_DATA_NUM) * APP_FEATURE_DIM * sizeof(int);
int query[APP_QUERY_NUM][APP_FEATURE_DIM];
long long posiMin[APP_QUERY_NUM];
long long negMin[APP_QUERY_NUM];
long long dist[APP_QUERY_NUM];
int posiVec[APP_QUERY_NUM][APP_FEATURE_DIM];
int negVec[APP_QUERY_NUM][APP_FEATURE_DIM];
int weight[APP_FEATURE_DIM];
bool filterVec[APP_FEATURE_DIM];
volatile unsigned char expected_output_buf[TRAIN_SET_SIZE];
int *expected_buf_ptr_in_4B;
int validFeatureNum = 0;

int gen_data() {
  for (int i = 0; i < APP_QUERY_NUM; i++) {
    for (int j = 0; j < APP_FEATURE_DIM; j++) {
      query[i][j] = rand() % 10;
    }
  }
}
void init() {
  gen_data();
  for (int i = 0; i < APP_QUERY_NUM; i++) {
    posiMin[i] = LLONG_MAX;
    negMin[i] = LLONG_MAX;
  }
  for (int i = 0; i < APP_FEATURE_DIM; i++) {
    weight[i] = 0;
  }
  expected_buf_ptr_in_4B = (int *)expected_output_buf;
  validFeatureNum = 0;
}

void get_distance(const int *sample, long long *dist) {
  for (int i = 0; i < APP_QUERY_NUM; i++) {
    dist[i] = 0;
    for (int j = 0; j < APP_FEATURE_DIM; j++) {
      int diff = sample[j] - query[i][j];
      dist[i] += diff * diff;
    }
  }
}

void find_nearest(int *sample, long long *dist, bool posi) {
  for (int i = 0; i < APP_QUERY_NUM; i++) {
    if (posi) {
      if (dist[i] < posiMin[i]) {
        posiMin[i] = dist[i];
        for (int j = 0; j < APP_FEATURE_DIM; j++) {
          posiVec[i][j] = sample[j];
        }
      }
    } else {
      if (dist[i] < negMin[i]) {
        negMin[i] = dist[i];
        for (int j = 0; j < APP_FEATURE_DIM; j++) {
          negVec[i][j] = sample[j];
        }
      }
    }
  }
}

void update_weight() {
  for (int i = 0; i < APP_QUERY_NUM; i++) {
    for (int j = 0; j < APP_FEATURE_DIM; j++) {
      weight[j] += ((int)(negVec[i][j] == query[i][j])) -
                   ((int)(posiVec[i][j] == query[i][j]));
    }
  }
  for (int i = 0; i < APP_FEATURE_DIM; i++) {
    if (weight[i] < APP_THRES) {
      filterVec[i] = false;
    } else {
      filterVec[i] = true;
      validFeatureNum++;
    }
  }
}
void filter(int *sample) {
  for (int i = 0; i < APP_FEATURE_DIM; i++) {
    if (filterVec[i]) {
      cout << (int)sample[i] << " ";
      *expected_buf_ptr_in_4B = sample[i];
      expected_buf_ptr_in_4B++;
    }
  }
  cout << endl;
}

inline void read_filte() {
  int fd = open("/mnt/relief_data.txt", O_RDWR);
  char read_buf[READ_BUF_SIZE];
  int read_cnt = -1;
  while (read_cnt != TRAIN_SET_SIZE / READ_BUF_SIZE) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int curReadBytes = read(fd, read_buf, READ_BUF_SIZE - read_bytes);
      if (curReadBytes == 0)
        break;
      read_bytes += curReadBytes;
    }
    read_cnt++;
    int *read_buf_ptr_in_4B = (int *)read_buf;
    cout << "total read bytes is: " << read_bytes << endl;
    for (int i = 0; i < read_bytes / APP_FEATURE_DIM / sizeof(int) / 2;
         i++) { // should change if multiple time read
      cout << i << endl;
      filter(read_buf_ptr_in_4B);
      read_buf_ptr_in_4B += APP_FEATURE_DIM;
    }
  }
  close(fd);
}

void read_train(void) {
  int fd = open("/mnt/relief_data.txt", O_RDWR);
  char read_buf[READ_BUF_SIZE];
  int read_cnt = -1;
  while (read_cnt != TRAIN_SET_SIZE / READ_BUF_SIZE) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int curReadBytes = read(fd, read_buf, READ_BUF_SIZE - read_bytes);
      if (curReadBytes == 0)
        break;
      read_bytes += curReadBytes;
    }
    read_cnt++;
    int *read_buf_ptr_in_4B = (int *)read_buf;
    for (int i = 0; i < read_bytes / APP_FEATURE_DIM / sizeof(int) / 2; i++) {
      get_distance(read_buf_ptr_in_4B, dist);
      find_nearest(read_buf_ptr_in_4B, dist,
                   (read_cnt < (TRAIN_SET_SIZE >> 1)));
      read_buf_ptr_in_4B += APP_FEATURE_DIM;
    }
  }
  update_weight();
  cout << "DEBUG: Expected data size is: "
       << validFeatureNum * (APP_HIT_DATA_NUM + APP_MISS_DATA_NUM) * sizeof(int)
       << endl;

  close(fd);
}

int main(int argc, char **argv) {
  init();
  puts("read_train().....");
  Timer timer;
  read_train();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("read_filte().....");
  timer.reset();
  read_filte();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
