#include <insider_itc.h>

#include "constant.h"
#include "structure.h"

#include "app_knn.cpp"

// describe the interconnection
void interconnects() {
  app_knn(app_input_data, app_output_data, app_input_params);
}
