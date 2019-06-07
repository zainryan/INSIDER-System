#include "../../inc/const.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <queue>
#include <unistd.h>

using namespace std;

#define BUF_NUM (32)
// pre-allocate 16 GB memory for input data
#define INPUT_BUF_SIZE (17179869184LL)
// pre-allocate 2 GB for each output buf
#define OUTPUT_BUF_SIZE (2147483648LL)

unsigned char input_buf[INPUT_BUF_SIZE];
unsigned char output_bufs[BUF_NUM][OUTPUT_BUF_SIZE];
unsigned long long input_valid_bytes;
unsigned long long output_valid_bytes[BUF_NUM];

unsigned char write_buf[WRITE_BUF_SIZE];

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

void decoder() {
  int tid = omp_get_thread_num();
  unsigned long long write_bytes = 0;
  unsigned char *input_buf_ptr =
      input_buf + (input_valid_bytes / 2 / BUF_NUM) * tid * 2;

  unsigned long long iter_upper_bound = (input_valid_bytes / 2 / BUF_NUM) * 2;
  if (tid == BUF_NUM - 1) {
    iter_upper_bound += input_valid_bytes - BUF_NUM * iter_upper_bound;
  }

  for (unsigned long long i = 0; i < iter_upper_bound; i += 2) {
    unsigned char len = input_buf_ptr[i];
    unsigned char alpha = input_buf_ptr[i + 1];
    for (int j = 0; j < len; j++) {
      output_bufs[tid][write_bytes + j] = alpha;
    }
    write_bytes += len;
  }
  output_valid_bytes[tid] = write_bytes;
}

void write_out() {
  int w_fd;
  w_fd = open("/mnt/centos/output.txt", O_CREAT | O_WRONLY, 0644);

  for (int i = 0; i < BUF_NUM; i++) {
    unsigned long long write_bytes = 0;
    while (write_bytes != output_valid_bytes[i]) {
      write_bytes += write(w_fd, output_bufs[i] + write_bytes,
                           output_valid_bytes[i] - write_bytes);
    }
  }

  close(w_fd);
}

void load_input_data(void) {
  int r_fd;
  r_fd = open("/mnt/centos/rle_data.txt", O_RDWR);
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
    ssize_t cur_read_bytes = read(r_fd, input_buf + input_valid_bytes,
                                  INPUT_BUF_SIZE - input_valid_bytes);
    if (!cur_read_bytes) {
      break;
    }
    input_valid_bytes += cur_read_bytes;
  }

  close(r_fd);
}

int main(int argc, char **argv) {
  Timer timer;
  load_input_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;
  timer.reset();
#pragma omp parallel num_threads(BUF_NUM)
  { decoder(); }
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;
  timer.reset();
  write_out();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
