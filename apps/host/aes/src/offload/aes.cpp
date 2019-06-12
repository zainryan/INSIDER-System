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

void send_params() {
  // send s_matrix
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j+=4) {
      send_input_params((s_matrix[i][j] << 24) | (s_matrix[i][j + 1] << 16) 
          | (s_matrix[i][j + 2] << 8) | (s_matrix[i][j + 3]));
    }
  }

  // send mix matrix
  for (int i = 0; i < 4; i++) {
    send_input_params((mix_matrix[i][0] << 0) | (mix_matrix[i][1] << 8) 
        | (mix_matrix[i][2] << 16) | (mix_matrix[i][3]) << 24);
  }

  // send key matrix
  for (int i = 0; i < 16; i+=4) {
    send_input_params((key_matrix[i + 0] << 0) | (key_matrix[i + 1] << 8) 
        | (key_matrix[i + 2] << 16) | (key_matrix[i + 3]) << 24);
  }
}

void work(void) {
  const char *virt_path = reg_virt_file("/mnt/centos/aes_data.txt");
  int fd = vopen(virt_path, O_RDONLY);

  send_params();

  long long total_read_bytes = 0;
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
    }
    total_read_bytes += readBytes;
  }
  std::cout << "total read bytes: " << total_read_bytes << std::endl;
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
