#include "fpga_util.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

double get_wall_time() {
  struct timeval time;
  if (gettimeofday(&time, NULL)) {
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

double get_cpu_time() { return (double)clock() / CLOCKS_PER_SEC; }

void *producer(void *) {
  double wall0 = get_wall_time();
  for (int i = 0; i < 1000000; i++) {
    unsigned int *input_buf0 = (unsigned int *)get_free_buf();
    for (unsigned int i = 0; i < BUF_SIZE / 4; i++) {
      input_buf0[i] = i;
    }
    commit_buf();
  }
  double wall1 = get_wall_time();
  std::cout << wall1 - wall0 << std::endl;
  return NULL;
}

void *consumer(void *) {
  while (true) {
    unsigned int *output_buf = (unsigned int *)get_data_buf();
    for (unsigned int i = 0; i < BUF_SIZE / 4; i++) {
      if (output_buf[i] != i + 1) {
        std::cout << output_buf[i] << " " << i + 1 << std::endl;
        std::cout << output_buf[i + 1] << " " << i + 2 << std::endl;
        std::cout << output_buf[i + 2] << " " << i + 3 << std::endl;
        std::cout << output_buf[i + 3] << " " << i + 4 << std::endl;
      }
      assert(output_buf[i] == i + 1);
    }
    release_buf();
  }
  return NULL;
}

void work() {
  init_buf_pools(16, 16);
  pthread_t producer_thread;
  pthread_t consumer_thread;
  pthread_create(&producer_thread, NULL, producer, NULL);
  pthread_create(&consumer_thread, NULL, consumer, NULL);
  while (1) {
  }
}

int main() {
  execute_program(work);

  return 0;
}
