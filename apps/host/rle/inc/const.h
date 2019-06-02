#ifndef CONST_H
#define CONST_H

#include <cstdint>
#include <cstdio>

#define READ_BUF_SIZE (2 * 1024 * 1024)
#define WRITE_BUF_SIZE (256 * READ_BUF_SIZE)
#define OUTPUT_WIDTH (187544LL)
#define OUTPUT_HEIGHT (129000LL)
#define OUTPUT_DEPTH (8)
#define OUTPUT_PLANES (1)
#define INFLAT_DATA_SIZE (OUTPUT_WIDTH * OUTPUT_HEIGHT * OUTPUT_DEPTH / 8)

struct __attribute__((__packed__)) bitmap_header {
  uint8_t magicNumber[2];
  uint32_t size;
  uint8_t reserved[4];
  uint32_t startOffset;
  uint32_t headerSize;
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t depth;
  uint32_t compression;
  uint32_t imageSize;
  uint32_t xPPM;
  uint32_t yPPM;
  uint32_t nUsedColors;
  uint32_t nImportantColors;

  bitmap_header()
      : magicNumber{'B', 'M'}, size(), reserved{0},
        startOffset(sizeof(bitmap_header)), headerSize(sizeof(bitmap_header)),
        width(OUTPUT_WIDTH), height(OUTPUT_HEIGHT), planes(OUTPUT_PLANES),
        depth(OUTPUT_DEPTH), compression(1), imageSize(),
        xPPM(), yPPM(), nUsedColors(), nImportantColors() {}
};

void print_header(bitmap_header *header) {
  fprintf(stdout, "\tMagic number: %c%c\n", header->magicNumber[0],
          header->magicNumber[1]);
  fprintf(stdout, "\tSize: %u\n", header->size);
  fprintf(stdout, "\tReserved: %u %u %u %u\n", header->reserved[0],
          header->reserved[1], header->reserved[2], header->reserved[3]);
  fprintf(stdout, "\tStart offset: %u\n", header->startOffset);
  fprintf(stdout, "\tHeader size: %u\n", header->headerSize);
  fprintf(stdout, "\tWidth: %u\n", header->width);
  fprintf(stdout, "\tHeight: %u\n", header->height);
  fprintf(stdout, "\tPlanes: %u\n", header->planes);
  fprintf(stdout, "\tDepth: %u\n", header->depth);
  fprintf(stdout, "\tCompression: %u\n", header->compression);
  fprintf(stdout, "\tImage size: %u\n", header->headerSize);
  fprintf(stdout, "\tX pixels per meters: %u\n", header->xPPM);
  fprintf(stdout, "\tY pixels per meters: %u\n", header->yPPM);
  fprintf(stdout, "\tNb of colors used: %u\n", header->nUsedColors);
  fprintf(stdout, "\tNb of important colors: %u\n", header->nImportantColors);
}

#endif
