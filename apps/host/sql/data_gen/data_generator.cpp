#include "../inc/const.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

using namespace std;

vector<string> round_name_vec;

char random_char() {
  int min_ascii = 65;
  int max_ascii = 68;
  return min_ascii + rand() % (max_ascii - min_ascii + 1);
}

string random_str(int len) {
  string s = "";
  for (int i = 0; i < len; i++) {
    s += random_char();
  }
  return s;
}

int main() {
  freopen("sql_data.txt", "w", stdout);
  ios::sync_with_stdio(false);

  for (int i = 0; i < ROUND_NUM; i++) {
    round_name_vec.push_back(random_str(ROUND_NAME_LEN));
  }

  for (int i = 0; i < ROW_NUM; i++) {
    string round_name = round_name_vec[rand() % round_name_vec.size()];

    string player_name = random_str(PLAYER_NAME_LEN);

    string score = to_string(rand() % 100);
    while (score.size() < SCORE_LEN) {
      score = "0" + score;
    }

    string month = to_string(rand() % 12 + 1);
    while (month.size() < MONTH_LEN) {
      month = "0" + month;
    }

    string day = to_string(rand() % 30 + 1);
    while (day.size() < DAY_LEN) {
      day = "0" + day;
    }

    string year = to_string(rand() % 100);
    while (year.size() < YEAR_LEN) {
      year = "0" + year;
    }

    string record_str = round_name + player_name + score + month + day + year;
    if (record_str.size() != 32) {
      cout << endl;
    }
    assert(record_str.size() == 32);

    cout << record_str;
  }

  return 0;
}
