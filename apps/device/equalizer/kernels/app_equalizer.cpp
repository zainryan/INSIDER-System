#ifndef APP_EQUALIZER_CPP_
#define APP_EQUALIZER_CPP_

#include <insider_kernel.h>

#include "equalizer.h"

#ifndef CSIM
#include <hls/utils/x_hls_utils.h>
#else
#define reg(x) (x)
#endif

ushort eq_func(ushort val, ushort min) {
#pragma HLS INLINE
  uint eq_val;
  if (val == 0) {
    eq_val = 0;
  } else {
    eq_val = val - min;
    eq_val *= (IMAGE_WIDTH_PIXELS - 1);
    eq_val /= (1 + IMAGE_WIDTH_PIXELS - min);
  }
  return eq_val;
}

void step_0(ST_Queue<APP_Data> &app_input_data,
            ushort pline[][IMAGE_WIDTH_PIXELS]) {
  for (uint i = 0; i < IMAGE_WIDTH_PIXELS; i++) {
#pragma HLS pipeline
    APP_Data input;
    app_input_data.read(input);
    for (int k = 0; k < 32; k++) {
#pragma HLS unroll
      ushort tmp = input.data(k * 16 + 15, k * 16);
      pline[k][i] = reg(tmp);
    }
  }
}

void step_1(ushort pline[][IMAGE_WIDTH_PIXELS],
            ushort line[][IMAGE_WIDTH_PIXELS],
            ushort pre_hist[][HISTOGRAM_DEPTH], ushort old[], ushort acc[]) {
  for (uint i = 0; i < HISTOGRAM_DEPTH; i++) {
#pragma HLS pipeline
    for (uint k = 0; k < 32; k++) {
#pragma HLS unroll
      pre_hist[k][i] = 0;
    }
  }

  for (uint i = 0; i < IMAGE_WIDTH_PIXELS; i++) {
#pragma HLS pipeline

#pragma HLS dependence variable = pre_hist intra RAW false
#pragma HLS dependence variable = line intra RAW false
#pragma HLS dependence variable = old intra RAW false
#pragma HLS dependence variable = acc intra RAW false
    APP_Data input;

    if (i == 0) {
      for (uint k = 0; k < 32; k++) {
#pragma HLS unroll
        old[k] = 0;
        acc[k] = 0;
      }
    }

    for (uint k = 0; k < 32; k++) {
#pragma HLS unroll
      ushort val = reg((pline[k][i] & (((1 << HISTOGRAM_BITS) - 1) << 4)) >> 4);
      line[k][i] = val;

      if (reg(old[k]) == val) {
        acc[k]++;
      } else {
        pre_hist[k][reg(old[k])] = acc[k];
        acc[k] = pre_hist[k][val] + 1;
      }

      old[k] = val;
    }
  }
}

void step_2(ushort pre_hist[][HISTOGRAM_DEPTH], ushort hist[][HISTOGRAM_DEPTH],
            ushort old[], ushort acc[], ushort max[], ushort min[],
            ushort cdf[][HISTOGRAM_DEPTH]) {
  for (uint i = 0; i < HISTOGRAM_DEPTH; i++) {
#pragma HLS pipeline
    for (int j = 0; j < 32; j++) {
#pragma HLS unroll
      hist[j][i] = reg(pre_hist[j][i]);
    }
  }

  for (uint i = 0; i < HISTOGRAM_DEPTH; i++) {
#pragma HLS pipeline
    for (int j = 0; j < 32; j++) {
#pragma HLS unroll
      if (i == 0) {
        max[j] = 0;
        min[j] = IMAGE_WIDTH_PIXELS;
        hist[j][reg(old[j])] = reg(acc[j]);
      }
      ushort val = reg(hist[j][i]);

      if (val != 0 && val < reg(min[j])) {
        min[j] = reg(val);
      }

      max[j] = reg(reg(max[j]) + val);

      cdf[j][i] = reg(max[j]);
    }
  }
}

void step_3(uint n, unsigned int image_height,
            ushort line[][IMAGE_WIDTH_PIXELS], ushort min[],
            ushort cdf[][HISTOGRAM_DEPTH],
            ushort bucket_out[][IMAGE_WIDTH_PIXELS]) {
  for (uint i = 0; i < IMAGE_WIDTH_PIXELS; i++) {
#pragma HLS pipeline

    for (uint k = 0; k < 32; k++) {
#pragma HLS unroll
      ushort val = reg(line[k][i]);
      bucket_out[k][i] = reg(eq_func(reg(cdf[k][val]), reg(min[k])) << 4);
    }
  }
}

void step_4(uint n, unsigned int image_height,
            ST_Queue<APP_Data> &app_output_data,
            ushort bucket_out[][IMAGE_WIDTH_PIXELS]) {
  for (uint i = 0; i < IMAGE_WIDTH_PIXELS; i++) {
#pragma HLS pipeline
    APP_Data output;
    for (int k = 0; k < 32; k++) {
#pragma HLS unroll
      output.data(k * 16 + 15, k * 16) = reg(bucket_out[k][i]);
    }

    output.len = 64;
    output.eop =
        ((i == (IMAGE_WIDTH_PIXELS - 1)) && (n == image_height / 32 - 1));
    app_output_data.write(reg(output));
  }
}

void equalizer(unsigned int image_height, ST_Queue<APP_Data> &app_input_data,
               ST_Queue<APP_Data> &app_output_data) {
  for (uint n = 0; n < image_height / 32; n++) {
#pragma HLS dataflow

    ushort line[32][IMAGE_WIDTH_PIXELS];
#pragma HLS array_partition variable = line complete dim = 1
    ushort pline[32][IMAGE_WIDTH_PIXELS];
#pragma HLS array_partition variable = pline complete dim = 1
    ushort bucket_out[32][IMAGE_WIDTH_PIXELS];
#pragma HLS array_partition variable = bucket_out complete dim = 1
    ushort pre_hist[32][HISTOGRAM_DEPTH];
#pragma HLS array_partition variable = pre_hist complete dim = 1
    ushort hist[32][HISTOGRAM_DEPTH];
#pragma HLS array_partition variable = hist complete dim = 1
    ushort old[32];
#pragma HLS array_partition variable = old complete dim = 1
    ushort acc[32];
#pragma HLS array_partition variable = acc complete dim = 1
    ushort max[32];
#pragma HLS array_partition variable = max complete dim = 1
    ushort min[32];
#pragma HLS array_partition variable = min complete dim = 1
    ushort cdf[32][HISTOGRAM_DEPTH];
#pragma HLS array_partition variable = cdf complete dim = 1

    step_0(app_input_data, pline);
    step_1(pline, line, pre_hist, old, acc);
    step_2(pre_hist, hist, old, acc, max, min, cdf);
    step_3(n, image_height, line, min, cdf, bucket_out);
    step_4(n, image_height, app_output_data, bucket_out);
  }
}

void app_equalizer(ST_Queue<unsigned int> &app_input_params,
                   ST_Queue<APP_Data> &app_input_data,
                   ST_Queue<APP_Data> &app_output_data) {
  while (1) {
    unsigned int image_height;
    if (app_input_params.read_nb(image_height)) {
      equalizer(image_height, app_input_data, app_output_data);
    }
  }
}

#endif
