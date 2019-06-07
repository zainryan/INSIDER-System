#include <insider_itc.h>

#include "structure.h"

#include "app_aes_start.cpp"
#include "app_aes_process_0.cpp"
#include "app_aes_process_1.cpp"
#include "app_aes_process_2.cpp"
#include "app_aes_process_3.cpp"
#include "app_aes_process_4.cpp"
#include "app_aes_process_5.cpp"
#include "app_aes_process_6.cpp"
#include "app_aes_process_7.cpp"
#include "app_aes_process_8.cpp"
#include "app_aes_end.cpp"

void interconnects() {

    ST_Queue<APP_Data> app_aes_proc_0_input_data(8);
    ST_Queue<APP_Data> app_aes_proc_1_input_data(8);
    ST_Queue<APP_Data> app_aes_proc_2_input_data(8);
    ST_Queue<APP_Data> app_aes_proc_3_input_data(8);
    ST_Queue<APP_Data> app_aes_proc_4_input_data(8);
    ST_Queue<APP_Data> app_aes_proc_5_input_data(8);
    ST_Queue<APP_Data> app_aes_proc_6_input_data(8);
    ST_Queue<APP_Data> app_aes_proc_7_input_data(8);
    ST_Queue<APP_Data> app_aes_proc_8_input_data(8);
    ST_Queue<APP_Data> app_aes_end_input_data(8);

    ST_Queue<unsigned int> app_aes_proc_0_params(8);
    ST_Queue<unsigned int> app_aes_proc_1_params(8);
    ST_Queue<unsigned int> app_aes_proc_2_params(8);
    ST_Queue<unsigned int> app_aes_proc_3_params(8);
    ST_Queue<unsigned int> app_aes_proc_4_params(8);
    ST_Queue<unsigned int> app_aes_proc_5_params(8);
    ST_Queue<unsigned int> app_aes_proc_6_params(8);
    ST_Queue<unsigned int> app_aes_proc_7_params(8);
    ST_Queue<unsigned int> app_aes_proc_8_params(8);
    ST_Queue<unsigned int> app_aes_end_params(8);


  app_aes_start(app_input_params, app_aes_proc_0_params, app_input_data, app_aes_proc_0_input_data);
  app_aes_process_0(app_aes_proc_0_params, app_aes_proc_1_params, app_aes_proc_0_input_data, app_aes_proc_1_input_data);
  app_aes_process_1(app_aes_proc_1_params, app_aes_proc_2_params, app_aes_proc_1_input_data, app_aes_proc_2_input_data);
  app_aes_process_2(app_aes_proc_2_params, app_aes_proc_3_params, app_aes_proc_2_input_data, app_aes_proc_3_input_data);
  app_aes_process_3(app_aes_proc_3_params, app_aes_proc_4_params, app_aes_proc_3_input_data, app_aes_proc_4_input_data);
  app_aes_process_4(app_aes_proc_4_params, app_aes_proc_5_params, app_aes_proc_4_input_data, app_aes_proc_5_input_data);
  app_aes_process_5(app_aes_proc_5_params, app_aes_proc_6_params, app_aes_proc_5_input_data, app_aes_proc_6_input_data);
  app_aes_process_6(app_aes_proc_6_params, app_aes_proc_7_params, app_aes_proc_6_input_data, app_aes_proc_7_input_data);
  app_aes_process_7(app_aes_proc_7_params, app_aes_proc_8_params, app_aes_proc_7_input_data, app_aes_proc_8_input_data);
  app_aes_process_8(app_aes_proc_8_params, app_aes_end_params, app_aes_proc_8_input_data, app_aes_end_input_data);
  app_aes_end(app_aes_end_params, app_aes_end_input_data, app_output_data);
}
