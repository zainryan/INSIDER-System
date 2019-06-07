#include <fcntl.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <insider_runtime.hpp>
#include <iostream>
#include <map>
#include <queue>
#include "../../inc/const.h"

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

void run() {
  int r_fd = open("/mnt/centos/rle_data.txt", O_RDONLY);
  const char *virt_path = reg_virt_file("/mnt/centos/output.txt");
  int w_fd = vopen(virt_path, O_WRONLY);

  if (r_fd && w_fd) {
    // read the header & check correctness
    bitmap_header *header = (bitmap_header *)malloc(sizeof(bitmap_header));
    long long input_read_bytes = read(r_fd, header, sizeof(bitmap_header));
    print_header(header);

    if (header->magicNumber[0] != 'B' || header->magicNumber[1] != 'M' ||
        header->headerSize != sizeof(bitmap_header) ||
        header->depth != OUTPUT_DEPTH || !header->compression) {
      fprintf(stderr, "Error: Incompatible file type\n");
      exit(-1);
    }

    while (1) {
      size_t input_read_bytes = read(r_fd, buf, READ_BUF_SIZE);
      if (input_read_bytes == 0) {
        break;
      } else if (input_read_bytes < 0) {
        perror("Read failed.");
        exit(-1);
      } else {
        size_t virt_write_bytes = 0;
        while (virt_write_bytes != input_read_bytes) {
          size_t delta_bytes = vwrite(w_fd, buf + virt_write_bytes,
                                      input_read_bytes - virt_write_bytes);
          if (delta_bytes <= 0) {
            std::cout << "Error: delta_bytes <= 0" << std::endl;
            goto finish;
          }
          virt_write_bytes += delta_bytes;
        }
      }
    }
  }
finish:
  if (w_fd) {
    size_t real_file_written_size;
    vclose(w_fd, &real_file_written_size);
    std::cout << "real_file_written_size = " << real_file_written_size << std::endl;
  }
  if (r_fd) {
    close(r_fd);
  }
}

int main(int argc, char **argv) {
  struct timespec start = timer_start();
  run();
  unsigned long finish = timer_end(start);
  cout << "elapsed time = " << finish / 1000.0 / 1000.0 / 1000.0 << endl;

  return 0;
}
