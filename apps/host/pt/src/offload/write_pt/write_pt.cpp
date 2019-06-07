#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <insider_runtime.hpp>
#include <fcntl.h>
#include <unistd.h>

const int BUF_SIZE = 2 * 1024 * 1024;
unsigned char buf[BUF_SIZE];

int main() {
  const char *input_path = "/mnt/centos/input.txt";
  const char *real_path = "/mnt/centos/output.txt";
  const char *virt_path = reg_virt_file(real_path);
  int v_fd = vopen(virt_path, O_WRONLY);
  int i_fd = open(input_path, O_RDONLY);

  if (v_fd && i_fd) {
    while (1) {
      size_t input_read_bytes = read(i_fd, buf, BUF_SIZE);
      if (input_read_bytes == 0) {
        break;
      } else if (input_read_bytes < 0) {
        perror("Read failed");
        exit(-1);
      } else {
        size_t virt_write_bytes = 0;
        while (virt_write_bytes != input_read_bytes) {
          size_t delta_bytes = vwrite(v_fd, buf + virt_write_bytes,
                                      input_read_bytes - virt_write_bytes);
          if (delta_bytes <= 0) {
            std::cout << "Error: delta_bytes <= 0" << std::endl;
            goto finish;
          }
          virt_write_bytes += delta_bytes;
        }
      }
    }
  }
finish:
  if (i_fd) {
    close(i_fd);
  }
  if (v_fd) {
    vclose(v_fd);
  }

  return 0;
}
