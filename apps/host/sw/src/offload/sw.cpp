#include "../../inc/sw.h"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <insider_runtime.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <omp.h>
#include <queue>
#include <unistd.h>

#define READ_BUF_SIZE (2 * 1024 * 1024)

using namespace std;

unsigned char read_buf[READ_BUF_SIZE];

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

void generate_params() {
  send_input_params(ITER_CNT);
}

void prepare() {
  const char *virt_path = reg_virt_file("/mnt/centos/sw_input.txt");
  fd = vopen(virt_path, O_RDONLY);
}

void execute() {
  unsigned char *buf = (unsigned char *)malloc(READ_BUF_SIZE);
  unique_ptr<unsigned char> buf_gc = unique_ptr<unsigned char>(buf);
  bool fin_file = false;
  unsigned long long total_read_bytes = 0;
  while (!fin_file) {
    unsigned long long read_bytes = 0;
    int tmp = vread(fd, buf + read_bytes, READ_BUF_SIZE - read_bytes);
    if (!tmp) {
      fin_file = true;
      break;
    } else {
      read_bytes += tmp;
    }
    total_read_bytes += read_bytes;
  }
  vclose(fd);
  free(buf);
  cout << "total read bytes is: " << total_read_bytes << endl;
}

int main(int argc, char **argv) {
  puts("prepare().....");
  Timer timer;
  prepare();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("generate_params().....");
  timer.reset();
  generate_params();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("execute().....");
  timer.reset();
  execute();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
