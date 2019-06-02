#ifndef APP_STENCIL_CPP_
#define APP_STENCIL_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

#ifndef CSIM
#include <hls/utils/x_hls_utils.h>
#else
#define reg(x) (x)
#endif

inline Point uint_to_point(unsigned int uint) {
#pragma HLS INLINE
  Point point;
  point.red = (uint >> 24) & 0xFFFFFFFF;
  point.green = (uint >> 16) & 0xFFFFFFFF;
  point.blue = (uint >> 8) & 0xFFFFFFFF;
  point.alpha = (uint >> 0) & 0xFFFFFFFF;
  return point;
}

inline unsigned int point_to_uint(const Point &point) {
#pragma HLS INLINE
  unsigned int uint = 0;
  uint |= ((unsigned int)point.red) << 24;
  uint |= ((unsigned int)point.green) << 16;
  uint |= ((unsigned int)point.blue) << 8;
  uint |= ((unsigned int)point.alpha) << 0;
  return uint;
}

inline Point operator*(const Point &pa, const Point &pb) {
#pragma HLS INLINE
  Point pc;
  pc.red = pa.red * pb.red;
  pc.green = pa.green * pb.green;
  pc.blue = pa.blue * pb.blue;
  pc.alpha = pa.alpha * pb.alpha;
  return pc;
}

inline Point &Point::operator+=(const Point &p) {
#pragma HLS INLINE
  this->red += p.red;
  this->green += p.green;
  this->blue += p.blue;
  this->alpha += p.alpha;
  return *this;
}

void app_stencil(ST_Queue<unsigned int> &app_input_params,
                 ST_Queue<APP_Data> &app_input_data,
                 ST_Queue<APP_Data> &app_output_data) {
  Point stencil[STENCIL_HEIGHT][STENCIL_WIDTH];
#pragma HLS array_partition variable = stencil complete dim = 0
  Point rows_buffer[STENCIL_HEIGHT][NUM_POINT_PER_INPUT]
                   [(PADDING_WIDTH + PICTURE_WIDTH) / NUM_POINT_PER_INPUT]
      __attribute__((annotate("no_reset")));
#pragma HLS array_partition variable = rows_buffer complete dim = 1
#pragma HLS array_partition variable = rows_buffer complete dim = 2
  Point cols_buffer[STENCIL_HEIGHT][STENCIL_WIDTH - 1]
                   [(PADDING_WIDTH + PICTURE_WIDTH) / NUM_POINT_PER_INPUT]
      __attribute__((annotate("no_reset")));
#pragma HLS array_partition variable = cols_buffer complete dim = 1
#pragma HLS array_partition variable = cols_buffer complete dim = 2
  bool finish_reading_stencil_points = false;
  unsigned char app_input_read_times = 0;
  unsigned long long num_line = 0;
  Point prev_points_input_data[NUM_POINT_PER_INPUT];
#pragma HLS array_partition variable = prev_points_input_data complete dim = 0
  unsigned char stencil_height_idx = 0;
  unsigned char stencil_width_idx = 0;

  while (1) {
#pragma HLS pipeline
    if (!finish_reading_stencil_points) {
      unsigned int input_param;
      if (app_input_params.read_nb(input_param)) {
        stencil[stencil_height_idx][stencil_width_idx] = uint_to_point(input_param);
        stencil_width_idx++;
        if (stencil_width_idx == STENCIL_WIDTH) {
          stencil_width_idx = 0;
          stencil_height_idx++;
          if (stencil_height_idx == STENCIL_HEIGHT) {
            finish_reading_stencil_points = true;
          }
        }
      }
    } else {
      APP_Data input_data;
      APP_Data output_data;
      if (app_input_data.read_nb(input_data)) {
        Point points_input_data[NUM_POINT_PER_INPUT];
#pragma HLS array_partition variable = points_input_data complete dim = 0
        for (int i = 0; i < NUM_POINT_PER_INPUT; i++) {
          unsigned int uint = input_data.data(i * 32 + 31, i * 32);
          points_input_data[i] = uint_to_point(uint);
        }
        Point picture_chunk[STENCIL_HEIGHT][NUM_POINT_PER_INPUT];
#pragma HLS array_partition variable = picture_chunk complete dim = 0
        for (int i = 0; i < STENCIL_HEIGHT; i++) {
#pragma HLS dependence variable = rows_buffer inter RAW false
          for (int j = 0; j < NUM_POINT_PER_INPUT; j++) {
            unsigned char index = i + num_line;
            if (index >= STENCIL_HEIGHT) {
              index -= STENCIL_HEIGHT;
            }
            if (i != STENCIL_HEIGHT - 1) {
              picture_chunk[i][j] = rows_buffer[index][j][app_input_read_times];
            } else {
              rows_buffer[index][j][app_input_read_times] =
                  points_input_data[j];
              picture_chunk[i][j] = points_input_data[j];
            }
          }
        }
        Point old_cols_buffer[STENCIL_HEIGHT][STENCIL_WIDTH - 1];
#pragma HLS array_partition variable = old_cols_buffer complete dim = 0
        for (int j = 0; j < STENCIL_HEIGHT; j++) {
#pragma HLS dependence variable = cols_buffer inter RAW false
          for (int k = 0; k < STENCIL_WIDTH - 1; k++) {
            unsigned char index = j + num_line;
            if (index >= STENCIL_HEIGHT) {
              index -= STENCIL_HEIGHT;
            }
            if (j == STENCIL_HEIGHT - 1) {
              cols_buffer[index][k][app_input_read_times] =
                  prev_points_input_data[NUM_POINT_PER_INPUT -
                                             (STENCIL_WIDTH - 1 - k)];
              old_cols_buffer[j][k] =
                  prev_points_input_data[NUM_POINT_PER_INPUT -
                                             (STENCIL_WIDTH - 1 - k)];
            } else {
              old_cols_buffer[j][k] =
                  cols_buffer[index][k][app_input_read_times];
            }
          }
        }
        Point points_output_data[NUM_POINT_PER_INPUT];
#pragma HLS array_partition variable = points_output_data complete dim = 0
        for (int i = 0; i < NUM_POINT_PER_INPUT; i++) {
          Point cur_point;
          cur_point.red = cur_point.green = cur_point.blue = cur_point.alpha =
              0;
          for (int j = 0; j < STENCIL_HEIGHT; j++) {
            for (int k = 0; k < STENCIL_WIDTH; k++) {
              if (i - (STENCIL_WIDTH - 1 - k) >= 0) {
                cur_point +=
                    reg(reg(stencil[j][k]) *
                        reg(picture_chunk[j][i - (STENCIL_WIDTH - 1 - k)]));
              } else {
                cur_point +=
                    reg(reg(stencil[j][k]) * reg(old_cols_buffer[j][k + i]));
              }
            }
          }
          points_output_data[i] = cur_point;
        }
        for (int i = 0; i < NUM_POINT_PER_INPUT; i++) {
          prev_points_input_data[i] = points_input_data[i];
        }
        for (int i = 0; i < NUM_POINT_PER_INPUT; i++) {
          output_data.data(i * 32 + 31, i * 32) =
              point_to_uint(points_output_data[i]);
        }
        app_input_read_times++;
        if (app_input_read_times ==
            (PADDING_WIDTH + PICTURE_WIDTH) / NUM_POINT_PER_INPUT) {
          app_input_read_times = 0;
          num_line++;
          if (num_line == STENCIL_HEIGHT) {
            num_line = 0;
          }
        }
        output_data.eop = input_data.eop;
        output_data.len = input_data.len;
        app_output_data.write(output_data);
      }
    }
  }
}
#endif
