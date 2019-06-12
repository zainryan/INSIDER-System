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

#define NDEBUG

using namespace std;

inline bool verify(const char *q, const char *x) {
  int overlap = 0;
  for (int i = 0; i < APP_QUERY_LENGTH; i++) {
    for (int j = 0; j < APP_QUERY_LENGTH; j++) {
      if (q[i] == x[j]) {
        overlap++;
      }
    }
  }
#ifndef NDEBUG
  float similarity = (float)overlap / APP_QUERY_LENGTH;
  cout << "similarity: " << similarity << " = " << overlap << "/"
       << APP_QUERY_LENGTH << endl;
#endif
  if (overlap > APP_QUERY_LENGTH - APP_QUERY_THRES) {
    return true;
  }
  return false;
}

inline int prefix_filter(const char *query, const char *dataset,
                         const int &dataset_len) {
  bool match = false;
  bool similar;
  int match_pairs = 0;
  const char *dataset_ptr = dataset;
  for (int i = 0; i < dataset_len; i++) {
    match = false;
    for (int j = 0; j < APP_QUERY_THRES; j++) {
      for (int k = 0; k < APP_RECORD_THRES; k++) {
        if (query[j] == dataset_ptr[k]) {
          match = true;
        }
      }
    }
    if (match) {
      similar = verify(query, dataset_ptr);
      if (similar) {
        match_pairs++;
        for (int i = 0; i < APP_RECORD_LENGTH; i++) {
          cout << *(dataset_ptr + i);
        }
        cout << endl;
      }
    }
    dataset_ptr += APP_RECORD_LENGTH;
  }
  return match_pairs;
}

void read_data(const char *query) {
  int fp = open("/mnt/centos/data_integration.txt", O_RDONLY);
  if (fp < 0) {
    cout << "ERROR: cannot open file" << endl;
  }
  int readBytes = 0;
  char read_buf[READ_BUF_SIZE];
  int read_cnt = 0;
  int num_of_matches = 0;
  while (read_cnt != APP_RECORD_NUM * APP_RECORD_LENGTH / READ_BUF_SIZE + 1) {
    char *read_buf_ptr = read_buf;
    readBytes = 0;
    while (readBytes != READ_BUF_SIZE) {
      int actualBytes = read(fp, read_buf_ptr, READ_BUF_SIZE - readBytes);
      if (actualBytes == 0) {
        break;
      }
      readBytes += actualBytes;
      read_buf_ptr += actualBytes;
    }
    num_of_matches += prefix_filter(query, read_buf, readBytes);
    read_cnt++;
  }
  cout << "total number of matches is: " << num_of_matches << " ("
       << APP_RECORD_NUM << " in total)" << endl;
}
char random_char() {
  int min_ascii = 65;
  int max_ascii = 69;
  return min_ascii + rand() % (max_ascii - min_ascii + 1);
}

int main() {
  char query[APP_QUERY_LENGTH + 1] = "NWLRBBMQBHCDARZOWKKYHIDDQSCDXRJM";
  /*for (int i = 0; i < APP_QUERY_LENGTH; i++) {
      query[i] = random_char();
  }
  */
  read_data(query);
  return 0;
}
