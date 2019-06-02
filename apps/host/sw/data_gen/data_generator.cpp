#include "../inc/genseq.h"
#include "../inc/sw.h"

#include <fcntl.h>
#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

char *generate_input() {
  unsigned int *dummy;
  unique_ptr<unsigned int> dummy_gc = unique_ptr<unsigned int>(dummy);
  return (char *)generatePackedNReadRefPair(ITER_CNT * NUMPACKED, MAXROW,
                                            MAXCOL, &dummy, 0);
}

int main() {
  int fd = open("/data_new_apps/sw_input.txt", O_CREAT | O_WRONLY);
  std::cout << "Start to generate data in memory..." << std::endl;
  char *input_ptr = generate_input();
  unique_ptr<char> input_ptr_gc = unique_ptr<char>(input_ptr);

  std::cout << "Start to persist the generated data to drive..." << std::endl;
  ssize_t total_write_bytes = DATA_SIZE;
  ssize_t written_bytes = 0;
  while (total_write_bytes != written_bytes) {
    ssize_t delta_written_bytes = write(fd, (char *)input_ptr + written_bytes,
                                        total_write_bytes - written_bytes);
    written_bytes += delta_written_bytes;
  }

  return 0;
}
