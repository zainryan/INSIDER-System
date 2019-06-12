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

#define c2i(c) ((c) - '0')
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
};

Record record_arr[ROW_NUM];
Record *record_arr_ptr = record_arr;

int get_round_name_mapping_id(string str) {
  static int id = 0;
  int ret = mappings_str_int[str];
  if (!ret) {
    ret = mappings_str_int[str] = ++id;
    mappings_int_str[ret] = str;
  }
  return ret;
}

void read_data(void) {
  int fd = open("/mnt/centos/sql_data.txt", O_RDWR);
  char read_buf[READ_BUF_SIZE];
  int read_cnt = 0;

  while (read_cnt != (unsigned long long)ROW_NUM * ROW_LEN / READ_BUF_SIZE) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      read_bytes += read(fd, read_buf, READ_BUF_SIZE - read_bytes);
    }
    read_cnt++;
    for (int i = 0; i < READ_BUF_SIZE / ROW_LEN; i++) {
      int idx = i * ROW_LEN;

      record_arr_ptr->year =
          c2i(read_buf[idx + 30]) * 10 + c2i(read_buf[idx + 31]) * 1;

      if (record_arr_ptr->year <= YEAR_UPPER_THRESH &&
          record_arr_ptr->year >= YEAR_LOWER_THRESH) {
        for (int j = 0; j < ROUND_NAME_LEN; j++) {
          record_arr_ptr->round_name[j] = read_buf[idx + j];
        }
        idx += ROUND_NAME_LEN;

        for (int j = 0; j < PLAYER_NAME_LEN; j++) {
          record_arr_ptr->player_name[j] = read_buf[idx + j];
        }
        idx += PLAYER_NAME_LEN;

        record_arr_ptr->score =
            c2i(read_buf[idx + 0]) * 10 + c2i(read_buf[idx + 1]) * 1;
        idx += 2;

        record_arr_ptr->month =
            c2i(read_buf[idx + 0]) * 10 + c2i(read_buf[idx + 1]) * 1;
        idx += 2;

        record_arr_ptr->day =
            c2i(read_buf[idx + 0]) * 10 + c2i(read_buf[idx + 1]) * 1;

        record_arr_ptr++;
      }
    }
  }
  cout << (record_arr_ptr - record_arr) / (double)ROW_NUM << endl;

  close(fd);
}

void execute() {
  for (Record *ptr = record_arr; ptr != record_arr_ptr; ptr++) {
    int id = get_round_name_mapping_id(string(ptr->round_name, ROUND_NAME_LEN));
    calc_scores[id].sum += ptr->score;
    calc_scores[id].num++;
  }
  for (int i = 1; i < ROUND_NUM; i++) {
    string str = mappings_int_str[i];
    if (str != "") {
      int id = get_round_name_mapping_id(str);
      cout << str << " " << calc_scores[id].sum / calc_scores[id].num << endl;
    }
  }
}

int main(int argc, char **argv) {
  puts("read_data_filter().....");
  Timer timer;
  read_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("execute().....");
  timer.reset();
  execute();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
