#include "../inc/constant.h"

#include <cassert>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <omp.h>
#include <random>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM_THREADS (32)
#define MAX_WRITE_CHUNK_SIZE (2 * 1024 * 1024)
#define min(a, b) ((a) < (b) ? (a) : (b))

using namespace std;

int fd;
mt19937 mt_rands[NUM_THREADS];
unsigned char write_buf[MAX_WRITE_CHUNK_SIZE];

void write_chunk(unsigned char *buf, unsigned long long size) {
  unsigned long long written_size = 0;
  while (size != written_size) {
    unsigned long long tmp =
        write(fd, write_buf + written_size, size - written_size);
    if (!tmp) {
      cerr << "Error in persisting chunk to drive!" << endl;
      exit(-1);
    }
    written_size += tmp;
  }
}

int main() {
  fd = open("/data_new_apps/gzip_input.txt", O_CREAT | O_WRONLY);

  for (int i = 0; i < NUM_THREADS; i++) {
    mt_rands[i] = mt19937(time(0));
  }

  unsigned long long total_write_bytes = INPUT_SIZE;
  while (total_write_bytes) {
    unsigned long long write_chunk_size =
        min(total_write_bytes, MAX_WRITE_CHUNK_SIZE);
    int idx = 0;
    while (idx < write_chunk_size) {
      unsigned char data =
          (unsigned char)mt_rands[omp_get_thread_num()]() % (MAX_VALUE + 1);
      unsigned char repeat_num =
          (unsigned char)mt_rands[omp_get_thread_num()]() %
          (MAX_REPEAT_NUM + 1);
      for (int i = idx; i < min(idx + repeat_num, write_chunk_size); i++) {
        write_buf[i] = data;
      }
      idx += repeat_num;
    }
    total_write_bytes -= write_chunk_size;
    write_chunk(write_buf, write_chunk_size);
  }

  return 0;
}
