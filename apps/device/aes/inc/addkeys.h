#pragma once

#include <insider_kernel.h>

#include "structure.h"

inline void addkeys(Matrix &data, Matrix &table) {
#pragma HLS inline
  data = data ^ table;
}
