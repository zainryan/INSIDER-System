#include <insider_itc.h>

#include "constant.h"
#include "structure.h"

#include "app_rlf_diff.cpp"
#include "app_rlf_dist.cpp"
#include "app_rlf_dspt.cpp"
#include "app_rlf_flt16.cpp"
#include "app_rlf_flt2.cpp"
#include "app_rlf_flt4.cpp"
#include "app_rlf_flt8.cpp"
#include "app_rlf_max_vec.cpp"
#include "app_rlf_rdc_16to8.cpp"
#include "app_rlf_rdc_2to1.cpp"
#include "app_rlf_rdc_4to2.cpp"
#include "app_rlf_rdc_8to4.cpp"
#include "app_rlf_stg.cpp"
#include "app_rlf_upd.cpp"
#include "app_rlf_wr.cpp"

// describe the interconnection
void interconnects() {

  ST_Queue<APP_Dist_Data> app_rlf_dist_input_data(8);
  ST_Queue<unsigned int> app_rlf_dist_input_params(8);
  ST_Queue<APP_Reduce_Data16> app_rlf_rdc_16to8_input_data(8);
  ST_Queue<APP_Reduce_Data8> app_rlf_rdc_8to4_input_data(8);
  ST_Queue<APP_Reduce_Data4> app_rlf_rdc_4to2_input_data(8);
  ST_Queue<APP_Reduce_Data2> app_rlf_rdc_2to1_input_data(8);
  ST_Queue<APP_Reduce_Data> app_rlf_max_vec_input_data(8);
  ST_Queue<bool> app_rlf_max_vec_init(8);
  ST_Queue<APP_Stg_Data> app_rlf_stg_input_data(8);
  ST_Queue<APP_Diff_Data> app_rlf_diff_input_data(8);
  ST_Queue<unsigned int> app_rlf_diff_input_params(8);
  ST_Queue<unsigned int> app_rlf_upd_input_params(8);
  ST_Queue<APP_Upd_Data> app_rlf_upd_input_data(8);
  ST_Queue<APP_Flt_Param> app_rlf_flt16_input_params(8);
  ST_Queue<APP_Flt_Data16> app_rlf_flt16_input_data(8);
  ST_Queue<APP_Flt_Data8> app_rlf_flt8_input_data(8);
  ST_Queue<APP_Flt_Data4> app_rlf_flt4_input_data(8);
  ST_Queue<APP_Flt_Data2> app_rlf_flt2_input_data(8);
  ST_Queue<APP_Write_Data> app_rlf_wr_input_data(8);

  app_rlf_dspt(app_input_params, app_input_data, app_rlf_upd_input_params,
               app_rlf_dist_input_params, app_rlf_diff_input_params,
               app_rlf_dist_input_data, app_rlf_flt16_input_data,
               app_rlf_max_vec_init);

  app_rlf_dist(app_rlf_dist_input_data, app_rlf_dist_input_params,
               app_rlf_rdc_16to8_input_data);
  app_rlf_rdc_16to8(app_rlf_rdc_16to8_input_data, app_rlf_rdc_8to4_input_data);
  app_rlf_rdc_8to4(app_rlf_rdc_8to4_input_data, app_rlf_rdc_4to2_input_data);

  app_rlf_rdc_4to2(app_rlf_rdc_4to2_input_data, app_rlf_rdc_2to1_input_data);
  app_rlf_rdc_2to1(app_rlf_rdc_2to1_input_data, app_rlf_max_vec_input_data);
  app_rlf_max_vec(app_rlf_max_vec_input_data, app_rlf_stg_input_data,
                  app_rlf_max_vec_init);
  app_rlf_stg(app_rlf_stg_input_data, app_rlf_diff_input_data);
  app_rlf_diff(app_rlf_diff_input_params, app_rlf_diff_input_data,
               app_rlf_upd_input_data);
  app_rlf_upd(app_rlf_upd_input_params, app_rlf_upd_input_data,
              app_rlf_flt16_input_params);
  app_rlf_flt16(app_rlf_flt16_input_params, app_rlf_flt16_input_data,
                app_rlf_flt8_input_data);
  app_rlf_flt8(app_rlf_flt8_input_data, app_rlf_flt4_input_data);
  app_rlf_flt4(app_rlf_flt4_input_data, app_rlf_flt2_input_data);
  app_rlf_flt2(app_rlf_flt2_input_data, app_rlf_wr_input_data);
  app_rlf_wr(app_rlf_wr_input_data, app_output_data);
}
