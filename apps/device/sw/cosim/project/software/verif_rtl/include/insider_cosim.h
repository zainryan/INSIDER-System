#ifndef INSIDER_COSIM_H_
#define INSIDER_COSIM_H_

#include <fcntl.h>
#include <insider_macros.h>
#include <sh_dpi_tasks.h>
#include <string.h>

#define ALLOCATED_BUF_NUM (8)
#define APP_BUF_SIZE_LOG2 (21)
#define KBUF_SIZE (1 << 22)
#define BUF_METADATA_IDX (1 << 21)
#define VIRT_FILE_FD (0xFFFF)

#define COSIM_DRAMA_ADDR_OFFSET (4LL << 32)
#define COSIM_DRAMB_ADDR_OFFSET (8LL << 32)
#define COSIM_DRAMC_ADDR_OFFSET (12LL << 32)
#define COSIM_DRAMD_ADDR_OFFSET (16LL << 32)

int i;
int first = 1;
unsigned long long app_buf_phy_addrs[ALLOCATED_BUF_NUM];
unsigned int app_bufs_ptr = 0;
int is_eop = 0;
int buf_idx = 0;
int buf_len = 0;
int file_finish_reading = 0;
int is_write;
unsigned long long host_written_bytes = 0;

static unsigned char dram_read(unsigned long long addr);
static void dram_write(unsigned long long addr, unsigned char data);
static void init();
static void update_read_metadata();
static void user_kernel_memcpy(unsigned long long kernel_buf_addr,
                               void *user_buf, size_t count);
static void kernel_user_memcpy(void *user_buf,
                               unsigned long long kernel_buf_addr,
                               size_t count);
static void reset_all();
static void reset();
static void simulator();
static void commit_write_buf(unsigned int len);
static void wait_write_buf();
static ssize_t virt_written_bytes_count(int fd);

int vopen(const char *pathname, int flags);
ssize_t vread(int fd, void *buf, size_t count);
int vwrite(int fd, void *buf, size_t count);
int vclose(int fd);
ssize_t real_written_bytes_count(int fd);
int vsync(int fd);

void cosim_wait(int num_cycles);
void set_physical_file(unsigned char *buf, unsigned int num_extents,
                       unsigned long long extents_start_idx[],
                       unsigned long long extents_len[]);
void get_physical_file(unsigned char *buf, unsigned int num_extents,
                       unsigned long long extents_start_idx[],
                       unsigned long long extents_len[]);
void send_input_param(unsigned int data);
void user_simulation_function();

static void reset_all() {
  app_bufs_ptr = is_eop = buf_idx = buf_len = file_finish_reading = 0;
  first = 1;
  host_written_bytes = 0;
}

static void dram_write(unsigned long long addr, unsigned char data) {
  int dram_id = (addr >> 12) & 3;
  int dram_idx = ((addr >> 14) << 12) + (addr & ((1 << 12) - 1));

  if (dram_id == 0) {
    host_memory_putc(dram_idx + COSIM_DRAMA_ADDR_OFFSET, data);
  } else if (dram_id == 1) {
    host_memory_putc(dram_idx + COSIM_DRAMB_ADDR_OFFSET, data);
  } else if (dram_id == 2) {
    host_memory_putc(dram_idx + COSIM_DRAMC_ADDR_OFFSET, data);
  } else if (dram_id == 3) {
    host_memory_putc(dram_idx + COSIM_DRAMD_ADDR_OFFSET, data);
  }
}

static unsigned char dram_read(unsigned long long addr) {
  int dram_id = (addr >> 12) & 3;
  int dram_idx = ((addr >> 14) << 12) + (addr & ((1 << 12) - 1));

  unsigned char data;
  if (dram_id == 0) {
    data = host_memory_getc(dram_idx + COSIM_DRAMA_ADDR_OFFSET);
  } else if (dram_id == 1) {
    data = host_memory_getc(dram_idx + COSIM_DRAMB_ADDR_OFFSET);
  } else if (dram_id == 2) {
    data = host_memory_getc(dram_idx + COSIM_DRAMC_ADDR_OFFSET);
  } else if (dram_id == 3) {
    data = host_memory_getc(dram_idx + COSIM_DRAMD_ADDR_OFFSET);
  }
  return data;
}

static void init() {
  for (i = 0; i < ALLOCATED_BUF_NUM; i++) {
    if (i == 0) {
      app_buf_phy_addrs[i] = 0;
    } else {
      app_buf_phy_addrs[i] =
          app_buf_phy_addrs[i - 1] + (1 << (APP_BUF_SIZE_LOG2 + 1));
    }
    cl_poke(TAG(APP_BUF_ADDRS_TAG), app_buf_phy_addrs[i] >> 32);
    cl_poke(TAG(APP_BUF_ADDRS_TAG), app_buf_phy_addrs[i] & 0xFFFFFFFF);
  }
}

int vopen(const char *pathname, int flags) {
  if (flags != O_RDONLY && flags != O_WRONLY) {
    puts("Error in vopen(): flags must be O_RDONLY or O_WRONLY");
    exit(-1);
  }
  if (strcmp(pathname, "cosim_phy_file")) {
    puts("Error in vopen(): pathname must be cosim_phy_file");
    exit(-1);
  }
  is_write = (O_WRONLY == flags);
  cl_poke(TAG(APP_IS_WRITE_MODE_TAG), is_write);
  return VIRT_FILE_FD;
}

static void update_read_metadata() {
  unsigned int metadata = 0, flag = 0;
  unsigned long long metadata_addr, flag_addr;
  volatile unsigned char *metadata_ptr;
  do {
    metadata_addr = app_buf_phy_addrs[app_bufs_ptr] + BUF_METADATA_IDX;
    flag_addr = app_buf_phy_addrs[app_bufs_ptr] + BUF_METADATA_IDX +
                sizeof(unsigned int);
    flag = (host_memory_getc(flag_addr + 3) << 24) |
           (host_memory_getc(flag_addr + 2) << 16) |
           (host_memory_getc(flag_addr + 1) << 8) |
           (host_memory_getc(flag_addr + 0) << 0);
    metadata = (host_memory_getc(metadata_addr + 3) << 24) |
               (host_memory_getc(metadata_addr + 2) << 16) |
               (host_memory_getc(metadata_addr + 1) << 8) |
               (host_memory_getc(metadata_addr + 0) << 0);
  } while (!(flag));
  host_memory_putc(flag_addr + 0, 0);
  host_memory_putc(flag_addr + 1, 0);
  host_memory_putc(flag_addr + 2, 0);
  host_memory_putc(flag_addr + 3, 0);
  buf_len = metadata >> 1;
  is_eop = metadata & 0x1;
}

static void kernel_user_memcpy(void *user_buf,
                               unsigned long long kernel_buf_addr,
                               size_t count) {
  for (size_t i = 0; i < count; i++) {
    ((unsigned char *)user_buf)[i] = host_memory_getc(kernel_buf_addr + i);
  }
}

static void user_kernel_memcpy(unsigned long long kernel_buf_addr,
                               void *user_buf, size_t count) {
  for (size_t i = 0; i < count; i++) {
    host_memory_putc(kernel_buf_addr + i, ((unsigned char *)user_buf)[i]);
  }
}

static void reset() {
  app_bufs_ptr = is_eop = buf_idx = buf_len = 0;
  first = 1;
}

ssize_t vread(int fd, void *buf, size_t count) {
  if (fd == VIRT_FILE_FD) {
    if (file_finish_reading) {
      return 0;
    } else if (first) {
      update_read_metadata();
      first = 0;
    }
    unsigned long long kbuf_addr = app_buf_phy_addrs[app_bufs_ptr];
    ssize_t read_size;
    if (count >= buf_len - buf_idx) {
      read_size = buf_len - buf_idx;
      if (is_eop) {
        kernel_user_memcpy(buf, kbuf_addr + buf_idx, read_size);
        file_finish_reading = 1;
        reset();
      } else {
        kernel_user_memcpy(buf, kbuf_addr + buf_idx, read_size);
        cl_poke(TAG(APP_FREE_BUF_TAG), 0);
        app_bufs_ptr = (app_bufs_ptr + 1) & (ALLOCATED_BUF_NUM - 1);
        buf_idx = 0;
        update_read_metadata();
      }
    } else {
      read_size = count;
      kernel_user_memcpy(buf, kbuf_addr + buf_idx, read_size);
      buf_idx += read_size;
    }
    return read_size;
  } else {
    return -1;
  }
}

static void commit_write_buf(unsigned int len) {
  unsigned long long metadata_addr;
  metadata_addr = app_buf_phy_addrs[app_bufs_ptr] + BUF_METADATA_IDX;
  host_memory_putc(metadata_addr, 1);
  host_memory_putc(metadata_addr + 1, 1);
  host_memory_putc(metadata_addr + 2, 1);
  host_memory_putc(metadata_addr + 3, 1);
  cl_poke(TAG(APP_COMMIT_WRITE_BUF_TAG), len);
}

static void wait_write_buf(void) {
  unsigned int metadata = 0;
  unsigned long long metadata_addr;
  do {
    metadata_addr = app_buf_phy_addrs[app_bufs_ptr] + BUF_METADATA_IDX;
    metadata = (host_memory_getc(metadata_addr + 3) << 24) |
               (host_memory_getc(metadata_addr + 2) << 16) |
               (host_memory_getc(metadata_addr + 1) << 8) |
               (host_memory_getc(metadata_addr + 0) << 0);
  } while (metadata);
}

int vwrite(int fd, void *buf, size_t count) {
  if (fd == VIRT_FILE_FD) {
    unsigned long long kbuf_addr = app_buf_phy_addrs[app_bufs_ptr];
    ssize_t write_size = 0;
    if (count >= BUF_METADATA_IDX - buf_idx) {
      write_size = BUF_METADATA_IDX - buf_idx;
      user_kernel_memcpy(kbuf_addr + buf_idx, buf, write_size);
      commit_write_buf(BUF_METADATA_IDX);
      app_bufs_ptr = (app_bufs_ptr + 1) & (ALLOCATED_BUF_NUM - 1);
      buf_idx = 0;
      wait_write_buf();
    } else {
      write_size = count;
      user_kernel_memcpy(kbuf_addr + buf_idx, buf, write_size);
      buf_idx += write_size;
    }
    host_written_bytes += write_size;
    return write_size;
  } else {
    return -1;
  }
}

int vclose(int fd) {
  if (fd == VIRT_FILE_FD) {
    if (is_write) {
      if (buf_idx) {
        commit_write_buf(buf_idx);
      }
      cl_poke(TAG(APP_WRITE_TOTAL_LEN_TAG), host_written_bytes >> 32);
      cl_poke(TAG(APP_WRITE_TOTAL_LEN_TAG), host_written_bytes & 0xFFFFFFFF);
      unsigned int write_finished;
      do {
        cl_peek(TAG(APP_WRITE_FINISHED_TAG), &write_finished);
      } while (!write_finished);
    }
    reset_all();
    puts("vclose invokes cosim_wait(50) to finalize the results.");
    cosim_wait(100);
    cl_poke(TAG(RESET_TAG), 0);
    return 0;
  } else {
    return -1;
  }
}

ssize_t real_written_bytes_count(int fd) {
  if (fd == VIRT_FILE_FD) {
    unsigned long long real_written_bytes = 0;
    unsigned int first_half, second_half;
    cl_peek(TAG(APP_REAL_WRITTEN_BYTES_TAG), &first_half);
    cl_peek(TAG(APP_REAL_WRITTEN_BYTES_TAG), &second_half);
    real_written_bytes = ((unsigned long long)first_half) << 32;
    real_written_bytes |= (unsigned long long)second_half;
    return real_written_bytes;
  } else {
    return -1;
  }
}

void set_physical_file(unsigned char *buf, unsigned int num_extents,
                            unsigned long long extents_start_idx[],
                            unsigned long long extents_len[]) {
  unsigned long long total_len = 0;
  for (int i = 0; i < num_extents; i++) {
    total_len += extents_len[i];
  }
  cl_poke(TAG(APP_FILE_INFO_TAG), num_extents);
  cl_poke(TAG(APP_FILE_INFO_TAG), total_len >> 32);
  cl_poke(TAG(APP_FILE_INFO_TAG), total_len & 0xFFFFFFFF);
  ssize_t buf_idx = 0;
  for (int i = 0; i < num_extents; i++) {
    for (int j = 0; j < extents_len[i]; j++) {
      dram_write(extents_start_idx[i] + j, buf[buf_idx++]);
    }
    cl_poke(TAG(APP_FILE_INFO_TAG), extents_start_idx[i] >> 32);
    cl_poke(TAG(APP_FILE_INFO_TAG), extents_start_idx[i] & 0xFFFFFFFF);
    cl_poke(TAG(APP_FILE_INFO_TAG), extents_len[i] >> 32);
    cl_poke(TAG(APP_FILE_INFO_TAG), extents_len[i] & 0xFFFFFFFF);
  }
}

void get_physical_file(unsigned char *buf, unsigned int num_extents,
                            unsigned long long extents_start_idx[],
                            unsigned long long extents_len[]) {
  ssize_t buf_idx = 0;
  for (int i = 0; i < num_extents; i++) {
    for (int j = 0; j < extents_len[i]; j++) {
      buf[buf_idx++] = dram_read(extents_start_idx[i] + j);
    }
  }
}

void send_input_param(unsigned int data) {
  cl_poke(TAG(APP_INPUT_PARAM_TAG), data);
}

static ssize_t virt_written_bytes_count(int fd) {
  if (fd == VIRT_FILE_FD) {
    unsigned long long virt_written_bytes = 0;
    unsigned int first_half, second_half;
    cl_peek(TAG(APP_VIRT_WRITTEN_BYTES_TAG), &first_half);
    cl_peek(TAG(APP_VIRT_WRITTEN_BYTES_TAG), &second_half);
    virt_written_bytes = ((unsigned long long)first_half) << 32;
    virt_written_bytes |= (unsigned long long)second_half;
    return virt_written_bytes;
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

static void simulator() {
  init();
  user_simulation_function();
}

void cosim_wait(int num_cycles) {
  for (int i = 0; i < num_cycles; i++) {
    cl_poke(TAG(DEBUG_TAG), 0);
    printf("Wait %d...\n", i);
  }
}

#endif
