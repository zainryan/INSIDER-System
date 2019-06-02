#define _GNU_SOURCE
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <insider_macros.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>

#define MAX_CMD_LEN (4096)
#define MAX_PATH_LEN (4096)
#define MAX_CMD_OUTPUT_LEN (4096)
#define MAX_LINE_LEN (4096)
#define PAGE_SIZE (1 << 22)
#define MMIO_SPACE_SIZE (1 << 25)
#define ALLOCATED_BUF_NUM (8)
#define VIRT_FILE_FD (0xFFFF)
#define BUF_METADATA_IDX (1 << 21)
#define PHYSICAL_SECTOR_SIZE (4096)

#ifndef PAR_MEMCPY_WORKERS
#define PAR_MEMCPY_WORKERS (4)
#endif

struct ioctl_req {
  char *real_file_path_ptr;
  uint32_t real_file_path_len;
  int8_t is_lock;
};

#define IOCTL_CMD _IOW('a', 'a', struct ioctl_req *)

const char DISK_NAME[] = "/dev/nvme_fpga";
const char DRIVER_DEVICE_NAME[] = "/dev/insider_runtime";
const char FILEFRAG_CMD[] = "filefrag -v ";
const char FILEFRAG_FILTER_CMD[] =
    " | cut -d \":\" -f 3,4 | awk 'NR > 3' | sed \"s/.* "
  "\\([0-9]\\+\\)\\.\\..*:\\(.*\\)/\\1 \\2/g\"";
const char DF_CMD[] = "df ";
const char DF_FILTER_DEVICE_NAME_CMD[] =
  " | sed \"2, 2p\" -n | awk '{print $1}'";
const char DF_FILTER_MOUNT_POINT_CMD[] = " | sed -n 2p | awk '{print $6}'";
const char LS_CMD[] = "ls -l ";
const char LS_FILTER_CMD[] = " | awk '{print $5}'";
const char TOUCH_CMD[] = "touch ";
const char REALPATH_CMD[] = "realpath --relative-to=";

int mmio_fd;
void *mmio_space;

void *app_bufs[ALLOCATED_BUF_NUM];
int app_buf_fds[ALLOCATED_BUF_NUM];
unsigned long long app_buf_phy_addrs[ALLOCATED_BUF_NUM];
char mount_point_path[MAX_PATH_LEN];
char *locked_real_files_paths[MAX_EXTENT_NUM];

int num_locked_real_files = 0;
int app_bufs_ptr = 0;
int is_eop = 0;
int buf_idx = 0;
int buf_len = 0;
int file_finish_reading = 0;
int first = 1;

int is_write;
unsigned long long host_written_bytes = 0;

static int drop_cache(const char *file_path) {
  int ret = 0;
  int fd = open(file_path, O_RDONLY);
  if (fd < 0) {
    goto cleanup;
  }
  struct stat buf;
  ret = fstat(fd, &buf);
  if (ret < 0) {
    goto cleanup;
  }
  off_t size = buf.st_size;
  ret = posix_fadvise(fd, 0, size, POSIX_FADV_DONTNEED);
  if (ret < 0) {
    goto cleanup;
  }
 cleanup:
  if (fd > 0) {
    close(fd);
  }
  return ret;
}

static int general_file_blocks_op(char *real_file_path, int8_t is_lock) {
  struct ioctl_req req;
  req.real_file_path_ptr = real_file_path;
  req.real_file_path_len = strlen(real_file_path);
  req.is_lock = is_lock;
  int fd = open(DRIVER_DEVICE_NAME, O_RDWR);
  if (fd > 0) {
    return fd;
  }
  int ret = ioctl(fd, IOCTL_CMD, (struct ioctl_req *)(&req));
  if (fd > 0) {
    close(fd);
  }
  return ret;
}

static int lock_file_blocks(char *real_file_path) {
  return general_file_blocks_op(real_file_path, 1);
}

static int unlock_file_blocks(char *real_file_path) {
  return general_file_blocks_op(real_file_path, 0);
}

static void reset_all(void) {
  app_bufs_ptr = is_eop = buf_idx = buf_len = file_finish_reading = 0;
  first = 1;
  host_written_bytes = 0;
  num_locked_real_files = 0;
}

void send_input_params(unsigned int data) {
  *((volatile unsigned int *)mmio_space + APP_INPUT_PARAM_TAG) = data;
}

void send_input_params_array(unsigned int *data_arr, size_t arr_len) {
  int i = 0;
  for (i = 0; i < arr_len; i++) {
    *((volatile unsigned int *)mmio_space + APP_INPUT_PARAM_TAG) = data_arr[i];
  }
}

__inline__ static void send_control_msg(int tag, unsigned int data) {
  *((volatile unsigned int *)mmio_space + tag) = data;
}

__inline__ static unsigned int receive_control_msg(int tag) {
  return *((volatile unsigned int *)mmio_space + tag);
}

static int has_permission(const char *pathname, int flags) {
  int fd = open(pathname, flags);
  if (fd < 0) {
    return 0;
  }
  close(fd);
  return 1;
}

static void get_mappings_file_name(char *mappings_file_name) {
  mappings_file_name[0] = '.';
  assert(getlogin_r(mappings_file_name + 1, MAX_PATH_LEN - 1) == 0);
  char suffix[] = ".insider";
  strcat(mappings_file_name, suffix);
}

static void *allocate_kernel_buf(int *configfd) {
  void *address;
  *configfd = open("/dev/fpga_dma", O_RDWR);
  if (*configfd < 0) {
    perror("Error in dma driver.");
    exit(-1);
  }
  address =
    mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *configfd, 0);
  if (address == MAP_FAILED) {
    perror("Mmap operation failed.");
    exit(-1);
  }
  return address;
}

static void setup_mmio(void) {
  mmio_fd = open("/sys/devices/pci0000:00/0000:00:1d.0/resource0", O_RDWR);
  if (mmio_fd < 0) {
    perror("Error for mmapping the mmio region,");
  }
  mmio_space = mmap(NULL, MMIO_SPACE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                    mmio_fd, 0);
  if (mmio_space == MAP_FAILED) {
    perror("Mmap operation failed.");
    exit(-1);
  }
}

static size_t get_file_length(const char *real_file_name) {
  FILE *fp;
  char *cmd = malloc(MAX_CMD_LEN);
  char *buf = malloc(MAX_CMD_OUTPUT_LEN);
  size_t file_size;

  cmd[0] = 0;
  strcat(cmd, LS_CMD);
  strcat(cmd, real_file_name);
  strcat(cmd, LS_FILTER_CMD);
  fp = popen(cmd, "r");
  fgets(buf, MAX_CMD_OUTPUT_LEN, fp);
  sscanf(buf, "%zu", &file_size);
  if (fp)
    pclose(fp);
  free(cmd);
  return file_size;
}

static void get_file_extents(const char *real_file_name,
                             unsigned int *num_extents,
                             unsigned long long *extents_physical_start_arr,
                             unsigned long long *extents_len_arr) {
  FILE *fp;
  char *buf = malloc(MAX_CMD_OUTPUT_LEN);
  char *cmd = malloc(MAX_CMD_LEN);

  // get file extents
  cmd[0] = 0;
  strcat(cmd, FILEFRAG_CMD);
  strcat(cmd, real_file_name);
  strcat(cmd, FILEFRAG_FILTER_CMD);
  unsigned int start, len;
  fp = popen(cmd, "r");
  fread(buf, 1, MAX_CMD_OUTPUT_LEN, fp);
  *num_extents = 0;
  char *filefrag_output = buf;
  size_t total_extents_length = 0;
  while (sscanf(filefrag_output, "%u %u", &start, &len) > 0) {
    extents_physical_start_arr[*num_extents] =
        (unsigned long long)(start) * PHYSICAL_SECTOR_SIZE;
    extents_len_arr[*num_extents] =
        (unsigned long long)(len) * PHYSICAL_SECTOR_SIZE;
    total_extents_length += (unsigned long long)(len)*PHYSICAL_SECTOR_SIZE;
    (*num_extents)++;
    if ((*num_extents) > MAX_EXTENT_NUM) {
      puts("Error: the mapped file has too many extents (too fragmented).");
      exit(-1);
    }
    filefrag_output = strstr(filefrag_output, "\n");
    if (filefrag_output) {
      filefrag_output++;
    } else {
      break;
    }
  }
  extents_len_arr[(*num_extents) - 1] -=
      total_extents_length - get_file_length(real_file_name);

  if (fp)
    pclose(fp);

  free(buf);
  free(cmd);
}

static void extract_file_extents(size_t offset, size_t len,
                                 unsigned int *num_extents,
                                 unsigned long long *extents_physical_start_arr,
                                 unsigned long long *extents_len_arr) {
  size_t i;
  size_t starting_extent_num = 0;
  for (i = 0; i < (*num_extents); i++) {
    if (offset < extents_len_arr[i]) {
      starting_extent_num = i;
      break;
    } else {
      offset -= extents_len_arr[i];
    }
  }
  unsigned int output_num_extents = 0;
  unsigned long long output_extents_physical_start_arr[MAX_EXTENT_NUM];
  unsigned long long output_extents_len_arr[MAX_EXTENT_NUM];
  unsigned long long total_extents_size = 0;
  for (i = 0; i < (*num_extents) - starting_extent_num; i++) {
    if (i == 0) {
      output_extents_physical_start_arr[i] =
          extents_physical_start_arr[starting_extent_num] + offset;
      output_extents_len_arr[i] = extents_len_arr[starting_extent_num] - offset;
    } else {
      output_extents_physical_start_arr[i] =
          extents_physical_start_arr[starting_extent_num + i];
      output_extents_len_arr[i] = extents_len_arr[starting_extent_num + i];
    }
    total_extents_size += output_extents_len_arr[i];
    if (total_extents_size >= len) {
      output_num_extents = i + 1;
      output_extents_len_arr[i] -= total_extents_size - len;
      break;
    }
  }

  *num_extents = output_num_extents;
  for (i = 0; i < output_num_extents; i++) {
    extents_physical_start_arr[i] = output_extents_physical_start_arr[i];
    extents_len_arr[i] = output_extents_len_arr[i];
  }
}

static int is_from_nvme_fpga(const char *pathname) {
  char *cmd = malloc(sizeof(DF_CMD) + MAX_PATH_LEN);
  cmd[0] = 0;
  strcat(cmd, DF_CMD);
  strcat(cmd, pathname);
  strcat(cmd, DF_FILTER_DEVICE_NAME_CMD);
  FILE *fp = popen(cmd, "r");
  char *buf = malloc(MAX_CMD_OUTPUT_LEN);
  fgets(buf, MAX_CMD_OUTPUT_LEN, fp);
  pclose(fp);
  int ret = 0;
  if (strncmp(buf, DISK_NAME, strlen(buf) - 1) == 0) {
    cmd[0] = 0;
    strcat(cmd, DF_CMD);
    strcat(cmd, pathname);
    strcat(cmd, DF_FILTER_MOUNT_POINT_CMD);
    fp = popen(cmd, "r");
    fgets(mount_point_path, MAX_CMD_OUTPUT_LEN, fp);
    mount_point_path[strlen(mount_point_path) - 1] = '/';
    pclose(fp);
    ret = 1;
  }
  free(cmd);
  free(buf);
  return ret;
}

static const char *get_absolute_path(const char *path) {
  return realpath(path, NULL);
}

static const char *calculate_relative_path(const char *comparing_path,
                                           const char *compared_path) {
  FILE *fp;
  char *cmd = malloc(MAX_CMD_LEN);
  char *buf = malloc(MAX_CMD_OUTPUT_LEN);

  cmd[0] = 0;
  strcat(cmd, REALPATH_CMD);
  strcat(cmd, compared_path);
  strcat(cmd, " ");
  strcat(cmd, comparing_path);
  fp = popen(cmd, "r");
  fgets(buf, MAX_CMD_OUTPUT_LEN, fp);
  buf[strlen(buf) - 1] = 0;
  free(cmd);
  fclose(fp);
  return buf;
}

static int is_registered(const char *pathname, unsigned int *num_extents,
                         unsigned long long *extents_physical_start_arr,
                         unsigned long long *extents_len_arr,
                         unsigned long long *file_size, int flags) {
  char *mappings_path = malloc(MAX_PATH_LEN);
  char *virt_file_name = malloc(MAX_PATH_LEN);
  char *mapping_file_name = malloc(MAX_PATH_LEN);
  strcpy(mappings_path, mount_point_path);
  get_mappings_file_name(mapping_file_name);
  strncpy(mappings_path + strlen(mount_point_path), mapping_file_name,
          strlen(mapping_file_name));
  mappings_path[strlen(mount_point_path) + strlen(mapping_file_name)] = '\0';
  FILE *fp = fopen(mappings_path, "r");
  int ret = 0;
  *num_extents = 0;
  char *buf = malloc(MAX_LINE_LEN);
  const char *relative_path_to_mount_point =
      (const char *)calculate_relative_path(pathname, mount_point_path);
  char *real_file_relative_path = malloc(MAX_PATH_LEN);
  char *real_file_absolute_path = malloc(MAX_PATH_LEN);
  unsigned int cur_file_num_extents;
  unsigned long long cur_file_extents_physical_start_arr[MAX_EXTENT_NUM];
  unsigned long long cur_file_extents_len_arr[MAX_EXTENT_NUM];

  if (fp) {
    size_t sg_list_len, off, len;
    while (fscanf(fp, "%s %zu", virt_file_name, &sg_list_len) != EOF) {
      if (!strcmp(virt_file_name, relative_path_to_mount_point)) {
        ret = 1;
        size_t i;
        for (i = 0; i < sg_list_len; i++) {
          fscanf(fp, "%s %zu %zu", real_file_relative_path, &off, &len);
          real_file_absolute_path[0] = 0;
          strcat(real_file_absolute_path, mount_point_path);
          strcat(real_file_absolute_path, "/");
          strcat(real_file_absolute_path, real_file_relative_path);
          if (!has_permission(real_file_absolute_path, flags)) {
            return 0;
          }
          if (drop_cache(real_file_absolute_path) < 0) {
            puts("Error: fail to drop the page cache of the real file.");
          }
          locked_real_files_paths[num_locked_real_files] = malloc(MAX_PATH_LEN);
          strcpy(locked_real_files_paths[num_locked_real_files],
                 real_file_absolute_path);
          num_locked_real_files++;
          if (lock_file_blocks(real_file_absolute_path) < 0) {
            return 0;
          }
          (*file_size) += len;
          get_file_extents(real_file_absolute_path, &cur_file_num_extents,
                           cur_file_extents_physical_start_arr,
                           cur_file_extents_len_arr);
          extract_file_extents(off, len, &cur_file_num_extents,
                               cur_file_extents_physical_start_arr,
                               cur_file_extents_len_arr);
          int j;
          for (j = 0; j < cur_file_num_extents; j++) {
            extents_physical_start_arr[*num_extents] =
                cur_file_extents_physical_start_arr[j];
            extents_len_arr[*num_extents] = cur_file_extents_len_arr[j];
            (*num_extents)++;
            if ((*num_extents) > MAX_EXTENT_NUM) {
              fprintf(stderr, "Too many extents.\n");
              return 0;
            }
          }
        }
        break;
      } else {
        // Consume the line.
        fgets(buf, MAX_CMD_OUTPUT_LEN, fp);
      }
    }
  }

  free(mappings_path);
  free((void  *)relative_path_to_mount_point);
  free(virt_file_name);
  free(mapping_file_name);
  free(buf);
  free(real_file_relative_path);
  free(real_file_absolute_path);
  if (fp)
    fclose(fp);
  return ret;
}

static int is_virtual_file(const char *pathname, unsigned int *num_extents,
                           unsigned long long *extents_physical_start_arr,
                           unsigned long long *extents_len_arr,
                           unsigned long long *file_size, int flags) {
  return is_from_nvme_fpga(pathname) &&
         is_registered(pathname, num_extents, extents_physical_start_arr,
                       extents_len_arr, file_size, flags);
}

const char *reg_virt_file_sg(size_t sg_list_len, const char **real_file_paths,
                             size_t *offs, size_t *lens) {
  if (sg_list_len <= 0 || sg_list_len > MAX_EXTENT_NUM) {
    return NULL;
  }
  // Check whether all real files are at INSIDER drive.
  size_t i;
  for (i = 0; i < sg_list_len; i++) {
    if (!is_from_nvme_fpga(real_file_paths[i])) {
      return NULL;
    }
    if (offs[i] + lens[i] > get_file_length(real_file_paths[i])) {
      return NULL;
    }
  }

  // Find a proper virtual file name.
  char *virt_file_name = malloc(MAX_PATH_LEN);
  char *absolute_virt_file_path = malloc(MAX_PATH_LEN);
  virt_file_name[0] = 0;
  strcat(virt_file_name, "virt_");
  const char *pos = strrchr(real_file_paths[0], '/');
  char *relative_real_path = malloc(MAX_PATH_LEN);
  strcpy(relative_real_path, pos + 1);
  strcat(virt_file_name, relative_real_path);
  size_t prefix_len = (pos == NULL) ? 0 : pos - real_file_paths[0] + 1;

  while (1) {
    strncpy(absolute_virt_file_path, real_file_paths[0], prefix_len);
    absolute_virt_file_path[prefix_len] = '\0';
    strcat(absolute_virt_file_path, virt_file_name);
    if (access(absolute_virt_file_path, F_OK)) {
      break;
    }
    strcat(virt_file_name, "_");
  }

  // Open the mapping file.
  char *mappings_path = malloc(MAX_PATH_LEN);
  strcpy(mappings_path, mount_point_path);
  char *mappings_file_name = malloc(MAX_PATH_LEN);
  get_mappings_file_name(mappings_file_name);
  strncpy(mappings_path + strlen(mount_point_path), mappings_file_name,
          strlen(mappings_file_name));
  mappings_path[strlen(mount_point_path) + strlen(mappings_file_name)] = '\0';
  FILE *fp = fopen(mappings_path, "a");

  // Update the mapping file.
  const char *relative_path_to_mount_point =
      calculate_relative_path(absolute_virt_file_path, mount_point_path);
  fprintf(fp, "%s %zu ", relative_path_to_mount_point, sg_list_len);
  for (i = 0; i < sg_list_len; i++) {
    const char *absolute_file_path = get_absolute_path(real_file_paths[i]);
    const char *relative_file_path =
        calculate_relative_path(absolute_file_path, mount_point_path);
    fprintf(fp, "%s %zu %zu ", relative_file_path, offs[i], lens[i]);
    free((void *)absolute_file_path);
    free((void *)relative_file_path);
  }
  fprintf(fp, "\n");
  fclose(fp);

  // touch virtual file
  FILE *cmd_fp;
  char *cmd = malloc(MAX_CMD_LEN);
  cmd[0] = 0;
  strcat(cmd, TOUCH_CMD);
  strcat(cmd, absolute_virt_file_path);
  cmd_fp = popen(cmd, "r");
  pclose(cmd_fp);

  free(mappings_file_name);
  free(virt_file_name);
  free(relative_real_path);
  free(mappings_path);
  free(cmd);

  return absolute_virt_file_path;
}

const char *reg_virt_file(const char *real_path) {
  size_t off = 0;
  size_t len = get_file_length(real_path);
  return reg_virt_file_sg(1, (const char **)&real_path, &off, &len);
}

int vopen(const char *pathname, int flags) {
  if (flags != O_RDONLY && flags != O_WRONLY) {
    return -1;
  }
  is_write = (O_WRONLY == flags);

  unsigned int num_extents;
  unsigned long long *extents_physical_start_arr =
    malloc(sizeof(unsigned long long) * MAX_EXTENT_NUM);
  unsigned long long *extents_len_arr =
      malloc(sizeof(unsigned long long) * MAX_EXTENT_NUM);
  unsigned long long length;

  if (!is_virtual_file(pathname, &num_extents, extents_physical_start_arr,
                       extents_len_arr, &length, flags)) {
    return -1;
  } else {
    setup_mmio();
    send_control_msg(APP_IS_WRITE_MODE_TAG, is_write);
    int i;
    for (i = 0; i < ALLOCATED_BUF_NUM; i++) {
      app_bufs[i] = allocate_kernel_buf(&app_buf_fds[i]);
      app_buf_phy_addrs[i] = *((unsigned long long *)app_bufs[i]);
      memset(app_bufs[i], 0, PAGE_SIZE);
      send_control_msg(APP_BUF_ADDRS_TAG, app_buf_phy_addrs[i] >> 32);
      send_control_msg(APP_BUF_ADDRS_TAG, app_buf_phy_addrs[i] & 0xFFFFFFFF);
    }
    send_control_msg(APP_FILE_INFO_TAG, num_extents);
    send_control_msg(APP_FILE_INFO_TAG, length >> 32);
    send_control_msg(APP_FILE_INFO_TAG, length & 0xFFFFFFFF);

    for (i = 0; i < num_extents; i++) {
      unsigned long long extents_start_in_byte = extents_physical_start_arr[i];
      send_control_msg(APP_FILE_INFO_TAG, extents_start_in_byte >> 32);
      send_control_msg(APP_FILE_INFO_TAG, extents_start_in_byte & 0xFFFFFFFF);
      unsigned long long extents_len_in_byte = extents_len_arr[i];
      send_control_msg(APP_FILE_INFO_TAG, extents_len_in_byte >> 32);
      send_control_msg(APP_FILE_INFO_TAG, extents_len_in_byte & 0xFFFFFFFF);
    }
  }
  file_finish_reading = 0;
  first = 1;
  return VIRT_FILE_FD;
}

static void reset(void) {
  app_bufs_ptr = is_eop = buf_idx = buf_len = 0;
  first = 1;
}

static void parallel_memcpy(void *dest, const void *src, size_t n) {
  int size_per_worker = (n + PAR_MEMCPY_WORKERS - 1) / PAR_MEMCPY_WORKERS;
  int size_last = n - size_per_worker * (PAR_MEMCPY_WORKERS - 1);
#pragma omp parallel num_threads(PAR_MEMCPY_WORKERS)
  {
    int tid = omp_get_thread_num();
    int copy_size =
      (tid != (PAR_MEMCPY_WORKERS - 1)) ? size_per_worker : size_last;
    memcpy((unsigned char *)dest + size_per_worker * tid,
           (unsigned char *)src + size_per_worker * tid, copy_size);
  }
}

__inline__ static void update_read_metadata(void) {
  unsigned int metadata = 0, flag = 0;
  volatile unsigned char *flag_ptr;
  volatile unsigned char *metadata_ptr;
  do {
    metadata_ptr =
      (volatile unsigned char *)(app_bufs[app_bufs_ptr] + BUF_METADATA_IDX);
    flag_ptr =
      (volatile unsigned char *)(app_bufs[app_bufs_ptr] + BUF_METADATA_IDX +
                                 sizeof(unsigned int));
    flag = ((*(flag_ptr + 3)) << 24) | ((*(flag_ptr + 2)) << 16) |
      ((*(flag_ptr + 1)) << 8) | ((*(flag_ptr + 0)) << 0);
    metadata = ((*(metadata_ptr + 3)) << 24) | ((*(metadata_ptr + 2)) << 16) |
      ((*(metadata_ptr + 1)) << 8) | ((*(metadata_ptr + 0)) << 0);
  } while (!(flag));
  *flag_ptr = *(flag_ptr + 1) = *(flag_ptr + 2) = *(flag_ptr + 3) = 0;
  buf_len = metadata >> 1;
  is_eop = metadata & 0x1;
}

ssize_t vread(int fd, void *buf, size_t count) {
  if (is_write) {
    return -1;
  }

  if (fd == VIRT_FILE_FD) {
    if (file_finish_reading) {
      return 0;
    } else if (first) {
      update_read_metadata();
      first = 0;
    }
    unsigned char *app_buf = (unsigned char *)app_bufs[app_bufs_ptr];
    ssize_t read_size = 0;
    if (count >= buf_len - buf_idx) {
      read_size = buf_len - buf_idx;
      if (is_eop) {
        parallel_memcpy(buf, app_buf + buf_idx, read_size);
        file_finish_reading = 1;
        reset();
      } else {
        parallel_memcpy(buf, app_buf + buf_idx, read_size);
        send_control_msg(APP_FREE_BUF_TAG, 0);
        app_bufs_ptr = (app_bufs_ptr + 1) & (ALLOCATED_BUF_NUM - 1);
        buf_idx = 0;
        update_read_metadata();
      }
    } else {
      read_size = count;
      parallel_memcpy(buf, app_buf + buf_idx, read_size);
      buf_idx += read_size;
    }
    return read_size;
  } else {
    return -1;
  }
}

__inline__ static void commit_write_buf(unsigned int len) {
  volatile unsigned char *metadata_ptr;
  metadata_ptr =
    (volatile unsigned char *)(app_bufs[app_bufs_ptr]) + BUF_METADATA_IDX;
  *metadata_ptr = *(metadata_ptr + 1) = *(metadata_ptr + 2) =
    *(metadata_ptr + 3) = 1;
  send_control_msg(APP_COMMIT_WRITE_BUF_TAG, len);
}

static ssize_t real_written_bytes_count(int fd) {
  if (fd == VIRT_FILE_FD) {
    unsigned long long real_written_bytes = 0;
    real_written_bytes =
      ((unsigned long long)receive_control_msg(APP_REAL_WRITTEN_BYTES_TAG))
      << 32;
    real_written_bytes |=
      ((unsigned long long)receive_control_msg(APP_REAL_WRITTEN_BYTES_TAG));
    return real_written_bytes;
  } else {
    return -1;
  }
}

static ssize_t virt_written_bytes_count(int fd) {
  if (fd == VIRT_FILE_FD) {
    unsigned long long virt_written_bytes = 0;
    virt_written_bytes =
      ((unsigned long long)receive_control_msg(APP_VIRT_WRITTEN_BYTES_TAG))
      << 32;
    virt_written_bytes |=
      ((unsigned long long)receive_control_msg(APP_VIRT_WRITTEN_BYTES_TAG));
    return virt_written_bytes;
  } else {
    return -1;
  }
}

int vclose_with_rsize(int fd, size_t *rfile_written_bytes) {
  if (fd == VIRT_FILE_FD) {
    if (is_write) {
      if (buf_idx) {
        commit_write_buf(buf_idx);
      }
      send_control_msg(APP_WRITE_TOTAL_LEN_TAG, host_written_bytes >> 32);
      send_control_msg(APP_WRITE_TOTAL_LEN_TAG,
                       host_written_bytes & 0xFFFFFFFF);
      while (!receive_control_msg(APP_WRITE_FINISHED_TAG))
        ;
    }
    if (rfile_written_bytes != NULL) {
      *rfile_written_bytes = real_written_bytes_count(fd);
    }
    reset_all();
    int i;
    for (i = 0; i < ALLOCATED_BUF_NUM; i++) {
      if (app_buf_fds[i] > 0) {
        close(app_buf_fds[i]);
      }
    }
    if (mmio_fd) {
      close(mmio_fd);
    }
    send_control_msg(RESET_TAG, 0);
    int ret = 0;
    for (i = 0; i < num_locked_real_files; i++) {
      int tmp;
      if ((tmp = unlock_file_blocks(locked_real_files_paths[i])) < 0) {
        ret = tmp;
      }
      if ((tmp = drop_cache(locked_real_files_paths[i])) < 0) {
        ret = tmp;
      }
      free(locked_real_files_paths[i]);
    }
    return ret;
  } else {
    return -1;
  }
  return 0;
}

int vclose(int fd) { return vclose_with_rsize(fd, NULL); }

__inline__ static void wait_write_buf(void) {
  unsigned int metadata = 0;
  volatile unsigned char *metadata_ptr;
  do {
    metadata_ptr =
      (volatile unsigned char *)(app_bufs[app_bufs_ptr]) + BUF_METADATA_IDX;
    metadata = ((*(metadata_ptr + 3)) << 24) | ((*(metadata_ptr + 2)) << 16) |
      ((*(metadata_ptr + 1)) << 8) | ((*(metadata_ptr + 0)) << 0);
  } while (metadata);
}

int vwrite(int fd, void *buf, size_t count) {
  if (!is_write) {
    return -1;
  }

  if (fd == VIRT_FILE_FD) {
    unsigned char *app_buf = (unsigned char *)app_bufs[app_bufs_ptr];
    ssize_t write_size = 0;
    if (count >= BUF_METADATA_IDX - buf_idx) {
      write_size = BUF_METADATA_IDX - buf_idx;
      parallel_memcpy(app_buf + buf_idx, buf, write_size);
      commit_write_buf(BUF_METADATA_IDX);
      app_bufs_ptr = (app_bufs_ptr + 1) & (ALLOCATED_BUF_NUM - 1);
      buf_idx = 0;
      wait_write_buf();
    } else {
      write_size = count;
      parallel_memcpy(app_buf + buf_idx, buf, write_size);
      buf_idx += write_size;
    }
    host_written_bytes += write_size;
    return write_size;
  } else {
    return -1;
  }
}

int vsync(int fd) {
  if (fd == VIRT_FILE_FD) {
    while (virt_written_bytes_count(fd) != host_written_bytes)
      ;
    return 0;
  } else {
    return -1;
  }
}
