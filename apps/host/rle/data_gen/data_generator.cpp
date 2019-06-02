#include <fcntl.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "../inc/const.h"

using namespace std;

int main() {
  long long inflat_data_size = 0;
  long long output_data_size = 0;  // count in Byte

  unsigned char *compressed = (unsigned char *)malloc(INFLAT_DATA_SIZE);
  unsigned char *data_ptr = compressed + sizeof(bitmap_header);

  while (inflat_data_size < INFLAT_DATA_SIZE) {
    unsigned char number = ((unsigned char)rand() % 28) + 1;
    unsigned char alpha = (unsigned char)(rand() % 26 + 'a');

    if (inflat_data_size + number > INFLAT_DATA_SIZE) {
      number = (INFLAT_DATA_SIZE - inflat_data_size);
    }
    *data_ptr = number;
    data_ptr++;
    *data_ptr = alpha;
    data_ptr++;
    inflat_data_size += number;
    output_data_size += 2;
  }
  cout << "compressed ratio: " << float(output_data_size) / inflat_data_size
       << " (" << output_data_size << " / " << inflat_data_size << ")"
       << std::endl;

  int fobj = open("/mnt/centos/rle_data.txt", O_CREAT | O_WRONLY, 0644);

  bitmap_header header;
  header.size = output_data_size +
                sizeof(bitmap_header);  // assume data field follows the header
  header.imageSize = output_data_size;

  long long total_write_bytes = 0;
  memcpy(compressed, &header, sizeof(bitmap_header));

  do {
    long long write_bytes =
        write(fobj, compressed + total_write_bytes,
              header.size - total_write_bytes);
    total_write_bytes += write_bytes;
  } while (total_write_bytes < header.size);
  return 0;
}
