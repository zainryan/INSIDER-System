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
#include <unordered_map>

#include <insider_runtime.hpp>
#include <stdio.h>
#include <time.h>

#define c2i(c) (c - '0')
#define max(a, b) ((a) > (b) ? (a) : (b))
#define BUF_LEN (2 * 1024 * 1024)
//#define NDEBUG

using namespace std;

unordered_map<string, int> mappings_str_int;
string mappings_int_str[ROUND_NUM + 1];

struct SumNumPair {
  double sum;
  int num;
} calc_scores[ROUND_NUM + 1];

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

struct Record {
  char round_name[ROUND_NAME_LEN];
  char player_name[PLAYER_NAME_LEN];
  unsigned char score;
  unsigned char month;
  unsigned char day;
  unsigned char year;
  unsigned char dummy[4];
};

Record record_arr[ROW_NUM] __attribute__((aligned(32)));

int get_round_name_mapping_id(string str) {
  static int id = 0;
  int ret = mappings_str_int[str];
  if (!ret) {
    ret = mappings_str_int[str] = ++id;
    mappings_int_str[ret] = str;
  }
  return ret;
}

long long totalReadBytes = 0;

void read_data() {
  const char *virt_path = reg_virt_file("/mnt/centos/sql_data.txt");
  int fd = vopen(virt_path, O_RDONLY);
  int year_upper_thresh = 30;
  int year_lower_thresh = 20;
  send_input_params((YEAR_UPPER_THRESH << 8) | YEAR_LOWER_THRESH);

  unsigned char *read_buf = (unsigned char *)malloc(READ_BUF_SIZE);
  bool fin_file = false;
  int offset = 0;

  while (!fin_file) {
    int read_bytes = 0;
    int write_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int tmp = vread(fd, read_buf, READ_BUF_SIZE - read_bytes);
      if (!tmp) {
        fin_file = true;
        break;
      } else {
        read_bytes += tmp;
      }
    }
    totalReadBytes += read_bytes;
#pragma omp parallel for schedule(static, 2) num_threads(8)
    for (int i = 0; i < read_bytes / ROW_LEN; i++) {
      int idx = i * ROW_LEN;
      record_arr[i + offset].year =
          c2i(read_buf[idx + 30]) * 10 + c2i(read_buf[idx + 31]) * 1;

      for (int j = 0; j < ROUND_NAME_LEN; j++) {
        record_arr[i + offset].round_name[j] = read_buf[idx + j];
      }
      idx += ROUND_NAME_LEN;

      for (int j = 0; j < PLAYER_NAME_LEN; j++) {
        record_arr[i + offset].player_name[j] = read_buf[idx + j];
      }
      idx += PLAYER_NAME_LEN;

      record_arr[i + offset].score =
          c2i(read_buf[idx + 0]) * 10 + c2i(read_buf[idx + 1]) * 1;
      idx += 2;

      record_arr[i + offset].month =
          c2i(read_buf[idx + 0]) * 10 + c2i(read_buf[idx + 1]) * 1;
      idx += 2;

      record_arr[i + offset].day =
          c2i(read_buf[idx + 0]) * 10 + c2i(read_buf[idx + 1]) * 1;
    }
    offset += read_bytes / ROW_LEN;
  }
  vclose(fd);
  free(read_buf);
}

int main(int argc, char **argv) {
  puts("read_data().....");
  Timer timer;
  read_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;
  cout << "total read bytes is " << totalReadBytes << endl;

  return 0;
}
