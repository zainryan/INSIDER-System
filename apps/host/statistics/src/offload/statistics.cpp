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

#define max(a, b) ((a) > (b) ? (a) : (b))
//#define NDEBUG

using namespace std;

char buf[READ_BUF_SIZE];

// call this function to start a nanosecond-resolution timer
struct timespec timer_start() {
  struct timespec start_time;
  clock_gettime(CLOCK_REALTIME, &start_time);
  return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time) {
  struct timespec end_time;
  clock_gettime(CLOCK_REALTIME, &end_time);
  long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 +
                     (end_time.tv_nsec - start_time.tv_nsec);
  return diffInNanos;
}

unsigned long long total = 0;

inline void calculation(int readBytes) {
  unsigned int *buffer_ptr_in_4B = (unsigned int *)buf;
  for (int i = 0; i < readBytes / sizeof(unsigned int) / 4; i++) {
    unsigned int v_max = *buffer_ptr_in_4B++;
    unsigned int v_min = *buffer_ptr_in_4B++;
    unsigned int v_sum_l = (unsigned int)*buffer_ptr_in_4B++;
    unsigned int v_sum_h = (unsigned int)*buffer_ptr_in_4B++;
    unsigned long long v_avg =
        ((((unsigned long long)v_sum_h) << 32) | v_sum_l) /
        (COL_NUM / sizeof(unsigned int));
    total += (v_max + v_min + v_avg);
    // cout << "v_max = " << v_max << ", v_min = " << v_min << " , v_avg = " <<
    // v_avg << endl;
  }
}

void work(void) {
  const char *virt_path = reg_virt_file("/mnt/centos/statistical_data.txt");
  int fd = vopen(virt_path, O_RDONLY);

  send_input_params(COL_NUM);

  int readBytes = 0;
  bool fin = false;
  while (!fin) {
    readBytes = 0;
    while (readBytes != READ_BUF_SIZE) {
      int tmp = vread(fd, &buf, READ_BUF_SIZE - readBytes);
      if (tmp) {
        readBytes += tmp;
      } else {
        fin = true;
        break;
      }
      calculation(readBytes);
    }
  }
  cout << total << endl;
  vclose(fd);
}

int main(int argc, char **argv) {
  puts("work().....");
  struct timespec start = timer_start();
  work();
  unsigned long finish = timer_end(start);
  cout << "elapsed time = " << finish / 1000.0 / 1000.0 / 1000.0 << endl;

  return 0;
}
