#include <insider_itc.h>

#include "constant.h"
#include "structure.h"

#include "app_stencil.cpp"

void interconnects() {
  app_stencil(app_input_params, app_input_data, app_output_data);
}
