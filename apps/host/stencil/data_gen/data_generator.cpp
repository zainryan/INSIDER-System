#include "../inc/common.h"
#include "../inc/constant.h"
#include "../inc/structure.h"

#include <cassert>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <omp.h>
#include <random>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM_THREADS (16)

using namespace std;

Point input_pictures[PADDING_HEIGHT + PICTURE_HEIGHT]
                    [PADDING_WIDTH + PICTURE_WIDTH];
mt19937 mt_rands[NUM_THREADS];

int main() {
  int fd = open("/mnt/centos/stencil_input.txt", O_CREAT | O_WRONLY);
  for (int i = 0; i < NUM_THREADS; i++) {
    mt_rands[i] = mt19937(time(0));
  }

  std::cout << "Start to generate data in memory..." << std::endl;
#pragma omp parallel for num_threads(NUM_THREADS)
  for (int i = 0; i < PADDING_HEIGHT + PICTURE_HEIGHT; i++) {
    for (int j = 0; j < PADDING_WIDTH + PICTURE_WIDTH; j++) {
      if (i >= PADDING_HEIGHT && j >= PADDING_WIDTH) {
        input_pictures[i][j] =
            host_uint_to_point(mt_rands[omp_get_thread_num()]());
      } else {
        input_pictures[i][j] = host_uint_to_point(0);
      }
    }
  }

  std::cout << "Start to persist the generated data to drive..." << std::endl;
  ssize_t total_write_bytes = sizeof(input_pictures);
  ssize_t written_bytes = 0;
  while (total_write_bytes != written_bytes) {
    ssize_t delta_written_bytes =
        write(fd, (char *)input_pictures + written_bytes,
              total_write_bytes - written_bytes);
    written_bytes += delta_written_bytes;
  }

  return 0;
}
