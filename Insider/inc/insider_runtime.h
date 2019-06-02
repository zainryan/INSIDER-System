#ifndef INSIDER_RUNTIME_H
#define INSIDER_RUNTIME_H

#include <stddef.h>
#include <sys/types.h>

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

#endif
