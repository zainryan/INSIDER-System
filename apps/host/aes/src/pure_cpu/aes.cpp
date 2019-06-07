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
//#include <openssl/crypto.h>
#include <openssl/aes.h>

//#define NDEBUG
//#define VERIFY
#define FULL_UNROLL
using namespace std;

volatile unsigned char expected_result[READ_BUF_SIZE];

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



void read_encrypt(void) {
  int fd;
  fd = open("../../data_gen/aes_data.txt", O_RDWR);
  //fd = open("/mnt/aes_data.txt", O_RDWR);

  AES_KEY key_matrix;
  for (int i = 0; i < 4 * (AES_MAXNR + 1); i++) 
    key_matrix.rd_key[i] = 0;
  key_matrix.rounds = 10;

  unsigned char read_buf[READ_BUF_SIZE] __attribute__((aligned (16))); // for SSE
  int read_cnt = -1;
  long long total_read_bytes = 0;
  while (read_cnt != DATA_SIZE / READ_BUF_SIZE) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int curReadBytes = read(fd, read_buf, READ_BUF_SIZE - read_bytes);
      if (curReadBytes == 0)
        break;
      read_bytes += curReadBytes;
    }
    read_cnt++;
    total_read_bytes += read_bytes;
    std::cout << "total read bytes: " << total_read_bytes << std::endl;
#pragma omp parallel for num_threads(8) schedule(static)
    for (int i = 0; i < read_bytes; i += 16) {
      AES_encrypt(read_buf + i, (unsigned char *) expected_result + i, &key_matrix);
    }
    for (int i = 0; i < read_bytes; i++) {
        std::cout << std::hex << (int) expected_result[i] << " ";
    }
    std::cout << std::endl;
  }
  close(fd);
}

int main(int argc, char **argv) {
  puts("read_encrypt().....");
  Timer timer;
  read_encrypt();
  cout << "\nelapsed time = " << timer.elapsed() / 1000 << endl;
  // read_log();
  return 0;
}
