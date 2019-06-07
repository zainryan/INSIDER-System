#include <insider_itc.h>

#include "constant.h"
#include "structure.h"

#include "app_intg_mat_rdc.cpp"
#include "app_intg_matcher.cpp"
#include "app_intg_rdc_16to8.cpp"
#include "app_intg_rdc_32to16.cpp"
#include "app_intg_rdc_4to2.cpp"
#include "app_intg_rdc_8to4.cpp"
#include "app_intg_verifier.cpp"
#include "app_intg_writer.cpp"

void interconnects() {
  ST_Queue<APP_Match> app_intg_mat_rdc_input(64);
  ST_Queue<APP_Ver_Param> app_intg_verifier_input_param(64);
  ST_Queue<APP_Ver_Record> app_intg_verifier_input_record(64);
  ST_Queue<APP_Reduce_Record32> app_intg_rdc_input_32to16(64);
  ST_Queue<APP_Reduce_Record16> app_intg_rdc_input_16to8(64);
  ST_Queue<APP_Reduce_Record8> app_intg_rdc_input_8to4(64);
  ST_Queue<APP_Reduce_Record4> app_intg_rdc_input_4to2(64);
  ST_Queue<APP_Reduce_Record2> app_intg_writer_input_record(64);
  ST_Queue<unsigned int> app_intg_writer_input_param(64);

  app_intg_matcher(app_input_params, app_input_data,
                   app_intg_writer_input_param, app_intg_verifier_input_param,
                   app_intg_mat_rdc_input);
  app_intg_mat_rdc(app_intg_mat_rdc_input, app_intg_verifier_input_record);
  app_intg_verifier(app_intg_verifier_input_param,
                    app_intg_verifier_input_record, app_intg_rdc_input_32to16);

  app_intg_rdc_32to16(app_intg_rdc_input_32to16, app_intg_rdc_input_16to8);
  app_intg_rdc_16to8(app_intg_rdc_input_16to8, app_intg_rdc_input_8to4);
  app_intg_rdc_8to4(app_intg_rdc_input_8to4, app_intg_rdc_input_4to2);
  app_intg_rdc_4to2(app_intg_rdc_input_4to2, app_intg_writer_input_record);
  app_intg_writer(app_intg_writer_input_param, app_intg_writer_input_record,
                  app_output_data);
}
