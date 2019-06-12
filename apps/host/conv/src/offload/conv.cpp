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

#define NDEBUG
// #define VERIFY
using namespace std;

int fd;
unsigned char buf[READ_BUF_SIZE];
char kernel_matrix[NUM_OF_KERNELS][KERNEL_SIZE];
//
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

void work(void) {
  long long totalBytes = 0;
  int readBytes = 0;
  bool fin = false;
  while (!fin) {
    readBytes = 0;
    while (readBytes != READ_BUF_SIZE) {
      int tmp = vread(fd, buf + readBytes, READ_BUF_SIZE - readBytes);
      if (tmp) {
        readBytes += tmp;
      } else {
        fin = true;
        break;
      }
    }
    totalBytes += readBytes;
#ifdef VERIFY
    for (int i = 0; i < readBytes; i++) {
      std::cout << std::hex << (int) buf[i] << " ";
    }
    std::cout << std::endl;
#endif
  }
  cout << "total bytes:" << totalBytes << endl;
  vclose(fd);
}

void send_param() {
  int kfd;
  if ((kfd = open("/mnt/centos/conv_kernel.txt", O_RDONLY)) < 0) {
    std::cout << "ERROR: cannot open file" << std::endl;
    exit(-1);
  }

  int read_bytes = 0;
  while (read_bytes != NUM_OF_KERNELS * KERNEL_SIZE) {
    int tmp_bytes = read(kfd, *kernel_matrix + read_bytes, NUM_OF_KERNELS * KERNEL_SIZE - read_bytes);
    if (!tmp_bytes) {
      if (read_bytes != NUM_OF_KERNELS * KERNEL_SIZE) 
        std::cout << "WARNING: insufficient kernel data!!" << std::endl;
      break;
    } else {
      read_bytes += tmp_bytes;
    }
  }

  for (int i = 0; i < NUM_OF_KERNELS; i++) {
    unsigned int engine_id = i / KERNEL_BATCH_SIZE;
    unsigned int engine_idx = i % KERNEL_BATCH_SIZE;
    for (int k = 0; k < KERNEL_SIZE; k++) {
      unsigned int data = (engine_id << 24) | (engine_idx << 16) | (k << 8) | (unsigned char) kernel_matrix[i][k];
      send_input_params(data);
    }
  }
}

void prepare() {
  const char *virt_path = reg_virt_file("/mnt/centos/conv_data.txt");
  fd = vopen(virt_path, O_RDONLY);
}

int main(int argc, char **argv) {
  struct timespec start;
  unsigned long finish;

  puts("prepare().....");
  start = timer_start();
  prepare();
  finish = timer_end(start);
  cout << "elapsed time = " << finish / 1000.0 / 1000.0 / 1000.0 << endl;

  puts("send_param().....");
  start = timer_start();
  send_param();
  finish = timer_end(start);
  cout << "elapsed time = " << finish / 1000.0 / 1000.0 / 1000.0 << endl;

  puts("work().....");
  start = timer_start();
  work();
  finish = timer_end(start);
  cout << "elapsed time = " << finish / 1000.0 / 1000.0 / 1000.0 << endl;

  return 0;
}
