#ifndef XIL_GZIP_CONFIG_H_
#define XIL_GZIP_CONFIG_H_

#define VEC 8 

// GMEM Interface Width of Kernel
#define GMEM_DWIDTH 512

// DDR to Kernel Burst Transfer Size
#define GMEM_BURST_SIZE 16

// Block Partition for Memory Alignment
#define BLOCK_PARITION 1024

// LZ77 Uses as Marker Value
#define MARKER 255

const int c_gmem_burst_size = GMEM_BURST_SIZE;
const int c_size_stream_depth = 8;

#endif
