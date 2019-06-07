#pragma once

#include <insider_kernel.h>

#include "structure.h"

inline void shiftrows (Matrix &data) {
  Matrix new_data;
  new_data(7, 0) = data(7, 0);    
  new_data(15, 8) = data(15, 8);    
  new_data(23, 16) = data(23, 16);    
  new_data(31, 24) = data(31, 24);    

  new_data(32 + 7, 32 + 0) = data(32 + 15, 32 + 8);    
  new_data(32 + 15, 32 + 8) = data(32 + 23, 32 + 16);    
  new_data(32 + 23, 32 + 16) = data(32 + 31, 32 + 24);    
  new_data(32 + 31, 32 + 24) = data(32 + 7, 32 + 0);    

  new_data(64 + 7, 64 + 0) = data(64 + 23, 64 + 16);    
  new_data(64 + 15, 64 + 8) = data(64 + 31, 64 + 24);    
  new_data(64 + 23, 64 + 16) = data(64 + 7, 64 + 0);    
  new_data(64 + 31, 64 + 24) = data(64 + 15, 64 + 8);    


  new_data(96 + 7, 96 + 0) = data(96 + 31, 96 + 24);    
  new_data(96 + 15, 96 + 8) = data(96 + 7, 96 + 0);    
  new_data(96 + 23, 96 + 16) = data(96 + 15, 96 + 8);    
  new_data(96 + 31, 96 + 24) = data(96 + 23, 96 + 16);    

  data = new_data;
}
