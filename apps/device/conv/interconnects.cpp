#include <insider_itc.h>

#include "structure.h"

#include "app_conv_dist.cpp"
#include "app_conv_mult_0.cpp"
#include "app_conv_mult_1.cpp"
#include "app_conv_mult_2.cpp"
#include "app_conv_mult_3.cpp"
#include "app_conv_mult_4.cpp"
#include "app_conv_mult_5.cpp"
#include "app_conv_mult_6.cpp"
#include "app_conv_mult_7.cpp"
#include "app_conv_mult_8.cpp"
#include "app_conv_mult_9.cpp"
#include "app_conv_mult_10.cpp"
#include "app_conv_mult_11.cpp"
#include "app_conv_mult_12.cpp"
#include "app_conv_mult_13.cpp"
#include "app_conv_mult_14.cpp"
#include "app_conv_mult_15.cpp"
#include "app_conv_mult_dummy.cpp"

#include "app_conv_row_reduce_0.cpp"
#include "app_conv_row_reduce_1.cpp"
#include "app_conv_row_reduce_2.cpp"
#include "app_conv_row_reduce_3.cpp"
#include "app_conv_row_reduce_4.cpp"
#include "app_conv_row_reduce_5.cpp"
#include "app_conv_row_reduce_6.cpp"
#include "app_conv_row_reduce_7.cpp"

#include "app_conv_combine_l0_0.cpp"
#include "app_conv_combine_l0_1.cpp"
#include "app_conv_combine_l0_2.cpp"
#include "app_conv_combine_l0_3.cpp"

#include "app_conv_combine_l1_0.cpp"
#include "app_conv_combine_l1_1.cpp"
#include "app_conv_combine_l2_0.cpp"

#include "app_conv_writer.cpp"

void interconnects() {
  ST_Queue<APP_Mult_Data> app_conv_mult_0_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_1_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_2_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_3_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_4_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_5_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_6_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_7_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_8_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_9_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_10_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_11_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_12_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_13_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_14_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_15_input_data(16);
  ST_Queue<APP_Mult_Data> app_conv_mult_dummy_input_data(16);

  ST_Queue<APP_Param> app_conv_mult_0_param(16);
  ST_Queue<APP_Param> app_conv_mult_1_param(16);
  ST_Queue<APP_Param> app_conv_mult_2_param(16);
  ST_Queue<APP_Param> app_conv_mult_3_param(16);
  ST_Queue<APP_Param> app_conv_mult_4_param(16);
  ST_Queue<APP_Param> app_conv_mult_5_param(16);
  ST_Queue<APP_Param> app_conv_mult_6_param(16);
  ST_Queue<APP_Param> app_conv_mult_7_param(16);
  ST_Queue<APP_Param> app_conv_mult_8_param(16);
  ST_Queue<APP_Param> app_conv_mult_9_param(16);
  ST_Queue<APP_Param> app_conv_mult_10_param(16);
  ST_Queue<APP_Param> app_conv_mult_11_param(16);
  ST_Queue<APP_Param> app_conv_mult_12_param(16);
  ST_Queue<APP_Param> app_conv_mult_13_param(16);
  ST_Queue<APP_Param> app_conv_mult_14_param(16);
  ST_Queue<APP_Param> app_conv_mult_15_param(16);
  ST_Queue<APP_Param> app_conv_mult_dummy_param(16);

  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_0_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_1_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_2_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_3_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_4_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_5_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_6_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_7_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_8_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_9_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_10_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_11_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_12_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_13_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_14_input_data(16);
  ST_Queue<APP_Reduce_Data> app_conv_row_reduce_15_input_data(16);

  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_0(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_1(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_2(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_3(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_4(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_5(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_6(16);
  ST_Queue<APP_Combine_L0_Data> app_conv_combine_l0_input_data_7(16);

  ST_Queue<APP_Combine_L1_Data> app_conv_combine_l1_input_data_0(16);
  ST_Queue<APP_Combine_L1_Data> app_conv_combine_l1_input_data_1(16);
  ST_Queue<APP_Combine_L1_Data> app_conv_combine_l1_input_data_2(16);
  ST_Queue<APP_Combine_L1_Data> app_conv_combine_l1_input_data_3(16);

  ST_Queue<APP_Combine_L2_Data> app_conv_combine_l2_input_data_0(16);
  ST_Queue<APP_Combine_L2_Data> app_conv_combine_l2_input_data_1(16);

  ST_Queue<APP_Data> app_conv_writer_input_data(16); 

  app_conv_dist(app_input_params, app_input_data, app_conv_mult_0_param, app_conv_mult_0_input_data);
 
  app_conv_mult_0(app_conv_mult_0_param, app_conv_mult_1_param, app_conv_mult_0_input_data, app_conv_mult_1_input_data, app_conv_row_reduce_0_input_data);
  app_conv_mult_1(app_conv_mult_1_param, app_conv_mult_2_param, app_conv_mult_1_input_data, app_conv_mult_2_input_data, app_conv_row_reduce_1_input_data);
  app_conv_mult_2(app_conv_mult_2_param, app_conv_mult_3_param, app_conv_mult_2_input_data, app_conv_mult_3_input_data, app_conv_row_reduce_2_input_data);
  app_conv_mult_3(app_conv_mult_3_param, app_conv_mult_4_param, app_conv_mult_3_input_data, app_conv_mult_4_input_data, app_conv_row_reduce_3_input_data);
  app_conv_mult_4(app_conv_mult_4_param, app_conv_mult_5_param, app_conv_mult_4_input_data, app_conv_mult_5_input_data, app_conv_row_reduce_4_input_data);
  app_conv_mult_5(app_conv_mult_5_param, app_conv_mult_6_param, app_conv_mult_5_input_data, app_conv_mult_6_input_data, app_conv_row_reduce_5_input_data);
  app_conv_mult_6(app_conv_mult_6_param, app_conv_mult_7_param, app_conv_mult_6_input_data, app_conv_mult_7_input_data, app_conv_row_reduce_6_input_data);
  app_conv_mult_7(app_conv_mult_7_param, app_conv_mult_8_param, app_conv_mult_7_input_data, app_conv_mult_8_input_data, app_conv_row_reduce_7_input_data);
  app_conv_mult_8(app_conv_mult_8_param, app_conv_mult_9_param, app_conv_mult_8_input_data, app_conv_mult_9_input_data, app_conv_row_reduce_8_input_data);
  app_conv_mult_9(app_conv_mult_9_param, app_conv_mult_10_param, app_conv_mult_9_input_data, app_conv_mult_10_input_data, app_conv_row_reduce_9_input_data);
  app_conv_mult_10(app_conv_mult_10_param, app_conv_mult_11_param, app_conv_mult_10_input_data, app_conv_mult_11_input_data, app_conv_row_reduce_10_input_data);
  app_conv_mult_11(app_conv_mult_11_param, app_conv_mult_12_param, app_conv_mult_11_input_data, app_conv_mult_12_input_data, app_conv_row_reduce_11_input_data);
  app_conv_mult_12(app_conv_mult_12_param, app_conv_mult_13_param, app_conv_mult_12_input_data, app_conv_mult_13_input_data, app_conv_row_reduce_12_input_data);
  app_conv_mult_13(app_conv_mult_13_param, app_conv_mult_14_param, app_conv_mult_13_input_data, app_conv_mult_14_input_data, app_conv_row_reduce_13_input_data);
  app_conv_mult_14(app_conv_mult_14_param, app_conv_mult_15_param, app_conv_mult_14_input_data, app_conv_mult_15_input_data, app_conv_row_reduce_14_input_data);
  app_conv_mult_15(app_conv_mult_15_param, app_conv_mult_dummy_param, app_conv_mult_15_input_data, app_conv_mult_dummy_input_data, app_conv_row_reduce_15_input_data);
  app_conv_mult_dummy(app_conv_mult_dummy_param, app_conv_mult_dummy_input_data);

  app_conv_row_reduce_0(app_conv_row_reduce_0_input_data, app_conv_row_reduce_1_input_data, app_conv_combine_l0_input_data_0);
  app_conv_row_reduce_1(app_conv_row_reduce_2_input_data, app_conv_row_reduce_3_input_data, app_conv_combine_l0_input_data_1);
  app_conv_row_reduce_2(app_conv_row_reduce_4_input_data, app_conv_row_reduce_5_input_data, app_conv_combine_l0_input_data_2);
  app_conv_row_reduce_3(app_conv_row_reduce_6_input_data, app_conv_row_reduce_7_input_data, app_conv_combine_l0_input_data_3);
  app_conv_row_reduce_4(app_conv_row_reduce_8_input_data, app_conv_row_reduce_9_input_data, app_conv_combine_l0_input_data_4);
  app_conv_row_reduce_5(app_conv_row_reduce_10_input_data, app_conv_row_reduce_11_input_data, app_conv_combine_l0_input_data_5);
  app_conv_row_reduce_6(app_conv_row_reduce_12_input_data, app_conv_row_reduce_13_input_data, app_conv_combine_l0_input_data_6);
  app_conv_row_reduce_7(app_conv_row_reduce_14_input_data, app_conv_row_reduce_15_input_data, app_conv_combine_l0_input_data_7);

  app_conv_combine_l0_0(app_conv_combine_l0_input_data_0, app_conv_combine_l0_input_data_1, app_conv_combine_l1_input_data_0);
  app_conv_combine_l0_1(app_conv_combine_l0_input_data_2, app_conv_combine_l0_input_data_3, app_conv_combine_l1_input_data_1);
  app_conv_combine_l0_2(app_conv_combine_l0_input_data_4, app_conv_combine_l0_input_data_5, app_conv_combine_l1_input_data_2);
  app_conv_combine_l0_3(app_conv_combine_l0_input_data_6, app_conv_combine_l0_input_data_7, app_conv_combine_l1_input_data_3);

  app_conv_combine_l1_0(app_conv_combine_l1_input_data_0, app_conv_combine_l1_input_data_1, app_conv_combine_l2_input_data_0);
  app_conv_combine_l1_1(app_conv_combine_l1_input_data_2, app_conv_combine_l1_input_data_3, app_conv_combine_l2_input_data_1);

  app_conv_combine_l2_0(app_conv_combine_l2_input_data_0, app_conv_combine_l2_input_data_1, app_conv_writer_input_data);


  app_conv_writer(app_conv_writer_input_data, app_output_data);
}
