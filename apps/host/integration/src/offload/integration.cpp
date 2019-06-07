#include "../../inc/const.h"
#include <insider_runtime.hpp>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

//#define NDEBUG

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
char query[33] = "NWLRBBMQBHCDARZOWKKYHIDDQSCDXRJM";
char random_char() {
  int min_ascii = 65;
  int max_ascii = 90;
  return min_ascii + rand() % (max_ascii - min_ascii + 1);
}
void init() {
  for (int i = 0; i < 32; i++) {
    query[i] = random_char();
  }
}

inline void disp_result(int readBytes) {
  char *buffer_ptr = buf;
  for (int i = 0; i < readBytes / APP_RECORD_LENGTH; i++) {
    string matched_record = "";
    for (int j = 0; j < APP_RECORD_LENGTH; j++) {
      matched_record += *buffer_ptr++;
    }
    cout << "matched record is: " << matched_record << endl;
  }
}

void work(void) {
  //    init();
  const char *virt_path = reg_virt_file("/mnt/data_integration.txt");
  int fd = vopen(virt_path, O_RDONLY);
  send_input_params(((APP_RECORD_THRES << 16) | (APP_QUERY_THRES)));
  send_input_params_array((unsigned int *)query,
                          sizeof(query) / sizeof(unsigned int));
  long long totalBytes = 0;
  int readBytes = 0;
  bool fin = false;
  while (!fin) {
    readBytes = 0;
    while (readBytes != READ_BUF_SIZE) {
      int tmp = vread(fd, &buf, READ_BUF_SIZE - readBytes);
      if (tmp) {
        readBytes += tmp;
      } else {
        fin = true;
        break;
      }
    }
    totalBytes += readBytes;
    // disp_result(readBytes);
  }

  cout << "total bytes is: " << totalBytes / 1024.0 / 1024.0 / 1024.0 << " GB"
       << endl;
  vclose(fd);
}

int main(int argc, char **argv) {
  puts("work().....");
  struct timespec start = timer_start();
  work();
  unsigned long finish = timer_end(start);
  printf("%lf\n", finish / 1000.0 / 1000.0 / 1000.0);
  return 0;
}
