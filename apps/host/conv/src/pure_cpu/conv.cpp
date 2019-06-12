#include "../../inc/const.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <queue>
#include <unistd.h>
#include <Eigen/Dense>

#define NDEBUG
//#define VERIFY

#define NUM_OF_ROWS (READ_BUF_SIZE / KERNEL_SIZE)
#define NUM_OF_COLS (KERNEL_SIZE)

using namespace std;

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

#define CONV_LOCAL_BATCH_SIZE (64L)
typedef Eigen::Matrix<char, CONV_LOCAL_BATCH_SIZE, KERNEL_SIZE, Eigen::RowMajor> dataMatrix;
typedef Eigen::Map<dataMatrix > mapDataMatrix;

typedef Eigen::Matrix<char, KERNEL_SIZE, NUM_OF_KERNELS> kernelMatrix;
typedef Eigen::Map<kernelMatrix > mapKernelMatrix;

typedef Eigen::Matrix<short, CONV_LOCAL_BATCH_SIZE, NUM_OF_KERNELS, Eigen::RowMajor> resMatrix;
typedef Eigen::Map<resMatrix > mapResMatrix;

char kernels[NUM_OF_KERNELS][KERNEL_SIZE];
mapKernelMatrix kernel_matrix = mapKernelMatrix(kernels[0]);    
char data_buf[NUM_OF_ROWS][NUM_OF_COLS];
volatile short expected_result[NUM_OF_ROWS][NUM_OF_KERNELS];

void read_kernels() {
  int fd;
#ifndef NDEBUG
  if ((fd = open("../../data_gen/conv_kernel.txt", O_RDWR)) < 0) {
#else
  if ((fd = open("/mnt/centos/conv_kernel.txt", O_RDWR)) < 0) {
#endif
    std::cout << "file open failed" << std::endl;
    exit(-1);
  };
  char read_buf[READ_BUF_SIZE];
  char *kernel_ptr = *kernels;
  int read_bytes = 0;
  while (read_bytes < KERNEL_SIZE * NUM_OF_KERNELS) {
    read_bytes += read(fd, read_buf, KERNEL_SIZE * NUM_OF_KERNELS - read_bytes);
  }
  for (int i = 0; i < KERNEL_SIZE * NUM_OF_KERNELS; i++) {
    *(kernel_ptr++) = read_buf[i];
  }
  close(fd);
}

inline void conv(char *data, short *result, int size) {
  long long iter_times = (size + (CONV_LOCAL_BATCH_SIZE * KERNEL_SIZE) - 1) 
                    / (CONV_LOCAL_BATCH_SIZE * KERNEL_SIZE);
  //for (long long i = 0; i * CONV_LOCAL_BATCH_SIZE * KERNEL_SIZE < size; i++) {
  #pragma omp parallel for
  for (long long i = 0; i < iter_times; i++) { 
    mapDataMatrix data_matrix = mapDataMatrix(data + i * CONV_LOCAL_BATCH_SIZE * KERNEL_SIZE);    
    mapResMatrix result_matrix = mapResMatrix(result + i * CONV_LOCAL_BATCH_SIZE * NUM_OF_KERNELS);    
    result_matrix.noalias() = data_matrix.template cast<short>() * kernel_matrix.template cast<short> ();
  }
}


void read_compute() {

  int fd;
#ifndef NDEBUG
  if ((fd = open("../../data_gen/conv_data.txt", O_RDWR)) < 0) {
#else
  if ((fd = open("/mnt/centos/conv_data.txt", O_RDWR)) < 0) {
#endif
    std::cout << "file open failed" << std::endl;
    exit(-1);
  };
  unsigned long long total_read_bytes = 0;
  while (total_read_bytes != (NUM_OF_TOTAL_ROWS * KERNEL_SIZE)) {
    unsigned long long read_bytes = 0;
    char *data_buf_ptr = *data_buf;
    while (read_bytes != READ_BUF_SIZE) {
      int tmp = read(fd, data_buf_ptr + read_bytes, READ_BUF_SIZE - read_bytes);
      if (!tmp) {
        std::cout << "no enough data to calc," 
          << "result may contain meaningless result" 
          << std::endl;
        break;
      }
      read_bytes += tmp;
    }
    total_read_bytes += read_bytes;
    std::cout << "total read bytes: " << total_read_bytes << std::endl;
    conv(data_buf[0], (short *) expected_result[0], read_bytes);
    
#ifdef VERIFY
    unsigned char *expected_ptr = (unsigned char *) expected_result[0];
    for (int i = 0; i < read_bytes; i++) 
      std::cout << std::hex << (int) *(expected_ptr++) << " ";
    std::cout << std::endl;
#endif

#ifndef NDEBUG
    for (int i = 0; i < read_bytes / sizeof(short); i++) 
      std::cout << (*expected_result)[i] << std::endl;
#endif
  }
  close(fd);
}


int main(int argc, char **argv) {
  omp_set_num_threads(1);
  Timer timer;
  puts("read_kernels().....");
  read_kernels();

  puts("read_compute().....");
  timer.reset();
  read_compute();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
