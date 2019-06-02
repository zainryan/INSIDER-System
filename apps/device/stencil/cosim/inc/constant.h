#ifndef CONSTANT_H_
#define CONSTANT_H_

/* // BUS_WIDTH % sizeof(Point) should be 0. */
/* #define BUS_WIDTH (64) */
/* #define STENCIL_WIDTH (3) */
/* #define STENCIL_HEIGHT (3) */
/* #define PICTURE_WIDTH (1920) */
/* // PADDING_WIDTH should be larger than STENCIL_WIDTH. */
/* // (PADDING_WIDTH + PICTURE_WIDTH) % (BUS_WIDTH / sizeof(Point)) should be 0. */
/* // 4B padding in left and 4B padding in right. */
/* #define PADDING_WIDTH (16) */
#define BUS_WIDTH (64)
#define STENCIL_WIDTH (3)
#define STENCIL_HEIGHT (3)
#define PICTURE_WIDTH (256)
#define PICTURE_HEIGHT (32)
/* #define PICTURE_WIDTH (1920) */
/* #define PICTURE_HEIGHT (1080) */
// PADDING_WIDTH should be larger than STENCIL_WIDTH.
// (PADDING_WIDTH + PICTURE_WIDTH) % (BUS_WIDTH / sizeof(Point)) should be 0.
// 4B padding in left and 4B padding in right.
#define PADDING_WIDTH (0)
#define PADDING_HEIGHT (STENCIL_HEIGHT - 1)
#define NUM_POINT_PER_INPUT (BUS_WIDTH / sizeof(Point))

#endif
