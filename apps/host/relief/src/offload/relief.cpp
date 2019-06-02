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

int query[APP_QUERY_NUM][APP_FEATURE_DIM];

int gen_data() {
  for (int i = 0; i < APP_QUERY_NUM; i++) {
    for (int j = 0; j < APP_FEATURE_DIM; j++) {
      query[i][j] = rand() % 10;
    }
  }
}
void work(void) {
  gen_data();
  const char *virt_path = reg_virt_file("/mnt/relief_data.txt");
  int fd = vopen(virt_path, O_RDONLY);

  send_input_params(APP_THRES);

  send_input_params(APP_HIT_DATA_NUM);
  send_input_params(APP_MISS_DATA_NUM);

  for (int i = 0; i < APP_QUERY_NUM; i++) {
    for (int j = 0; j < APP_FEATURE_DIM; j++) {
      send_input_params((unsigned int)query[i][j]);
    }
  }

  int readBytes = 0;
  long long totalBytes = 0;
  unsigned char buf[READ_BUF_SIZE];
  bool fin = false;
  while (!fin) {
    readBytes = 0;
    while (readBytes != READ_BUF_SIZE) {
      int curReadBytes = vread(fd, &buf, READ_BUF_SIZE - readBytes);
      if (curReadBytes) {
        readBytes += curReadBytes;
      } else {
        fin = true;
        break;
      }
      totalBytes += readBytes;
    }
  }
  cout << "total bytes:" << totalBytes << endl;
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
