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

//#define NDEBUG

using namespace std;

char param_str[MAX_GREP_PARAM_SIZE];
char buf[FILE_ROW_NUM][FILE_COL_NUM];
int result[FILE_ROW_NUM];

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

void read_data(void) {
  int fd = open("/mnt/grep_input.txt", O_RDONLY);
  char *read_buf = (char *)&buf;
  int read_buf_cnt = 0;

  while (read_buf_cnt !=
         (unsigned long long)FILE_ROW_NUM * FILE_COL_NUM / READ_BUF_SIZE) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      read_bytes += read(fd, read_buf, READ_BUF_SIZE - read_bytes);
    }
    read_buf += READ_BUF_SIZE;
    read_buf_cnt++;
  }

  close(fd);
}

char random_char() {
  int min_ascii = 65;
  int max_ascii = 68;
  return min_ascii + rand() % (max_ascii - min_ascii + 1);
}

void generate_compared_str() {
  for (int i = 0; i < MAX_GREP_PARAM_SIZE; i++) {
    if (i < PARAM_STR_LEN) {
      param_str[i] = random_char();
    } else {
      param_str[i] = 0;
    }
  }
}

void matching() {
  for (int i = 0; i < FILE_ROW_NUM; i++) {
    result[i] = -1;
    for (int j = 0; j < FILE_COL_NUM; j++) {
      bool matched = true;
      if (j + PARAM_STR_LEN <= FILE_COL_NUM) {
        for (int k = 0; k < PARAM_STR_LEN; k++) {
          matched &= (param_str[k] == buf[i][j + k]);
        }
        if (matched) {
          result[i] = j;
          // cout << "Matched at i = " << i << ", j = " << j << endl;
          break;
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  puts("read_data().....");
  Timer timer;
  read_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("generate_compared_str().....");
  timer.reset();
  generate_compared_str();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("matching().....");
  timer.reset();
  matching();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
