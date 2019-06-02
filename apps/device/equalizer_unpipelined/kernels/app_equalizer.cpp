#ifndef APP_EQUALIZER_CPP_
#define APP_EQUALIZER_CPP_

#include <insider_kernel.h>

#include "equalizer.h"

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

void equalizer(unsigned int image_height, ST_Queue<APP_Data> &app_input_data,
               ST_Queue<APP_Data> &app_output_data) {
  for (uint n = 0; n < image_height / 32; n++) {
    ushort line[32][IMAGE_WIDTH_PIXELS];
#pragma HLS array_partition variable = line complete dim = 1
    ushort hist[32][HISTOGRAM_DEPTH];
#pragma HLS array_partition variable = hist complete dim = 1

    for (uint i = 0; i < HISTOGRAM_DEPTH; i++) {
#pragma HLS pipeline
      for (uint k = 0; k < 32; k++) {
        hist[k][i] = 0;
      }
    }

    ushort old[32];
#pragma HLS array_partition variable = old complete dim = 1
    ushort acc[32];
#pragma HLS array_partition variable = acc complete dim = 1

    for (uint i = 0; i < IMAGE_WIDTH_PIXELS; i++) {
#pragma HLS dependence variable = hist intra RAW false
#pragma HLS dependence variable = line intra RAW false
#pragma HLS dependence variable = old intra RAW false
#pragma HLS dependence variable = acc intra RAW false
#pragma HLS pipeline
      ushort pline[32];
#pragma HLS array_partition variable = pline complete dim = 1
      APP_Data input;
      app_input_data.read(input);
      for (int k = 0; k < 32; k++) {
        pline[k] = input.data(k * 16 + 15, k * 16);
      }

      if (i == 0) {
        for (uint k = 0; k < 32; k++) {
          old[k] = 0;
          acc[k] = 0;
        }
      }

      for (uint k = 0; k < 32; k++) {
        ushort val = (pline[k] & (((1 << HISTOGRAM_BITS) - 1) << 4)) >> 4;
        line[k][i] = val;

        if (old[k] == val) {
          acc[k]++;
        } else {
          hist[k][old[k]] = acc[k];
          acc[k] = hist[k][val] + 1;
        }

        old[k] = val;
      }
    }

    /* CDF */
    ushort max[32];
#pragma HLS array_partition variable = max complete dim = 1
    ushort min[32];
#pragma HLS array_partition variable = min complete dim = 1
    ushort cdf[32][HISTOGRAM_DEPTH];
#pragma HLS array_partition variable = cdf complete dim = 1

    for (uint i = 0; i < HISTOGRAM_DEPTH; i++) {
#pragma HLS pipeline
      for (int j = 0; j < 32; j++) {
        if (i == 0) {
          max[j] = 0;
          min[j] = IMAGE_WIDTH_PIXELS;
          hist[j][old[j]] = acc[j];
        }
        ushort val = hist[j][i];

        if (val != 0 && val < min[j]) {
          min[j] = val;
        }

        max[j] += val;

        cdf[j][i] = max[j];
      }
    }

    for (uint i = 0; i < IMAGE_WIDTH_PIXELS; i++) {
#pragma HLS pipeline
      ushort bucket_out[32];
#pragma HLS array_partition variable = bucket_out complete dim = 1

      for (uint k = 0; k < 32; k++) {
        ushort val = line[k][i];
        bucket_out[k] = eq_func(cdf[k][val], min[k]) << 4;
      }

      APP_Data output;
      for (int k = 0; k < 32; k++) {
        output.data(k * 16 + 15, k * 16) = bucket_out[k];
      }

      output.len = 64;
      output.eop = ((i == (IMAGE_WIDTH_PIXELS - 1)) &&
                    (n == image_height / 32 - 1));
      app_output_data.write(output);
    }
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
