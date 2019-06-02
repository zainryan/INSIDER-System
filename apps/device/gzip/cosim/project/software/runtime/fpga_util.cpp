#include "const.h"
#include "fpga_mgmt.h"
#include "fpga_pci.h"
#include "utils/lcd.h"
#include <pthread.h>

uint16_t _pci_vendor_id = 0x1D0F;
uint16_t _pci_device_id = 0xF000;
pci_bar_handle_t _pci_bar_handle;
struct fpga_pci_bar *_bar;
void *_IBufs[INUM_LIMIT];
void *_OBufs[ONUM_LIMIT];
uint64_t _IBufs_phy[INUM_LIMIT];
uint64_t _OBufs_phy[ONUM_LIMIT];
int _configfds[INUM_LIMIT + ONUM_LIMIT];
uint8_t _inputL = 0;
uint8_t _inputR = 0;
bool _input_empty = false;
uint8_t _outputL = 0;
uint8_t _outputR = 0;
bool _output_empty = false;
unsigned char _comm_Inum = 0;
unsigned char _comm_Onum = 0;
pthread_mutex_t _receive_control_msg_mutex = PTHREAD_MUTEX_INITIALIZER;
