#ifndef STRUCTURE_H_
#define STRUCTURE_H_

struct SQL_Record {
  char round_name[12];
  char player_name[12];
  char score[2];
  char month[2];
  char day[2];
  char year[2];
  bool eop;
  bool valid;
} __attribute__((packed));

struct Filter_Params {
  unsigned char year_upper_thresh;
  unsigned char year_lower_thresh;
};
#endif
