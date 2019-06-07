#include <insider_itc.h>

#include "structure.h"

#include "app_stat_calculator.cpp"
#include "app_stat_rdc_16to8.cpp"
#include "app_stat_rdc_4to2.cpp"
#include "app_stat_rdc_8to4.cpp"
#include "app_stat_writer.cpp"

void interconnects() {
  ST_Queue<APP_Scatter_Data16> app_stat_rdc_16to8_input(8);
  ST_Queue<APP_Scatter_Data8> app_stat_rdc_8to4_input(8);
  ST_Queue<APP_Scatter_Data4> app_stat_rdc_4to2_input(8);
  ST_Queue<APP_Scatter_Data2> app_stat_writer_input(8);
  app_stat_calculator(app_input_params, app_input_data,
                      app_stat_rdc_16to8_input);
  app_stat_rdc_16to8(app_stat_rdc_16to8_input, app_stat_rdc_8to4_input);
  app_stat_rdc_8to4(app_stat_rdc_8to4_input, app_stat_rdc_4to2_input);
  app_stat_rdc_4to2(app_stat_rdc_4to2_input, app_stat_writer_input);
  app_stat_writer(app_stat_writer_input, app_output_data);
}
