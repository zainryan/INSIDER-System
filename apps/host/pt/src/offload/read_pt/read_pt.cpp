#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <insider_runtime.hpp>
#include <iostream>
#include <unistd.h>

const int BUF_SIZE = 2 * 1024 * 1024;
unsigned char buf_0[BUF_SIZE];
unsigned char buf_1[BUF_SIZE];

int main() {
  bool error = 0;
  const char *real_path = "/mnt/centos/input.txt";
  const char *virt_path = reg_virt_file(real_path);
  int v_fd = vopen(virt_path, O_RDONLY);
  int r_fd = open(real_path, O_RDONLY);
  if (v_fd && r_fd) {
    while (1) {
      size_t real_read_bytes = read(r_fd, buf_0, BUF_SIZE);
      if (real_read_bytes == 0) {
        break;
      } else if (real_read_bytes < 0) {
        perror("Read failed");
        exit(-1);
      } else {
        size_t virt_read_bytes = 0;
        while (virt_read_bytes != real_read_bytes) {
          size_t delta_bytes = vread(v_fd, buf_1 + virt_read_bytes,
                                     real_read_bytes - virt_read_bytes);
          if (delta_bytes <= 0) {
            std::cout << "Error: delta_bytes <= 0" << std::endl;
            error = true;
            goto finish;
          }
          virt_read_bytes += delta_bytes;
        }
        for (int i = 0; i < real_read_bytes; i++) {
          if (buf_0[i] != buf_1[i]) {
            std::cout << "Error: buf_0[i] != buf_1[i]" << std::endl;
            error = true;
            goto finish;
          }
        }
      }
    }
  }
finish:
  if (r_fd) {
    close(r_fd);
  }
  if (v_fd) {
    vclose(v_fd);
  }
  if (!error) {
    std::cout << "PASSED!" << std::endl;
  }

  return 0;
}
