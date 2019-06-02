#include "../inc/const.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

using namespace std;

char random_char() {
  int min_ascii = 65;
  int max_ascii = 90;
  return min_ascii + rand() % (max_ascii - min_ascii + 1);
}

int main() {
  ios::sync_with_stdio(false);

  freopen("data_integration.txt", "w", stdout);

  for (int i = 0; i < APP_RECORD_NUM; i++) {
    string row_data = "";
    for (int j = 0; j < APP_RECORD_LENGTH; j++) {
      row_data += random_char();
    }
    cout << row_data;
  }
}
