#include "../../inc/const.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <queue>
#include <unistd.h>

#define c2i(c) ((c) - '0')
//#define NDEBUG

using namespace std;

const int NUMBER_PER_ROW = COL_NUM / sizeof(unsigned int);

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

unsigned int input_buf[ROW_NUM][NUMBER_PER_ROW];

void read_data(void) {
  int fd = open("/mnt/centos/statistical_data.txt", O_RDWR);

  unsigned int *input_buf_ptr = (unsigned int *)&input_buf;
  char read_buf[READ_BUF_SIZE];
  int read_cnt = 0;
  while (read_cnt != (unsigned long long)COL_NUM * ROW_NUM / READ_BUF_SIZE) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      read_bytes += read(fd, read_buf, READ_BUF_SIZE - read_bytes);
    }
    read_cnt++;
    unsigned int *read_buf_ptr_in_4B = (unsigned int *)read_buf;
    for (int i = 0; i < READ_BUF_SIZE / sizeof(unsigned int); i++) {
      *(input_buf_ptr++) = read_buf_ptr_in_4B[i];
    }
  }

  close(fd);
}

void calculation() {
  for (int i = 0; i < ROW_NUM; i++) {
    unsigned int v_max = 0, v_min = INT_MAX;
    unsigned long long v_sum = 0;
    for (int j = 0; j < NUMBER_PER_ROW; j++) {
      unsigned int cur = input_buf[i][j];
      v_max = max(v_max, cur);
      v_min = min(v_min, cur);
      v_sum += cur;
    }
    // cout << "v_max = " << v_max << ", v_min = " << v_min << " , v_avg = " <<
    // v_sum / NUMBER_PER_ROW << endl;
  }
}

int main(int argc, char **argv) {
  puts("read_data().....");
  Timer timer;
  read_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("calculation().....");
  timer.reset();
  calculation();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
