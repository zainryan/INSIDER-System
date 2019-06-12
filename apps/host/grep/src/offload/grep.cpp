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
//#define NDEBUG

using namespace std;

int fd;

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

char random_char() {
  int min_ascii = 65;
  int max_ascii = 68;
  return min_ascii + rand() % (max_ascii - min_ascii + 1);
}

void generate_compared_str() {
  const char *virt_path = reg_virt_file("/mnt/centos/grep_input.txt");
  fd = vopen(virt_path, O_RDONLY);
  char param_str[MAX_GREP_PARAM_SIZE];
  send_input_params(PARAM_STR_LEN);
  for (int i = 0; i < MAX_GREP_PARAM_SIZE; i++) {
    if (i < PARAM_STR_LEN) {
      param_str[i] = random_char();
    } else {
      param_str[i] = 0;
    }
  }
  for (int i = 0; i < (PARAM_STR_LEN + POKE_WIDTH_BYTES - 1) / POKE_WIDTH_BYTES;
       i++) {
    int idx = i * POKE_WIDTH_BYTES;
    unsigned int param_str_snippet =
        (param_str[idx] << 24) | (param_str[idx + 1] << 16) |
        (param_str[idx + 2] << 8) | param_str[idx + 3];
    send_input_params(param_str_snippet);
  }
}

void work() {
  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  bool fin_file = false;
  long long total_bytes = 0;
  while (!fin_file) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int tmp = vread(fd, buf, READ_BUF_SIZE - read_bytes);
      if (!tmp) {
        fin_file = true;
        break;
      } else {
        read_bytes += tmp;
      }
    }
    total_bytes += read_bytes;
    assert((read_bytes & 7) == 0);
    for (int i = 0; i < (read_bytes >> 3); i++) {
      int idx = i << 3;
      int row = buf[idx++] | (buf[idx++] << 8) | (buf[idx++] << 16) |
                (buf[idx++] << 24);
      int col = buf[idx++] | (buf[idx++] << 8) | (buf[idx++] << 16) |
                (buf[idx++] << 24);
      // cout << "Matched at row = " << row << " col = " << col << endl;
    }
  }
  vclose(fd);
  free(buf);
  cout << "total bytes is : " << total_bytes << endl;
}

int main(int argc, char **argv) {
  puts("generate_compared_str().....");
  Timer timer;
  generate_compared_str();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("work().....");
  work();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
