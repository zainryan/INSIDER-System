#ifndef INSIDER_RUNTIME_HPP
#define INSIDER_RUNTIME_HPP

#include <cstddef>
#include <sys/types.h>

extern "C" {
void send_input_params(unsigned int data);
void send_input_params_array(unsigned int *data_arr, size_t arr_len);
int vopen(const char *pathname, int flags);
ssize_t vread(int fd, void *buf, size_t count);
int vwrite(int fd, void *buf, size_t count);
int vsync(int fd);
int vclose(int fd);
int vclose_with_rsize(int fd, size_t *rfile_written_bytes);
const char *reg_virt_file(const char *real_path);
const char *reg_virt_file_sg(size_t sg_list_len, const char **file_names,
                             size_t *offs, size_t *lens);
}

const char *reg_virt_file(size_t sg_list_len, const char **file_names,
                          size_t *offs, size_t *lens) {
  return reg_virt_file_sg(sg_list_len, file_names, offs, lens);
}

int vclose(int fd, size_t *rfile_written_bytes) {
  return vclose_with_rsize(fd, rfile_written_bytes);
}

#endif
