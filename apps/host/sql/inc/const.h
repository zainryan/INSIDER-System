#ifndef CONST_H
#define CONST_H

#define ROW_NUM (2013265920)
#define ROUND_NUM (1024)
#define ROUND_NAME_LEN (12)
#define PLAYER_NAME_LEN (12)
#define SCORE_LEN (2)
#define MONTH_LEN (2)
#define DAY_LEN (2)
#define YEAR_LEN (2)
#define ROW_LEN                                                                \
  (ROUND_NAME_LEN + PLAYER_NAME_LEN + SCORE_LEN + MONTH_LEN + DAY_LEN +        \
   YEAR_LEN)

#define READ_BUF_SIZE (1024 * 1024 * 2)
#define YEAR_LOWER_THRESH (50)
#define YEAR_UPPER_THRESH (60)

#endif
