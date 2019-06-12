#include "../../inc/common.h"
#include "../../inc/constant.h"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <queue>
#include <unistd.h>

#define READ_BUF_SIZE (2 * 1024 * 1024)

using namespace std;

Point stencil[STENCIL_HEIGHT][STENCIL_WIDTH];
Point input_pictures[PADDING_HEIGHT + PICTURE_HEIGHT]
                    [PADDING_WIDTH + PICTURE_WIDTH];

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
  int fd = open("/mnt/centos/stencil_input.txt", O_RDONLY);
  char *read_buf = (char *)&input_pictures[0][0];
  unsigned long long total_read_bytes = 0;

  while (total_read_bytes !=
         ((unsigned long long)sizeof(Point)) * PICTURE_WIDTH * PICTURE_HEIGHT) {
    unsigned long long read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int tmp = read(fd, read_buf + read_bytes, READ_BUF_SIZE - read_bytes);
      if (!tmp) {
        goto finish_reading;
      }
      read_bytes += tmp;
    }
    read_buf += read_bytes;
    total_read_bytes += read_bytes;
  }

finish_reading:
  close(fd);
}

void generate_params() {
  for (int i = 0; i < STENCIL_WIDTH; i++) {
    for (int j = 0; j < STENCIL_WIDTH; j++) {
      stencil[i][j] = host_uint_to_point(rand());
    }
  }
}

void computing() {
#pragma omp parallel for
  for (int i = 0; i < PICTURE_HEIGHT; i++) {
    for (int j = 0; j < PICTURE_WIDTH; j++) {
      volatile Point result;
      result.red = result.green = result.blue = result.alpha = 0;
      for (int k = 0; k < STENCIL_HEIGHT; k++) {
        for (int p = 0; p < STENCIL_WIDTH; p++) {
          add(result,
              stencil[k][p] *
                  input_pictures[i + PADDING_HEIGHT - (STENCIL_HEIGHT - 1 - k)]
                                [j + PADDING_WIDTH - (STENCIL_WIDTH - 1 - p)]);
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  Timer timer;
  puts("generate_params().....");
  timer.reset();
  generate_params();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("read_data().....");
  read_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("computing().....");
  timer.reset();
  computing();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
